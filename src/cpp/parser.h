#ifndef PARSER_TEXT_H
#define PARSER_TEXT_H

#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include <format>
#include <sstream>
#include <algorithm>

#include "structs.h"

using namespace std;

// token parser for struct member assignment
class TokenParser {

private:  // local

    size_t position;
    vector<Token> tokens;

    // deconstruct function format
    auto breakdown_function_format(string& function) {

        // isolate name and arguments from delims
        size_t open_index = function.find('(');
        string name = function.substr(0, open_index);  // start, length
        size_t close_index = function.find(')');
        string args_str = function.substr(open_index + 1, close_index - open_index - 1);

        vector<string> args;
        stringstream stream(args_str);
        string arg;
        char delim = ',';

        while (getline(stream, arg, delim)) {  // breakdown inner delims
            args.push_back(arg);
        }

        string args_b{};
        if (!args[1].empty() && args[1][0] == ' ') {
            args_b = args[1].substr(1);  // skip whitespace
        }

        return make_tuple(name, args[0], args_b);

    }

    // parse strategy tokens
    string parse_strategy_tokens() {

        string strat_name{};

        while (position < tokens.size() && tokens[position].value == "STRATEGY") {

            position++;  // pass keyword

            if (tokens[position].type != TokenType::NAME) {  // case: format failure
                cout << "Strategy name format is invalid." << endl;
                break;
            }
            strat_name = tokens[position].value;  // strat name
            position++;

        }

        return strat_name;

    }

    // parse indicator tokens
    vector<Indicators> parse_indicator_tokens() {

        vector<Indicators> indicators;  // store sub-structs

        while (position < tokens.size() && tokens[position].value == "INDICATOR") {

            position++;  // pass keyword

            if (tokens[position].type != TokenType::VARIABLE) {  // case: format failure
                cout << "Indicator variable format is invalid." << endl;
                break;
            }
            string name = tokens[position].value;  // variable name
            position++;

            if (tokens[position].type != TokenType::ASSIGNMENT) {  // case: format failure
                cout << "Indicator assignment format is invalid." << endl;
                break;
            }
            position++;

            if (tokens[position].type != TokenType::FUNCTION) {  // case: format failure
                cout << "Indicator function format is invalid." << endl;
                break;
            }
            auto [type, source, period] = breakdown_function_format(tokens[position].value);  // parse function
            position++;

            Indicators indicator(name, type, source, stoi(period));
            indicators.push_back(indicator);

        }

        return indicators;

    }

    // parse entry rule tokens
    vector<EntryRules> parse_entry_tokens() {
    
        vector<Conditions> conditions;
        vector<EntryRules> entry_rules;  // store sub-structs
        string logic_op{};
        string direction{};

        while (position < tokens.size() && tokens[position].value == "ENTRY") {

            position++;  // pass keyword

            if (tokens[position].type != TokenType::COVERAGE) {  // case: format failure
                cout << "Entry rule coverage format is invalid." << endl;
                break;
            }
            direction = tokens[position].value;  // variable name
            position++;

            //! changed to have entry on multiple lines but needs adjustments
            if (tokens[position].type == TokenType::FUNCTION) {
                auto [operation, left_op, right_op] = breakdown_function_format(tokens[position].value);  // parse function
                Conditions condition(
                    operation,  // operation
                    left_op,  // left op
                    right_op  // right op
                );
                conditions.push_back(condition);
                position++;
            } else {  // case: comparison (non-func)
                Conditions condition(
                    tokens[position+1].value,  // operation
                    tokens[position].value,  // left op
                    tokens[position+2].value  // right op
                );
                conditions.push_back(condition);
                position += 3;  //! fix hardcode forward
            }

            EntryRules entry_rule(
                direction,  // direction
                conditions,  // conditions
                logic_op  // logic op
            );
            entry_rules.push_back(entry_rule);

        }

        return entry_rules;

    }

    // parse exit rule tokens
    vector<ExitRules> parse_exit_tokens() {

        vector<Conditions> conditions;
        vector<ExitRules> exit_rules;  // store sub-structs
        string action{};
        string expression{};  // arithmitic

        while (position < tokens.size() && tokens[position].value == "EXIT") {

            position++;  // pass keyword

            if (tokens[position].type != TokenType::COVERAGE) {  // case: format failure
                cout << "Indicator coverage format is invalid." << endl;
                break;
            }
            action = tokens[position].value;  // coverage
            position++;

            if (tokens[position].type == TokenType::FUNCTION) {  // case: simple function
                auto [operation, left_op, right_op] = breakdown_function_format(tokens[position].value);  // parse function
                Conditions condition(
                    operation,  // operation
                    left_op,  // left op
                    right_op  // right op
                );
                conditions.push_back(condition);
                position++;
            } else {
                //! need to update later for expression in condition, currently hardcoded-ish
                expression = format(  // concatenate
                    "{} {} {}",  // a * b
                    tokens[position+2].value,  // left op
                    tokens[position+3].value,  // operation
                    tokens[position+4].value  // right op
                );
                Conditions condition(
                    tokens[position+1].value,  // operation
                    tokens[position].value,  // left op
                    expression  // right op (a * b)
                );
                conditions.push_back(condition);
                position += 5;  //! fix hardcode forward
            }

            ExitRules exit_rule(action, conditions);
            exit_rules.push_back(exit_rule);
            conditions.clear();  // clear for next exit rule

        }

        return exit_rules;

    }

    // parse position tokens
    PositionSettings parse_position_tokens() {
    
        vector<string> pos_vars;

        while (position < tokens.size() && tokens[position].value == "POSITION") {

            position++;  // pass keyword

            if (tokens[position].type != TokenType::VARIABLE) {  // case: format failure
                cout << "Position variable format is invalid." << endl;
                break;
            }
            position++;

            if (tokens[position].type != TokenType::ASSIGNMENT) {  // case: format failure
                cout << "Position assignment format is invalid." << endl;
                break;
            }
            position++;

            if (
                tokens[position].type == TokenType::INTEGER_LITERAL ||  // int
                tokens[position].type == TokenType::FLOAT_LITERAL ||  // float
                tokens[position].type == TokenType::BOOL_LITERAL  // bool
            ) {
                pos_vars.push_back(tokens[position].value);
                position++;
            } else {
                cout << "Position variable format is invalid." << endl;
                break;
            }

        }

        // string to bool
        bool allow{};
        if (pos_vars[2] == "false") {
            allow = false;
        } else {
            allow = true;
        }

        PositionSettings pos_settings(
            stof(pos_vars[0]),  // size %
            stoi(pos_vars[1]),  // max position
            allow  // allow short
        );

        return pos_settings;

    }

public:  // callable outside

    TokenParser(vector<Token>& t) : tokens(t), position(0) {};  // constructor

    // set parsed token rules to strategy struct members
    Strategy parse_rules() {

        // default construct to update later
        string name{};
        vector<Indicators> indicators{{"", "", "", 0}};
        vector<EntryRules> entry_rules{{"", {}, ""}};
        vector<ExitRules> exit_rules{{"", {}}};
        PositionSettings pos_settings{0.0f, 0, false};

        while (position < tokens.size()) {  // token objects (type, value) 
            if (tokens[position].type == TokenType::KEYWORD) {

                // populate struct members
                if (tokens[position].value == "STRATEGY") {
                    cout << "Working on assigning STRATEGY struct." << endl;
                    name = parse_strategy_tokens();
                } else if (tokens[position].value == "INDICATOR") {
                    cout << "Working on assigning INDICATOR struct." << endl;
                    indicators = parse_indicator_tokens();
                } else if (tokens[position].value == "ENTRY") {
                    cout << "Working on assigning ENTRY struct." << endl;
                    entry_rules = parse_entry_tokens();
                } else if (tokens[position].value == "EXIT") {
                    cout << "Working on assigning EXIT struct." << endl;
                    exit_rules = parse_exit_tokens();
                } else if (tokens[position].value == "POSITION") {
                    cout << "Working on assigning POSITION struct." << endl;
                    pos_settings = parse_position_tokens();
                } else {
                    cout << "All struct members have been created." << endl;
                    break;
                }

            } else {
                cout << "End of valid strategy tokens reached." << endl;
                break;
            }

        }

        // construct stragey object
        Strategy strategy(
            name,
            indicators,
            entry_rules,
            exit_rules,
            pos_settings
        );

        return strategy;

    }
};

#endif