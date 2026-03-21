#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include <map>
#include <format>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <optional>
#include <cmath>

#include "structs.h"
#include "tokenize.h"

using namespace std;

// run command: g++ -std=c++20 engine.cpp -o engine.out && ./engine.out

//! IMPROVE: temp data --> replace with webscraped data later
// temp market data : day, [open, high, low, close, volume]
map<int, vector<int>> market_data = {  // ordered hashmap
    {1, {100, 103, 99, 102, 1000}},
    {2, {102, 104, 101, 103, 1100}},
    {3, {103, 105, 102, 104, 1200}},
    {4, {104, 106, 103, 105, 1500}},
    {5, {105, 107, 104, 106, 1800}},
    {6, {106, 108, 105, 107, 1600}},
    {7, {107, 108, 104, 105, 2000}},
    {8, {105, 106, 101, 102, 2200}},
    {9, {102, 103, 98, 99, 2500}},
    {10, {99, 101, 95, 97, 2800}},
    {11, {98, 100, 94, 96, 2700}},
    {12, {97, 99, 93, 95, 2600}},
    {13, {96, 98, 92, 94, 2550}},
    {14, {95, 97, 91, 93, 2500}},
    {15, {94, 96, 90, 92, 2450}},
};

// check of conditions match parent operator
bool validate_condition_operator(
    bool con_a, bool con_b, string logic_op
) {

    if (logic_op == "AND") {
        return con_a && con_b;
    }
    else {  // case: OR
        return con_a || con_b;
    }

}

// get market data index from hashmap layout
int get_data_index(string source) {

    int source_index{};

    // map source index based on market data position
    if (source == "open") {
        source_index = 0;
    } else if (source == "close") {
        source_index = 3;
    } else if (source == "high") {
        source_index = 2;
    } else if (source == "low") {
        source_index = 1;
    } else {
        source_index = 4;  // volume
    }

    return source_index;

}

// compute operand values from condition parts
float evaluate_condition_vals(
    int curr_day, 
    string operand, 
    map<string, float> current_values, 
    map<int, vector<int>> market_data,
    array<string, 5> bar_fields,
    optional<float> entry_price = nullopt  //! fix later (hardcoded)
) {

    auto search_it = find(begin(bar_fields), end(bar_fields), operand);  // search for operand
    
    // TODO: be able to handle equations too
    //! remove hardcoded condition for exit equation --> need to breakdown somehow
    if (entry_price.has_value()) {  // case: sub-equation
        return entry_price.value() * 0.97;  //! hardcoded part
    } else if (search_it != end(bar_fields)) {  // case: in bar fields
        int source_index = get_data_index(operand);  // get op index from data
        return market_data[curr_day][source_index];
    } else {  // case: in curr vals map
        return current_values[operand];
    }

}

// check if condition expression is true/false (crossover/crossunder)
bool cross_condition_operation(
    float left_op_val, float right_op_val, string operation,
    int curr_day, string left_operand, string right_operand, 
    map<string, float> previous_values, 
    map<int, vector<int>> market_data, 
    array<string, 5> bar_fields
) {

    int prev_day = curr_day - 1;

    // using previous day --> "current values" equiv
    float prev_left_val = evaluate_condition_vals(  // prev day left op val
        prev_day, left_operand, previous_values, market_data, bar_fields
    );
    float prev_right_val = evaluate_condition_vals(  // prev day right op val
        prev_day, right_operand, previous_values, market_data, bar_fields
    );

    // validate comparison ops
    if (operation == "crossunder") {  // crossover --> left <= right prev, left > right curr
        if (prev_left_val <= prev_right_val && left_op_val > right_op_val) {
            return true;
        }
    } else {  // crossunder --> left >= right prev, left < right curr
        if (prev_left_val >= prev_right_val && left_op_val < right_op_val) {
            return true;
        }
    }
    return false;  // op not true
}

// check if condition expression is true/false (only >, <, >=, <= ops)
bool basic_condition_operation(
    float left_op_val, float right_op_val, string operation
) {

    // validate comparison ops
    if (operation == ">") {
        if (left_op_val > right_op_val) {
            return true;
        }
    } else if (operation == ">=") {
        if (left_op_val >= right_op_val) {
            return true;
        }
    } else if (operation == "<") {
        if (left_op_val < right_op_val) {
            return true;
        }
    } else {  // case: "<="
        if (left_op_val <= right_op_val) {
            return true;
        }
    }
    return false;  // op not true
}

// compute indicator values from strategy
float compute_indicator(
    map<int, vector<int>> market_data, 
    int curr_day, string type, 
    string source, int period
) {

    float value{};
    int source_index = get_data_index(source);  // get data index

    // define time interval for compute
    int start_day = curr_day - period + 1;
    int end_day = curr_day;

    // base on indicator type
    if (type == "SMA") {
        float total_sum{};  // 0.0f
        for (int i = start_day; i <= end_day; i++) {
            total_sum += market_data[i][source_index];  // day, bar field
        }
        value = total_sum / period;  // short sma 
    }

    return value;

}

// break line into vector parts
vector<string> str_to_vec(string text_line) {

    vector<string> split_parts;
    string s;  // curr token
    stringstream stream(text_line);  // input stream
    
    while (stream >> s) {  // bitwise char read
        split_parts.push_back(s);  // append
    }

    return split_parts;

}

// handle operation function calls (consolidate)
bool operation_handler(
    string operation, int curr_day, 
    string left_operand, string right_operand, 
    map<string, float> current_values,
    map<string, float> previous_values, 
    map<int, vector<int>> market_data, 
    array<string, 5> bar_fields,
    optional<float> entry_price = nullopt  //! fix later (hardcoded)
) {

    float right_val{};
    float left_val = evaluate_condition_vals(  // left op val
        curr_day, left_operand, current_values, market_data, bar_fields
    );
    if (entry_price.has_value()) {  // special case
        right_val = evaluate_condition_vals(  // right op val
            curr_day, right_operand, current_values, 
            market_data, bar_fields, entry_price  // optional entry price
        );
    } else {
        right_val = evaluate_condition_vals(  // right op val
            curr_day, right_operand, current_values, 
            market_data, bar_fields
        );
    }

    bool condition_res{};
    // cout << "operation: " << operation << endl;
    if (operation == "crossover" || operation == "crossunder") {  // case: cross ops
        cout << "Rule (Cross): " + operation << endl;
        condition_res = cross_condition_operation(
            left_val, right_val, operation,
            curr_day, left_operand, right_operand, 
            previous_values, market_data, bar_fields
        );
    } else {  // case: (>, <, >=, <=) --> not cross ops
        cout << "Rule (Basic): " + to_string(left_val) + " " + operation + " " + to_string(right_val) << endl;
        condition_res = basic_condition_operation(left_val, right_val, operation);
    }

    return condition_res;  // store bool

}

// compute share quantity to use for trade
float compute_share_quantity(
    float entry_price, float capital, float size_percentage
) {

    float dollars_to_put = capital * (size_percentage / 100);  // from capital
    int num_shares = static_cast<int>(round(dollars_to_put / entry_price));  // no user warning, floor round
    return num_shares;

}

// compute pnl from current entry/exit cycle
float compute_current_pnl(
    int share_quantity, float entry_price, 
    float exit_price, string asset_direction
) {

    float current_pnl{};
    if (asset_direction == "long") {  // case: long term
        current_pnl = (exit_price - entry_price) * share_quantity;
    } else {  // case: short term (flip order)
        current_pnl = (entry_price - exit_price) * share_quantity;
    }

    cout << "Entry Price: " << entry_price << endl;
    cout << "Exit Price: " << exit_price << endl;
    cout << "Shares: " << share_quantity << endl;
    cout << "Current PnL: $" << current_pnl << endl;
    return current_pnl;

}

// compute percent difference in pnl from initial capital
float compute_pnl_percentage_diff(float remaining_capital, float capital) {

    float price_diff = capital - remaining_capital;
    return (price_diff / capital) * 100;  // 1-100% scale

}

// deconstruct function format
auto breakdown_function_format(string function) {

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

    // get arguments
    string arg_a = args[0];
    string arg_b = args[1];

    return make_tuple(name, arg_a, arg_b);

}

// parse strategy tokens
auto parse_strategy_tokens(vector<Token> tokens, size_t pos) {

    string strat_name{};

    while (pos < tokens.size() && tokens[pos].value == "STRATEGY") {

        pos++;  // pass keyword

        if (tokens[pos].type != TokenType::NAME) {  // case: format failure
            cout << "Strategy name format is invalid." << endl;
            break;
        }
        strat_name = tokens[pos].value;  // strat name
        pos++;

    }

    return make_tuple(pos, strat_name);

}

// parse indicator tokens
auto parse_indicator_tokens(vector<Token> tokens, size_t pos) {

    vector<Indicators> indicators;  // store sub-structs

    while (pos < tokens.size() && tokens[pos].value == "INDICATOR") {

        pos++;  // pass keyword

        if (tokens[pos].type != TokenType::VARIABLE) {  // case: format failure
            cout << "Indicator variable format is invalid." << endl;
            break;
        }
        string name = tokens[pos].value;  // variable name
        pos++;

        if (tokens[pos].type != TokenType::ASSIGNMENT) {  // case: format failure
            cout << "Indicator assignment format is invalid." << endl;
            break;
        }
        pos++;

        if (tokens[pos].type != TokenType::FUNCTION) {  // case: format failure
            cout << "Indicator function format is invalid." << endl;
            break;
        }
        auto [type, source, period] = breakdown_function_format(tokens[pos].value);  // parse function
        pos++;

        Indicators indicator(name, type, source, stoi(period));
        indicators.push_back(indicator);

    }

    return make_tuple(pos, indicators);

}

// parse entry rule tokens
auto parse_entry_tokens(vector<Token> tokens, size_t pos) {
  
    vector<Conditions> conditions;
    string logic_op{};
    string direction{};

    while (pos < tokens.size() && tokens[pos].value == "ENTRY") {

        pos++;  // pass keyword

        if (tokens[pos].type != TokenType::COVERAGE) {  // case: format failure
            cout << "Entry rule coverage format is invalid." << endl;
            break;
        }
        direction = tokens[pos].value;  // variable name
        pos++;

        //! currently only handles our exact entry rule (cond1 AND comparison)
        if (
            tokens[pos].type == TokenType::FUNCTION ||  // func
            tokens[pos].type == TokenType::VARIABLE ||  // variable
            tokens[pos].type == TokenType::LOGIC_OPERATOR  // logic op
        ) {
            if (tokens[pos].type == TokenType::FUNCTION) {
                auto [operation, left_op, right_op] = breakdown_function_format(tokens[pos].value);  // parse function
                Conditions condition(
                    operation,  // operation
                    left_op,  // left op
                    right_op  // right op
                );
                conditions.push_back(condition);
                pos++;
            } 
            if (tokens[pos].type == TokenType::LOGIC_OPERATOR) {
                logic_op = tokens[pos].value;  // AND/OR
                pos++;

            } 
            if (tokens[pos].type == TokenType::VARIABLE) {  // variable
                Conditions condition(
                    tokens[pos+1].value,  // operation
                    tokens[pos].value,  // left op
                    tokens[pos+2].value  // right op
                );
                conditions.push_back(condition);
                pos += 3;  //! fix hardcode forward
            }
        } else {
            cout << "Entry rule condition format is invalid." << endl;
            break;
        }

    }

    EntryRule entry_rule(
        direction,  // direction
        conditions,  // conditions
        logic_op  // logic op
    );

    return make_tuple(pos, entry_rule);

}

// parse exit rule tokens
auto parse_exit_tokens(vector<Token> tokens, size_t pos) {

    vector<Conditions> conditions;
    vector<ExitRules> exit_conditions;  // store sub-structs
    string action{};
    string expression{};  // arithmitic

    while (pos < tokens.size() && tokens[pos].value == "EXIT") {

        pos++;  // pass keyword

        if (tokens[pos].type != TokenType::COVERAGE) {  // case: format failure
            cout << "Indicator coverage format is invalid." << endl;
            break;
        }
        action = tokens[pos].value;  // coverage
        pos++;

        if (tokens[pos].type == TokenType::FUNCTION) {  // case: simple function
            auto [operation, left_op, right_op] = breakdown_function_format(tokens[pos].value);  // parse function
            Conditions condition(
                operation,  // operation
                left_op,  // left op
                right_op  // right op
            );
            conditions.push_back(condition);
            pos++;
        } else {
            //! need to update later for expression in condition, currently hardcoded-ish
            expression = format(  // concatenate
                "{} {} {}",  // a * b
                tokens[pos+3].value,
                tokens[pos+2].value, 
                tokens[pos+4].value
            );
            Conditions condition(
                tokens[pos+1].value,  // operation
                tokens[pos].value,  // left op
                expression  // right op (a * b)
            );
            conditions.push_back(condition);
            pos += 5;  //! fix hardcode forward
        }

        ExitRules exit_rule(action, conditions);
        exit_conditions.push_back(exit_rule);
        conditions.clear();  // clear for next exit rule

    }

    return make_tuple(pos, exit_conditions);

}

// parse position tokens
auto parse_position_tokens(vector<Token> tokens, size_t pos) {
  
    vector<string> pos_vars;

    while (pos < tokens.size() && tokens[pos].value == "POSITION") {

        pos++;  // pass keyword

        if (tokens[pos].type != TokenType::VARIABLE) {  // case: format failure
            cout << "Position variable format is invalid." << endl;
            break;
        }
        pos++;

        if (tokens[pos].type != TokenType::ASSIGNMENT) {  // case: format failure
            cout << "Position assignment format is invalid." << endl;
            break;
        }
        pos++;

        if (
            tokens[pos].type == TokenType::INTEGER_LITERAL ||  // int
            tokens[pos].type == TokenType::FLOAT_LITERAL ||  // float
            tokens[pos].type == TokenType::BOOL_LITERAL  // bool
        ) {
            pos_vars.push_back(tokens[pos].value);
            pos++;
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
        stoi(pos_vars[1]),  // max pos
        allow  // allow short
    );

    return make_tuple(pos, pos_settings);

}

// set parsed token rules to strategy struct members
Strategy parse_rules(vector<Token> tokens) {  // ref to modify -> not copy

    size_t pos{};  // global

    // default construct to update later
    string name{};
    vector<Indicators> indicators{{"", "", "", 0}};
    EntryRule entry_rule{"", {}, ""};
    vector<ExitRules> exit_rules{{"", {}}};
    PositionSettings pos_settings{0.0f, 0, false};

    while (pos < tokens.size()) {  // token objects (type, value) 
        if (tokens[pos].type == TokenType::KEYWORD) {
            if (tokens[pos].value == "STRATEGY") {
                cout << "Working on assigning STRATEGY struct." << endl;
                auto [new_pos, parsed_name] = parse_strategy_tokens(tokens, pos);
                name = std::move(parsed_name);  // move resource --> prevent copy
                pos = new_pos;
                continue;
            } else if (tokens[pos].value == "INDICATOR") {
                cout << "Working on assigning INDICATOR struct." << endl;
                auto [new_pos, parsed_indicators] = parse_indicator_tokens(tokens, pos);
                indicators = std::move(parsed_indicators);
                pos = new_pos;
                continue;
            } else if (tokens[pos].value == "ENTRY") {
                cout << "Working on assigning ENTRY struct." << endl;
                auto [new_pos, parsed_entry_rule] = parse_entry_tokens(tokens, pos);
                entry_rule = std::move(parsed_entry_rule);
                pos = new_pos;
                continue;
            } else if (tokens[pos].value == "EXIT") {
                cout << "Working on assigning EXIT struct." << endl;
                auto [new_pos, parsed_exit_conditions] = parse_exit_tokens(tokens, pos);
                exit_rules = std::move(parsed_exit_conditions);
                pos = new_pos;
                continue;
            } else if (tokens[pos].value == "POSITION") {
                cout << "Working on assigning POSITION struct." << endl;
                auto [new_pos, parsed_pos_settings] = parse_position_tokens(tokens, pos);
                pos_settings = std::move(parsed_pos_settings);
                pos = new_pos;
                continue;
            } else {
                cout << "All struct members have been created." << endl;
                break;
            }
        } else {
            cout << "End of valid strategy tokens reached." << endl;
            break;
        }

    }

    // populate strat constructor
    Strategy strategy(
        name,
        indicators,
        entry_rule,
        exit_rules,
        pos_settings
    );

    return strategy;

}

// read lines from strategy file
Strategy read_strat(string filename) {  // ref to struct

    // pull temp backtesting strategy file
    string strat_text;
    ifstream strat_file(filename);  // read, name, file
    string full_text{};

    while (getline(strat_file, strat_text)) {
        full_text += strat_text + "\n";  // one large string
    }

    LexicalTokenParser token_parser(full_text);  // parse text
    vector<Token> tokens = token_parser.tokenize();  // tokenize
    Strategy strategy = parse_rules(tokens);  // assign tokens to strategy struct

    cout << "\n============================\n" << endl;
    cout << "PARSED STRATEGY:\n" << endl;
    cout << full_text << endl;
    cout << "============================\n" << endl;

    strat_file.close();  // for memory space

    return strategy;  // struct with members

}

int main() {

    cout << "Backtesting Engine!\n" << endl;

    string filename = "./improved_temp_strat.txt";
    Strategy strategy = read_strat(filename);  // built strategy struct
    cout << "Strategy struct's members have been populate with file attributes!\n" << endl;

    // store updates indicator values
    map<string, float> current_values;
    map<string, float> previous_values;
    float value{};
    int min_start_day{};  // start to 0

    bool in_entry_cycle = false;
    float pnl_update_val{};  // track entry session
    float final_pnl{};  // final profit/loss

    float unrealized_pnl{};  // exit
    float realized_pnl{};  // in trade

    float price_entered_at{};
    float price_exited_at{};

    // portfolio info
    float capital = 100000.0f;  // $$$
    float share_quantity{};
    cout << "Initial Capital: $" << capital << endl;

    // immutable array to define possible bar fields
    const array<string, 5> bar_fields = {"open", "high", "low", "close", "volume"};

    // get minimum start day needed for compute
    for (auto indicator : strategy.indicators) {
        if (min_start_day < indicator.period) {
            min_start_day = indicator.period;  // update
        }
    }

    // point to last day from ordered hashmap
    auto last_day_ptr = prev(market_data.end());  // end = after last --> no data, prev
    int last_day_in_data = last_day_ptr->first;  // deref

    // print each data point from hashmap
    for (auto market_val : market_data) {  // auto data type
        
        // map market data to variables init
        int curr_day = market_val.first;
        float open_val = market_val.second[0];
        float high_val = market_val.second[1];
        float low_val = market_val.second[2];
        float close_val = market_val.second[3];
        int total_volume = market_val.second[4];

        cout << "\nDay: " << curr_day << 
        " | Open: " << open_val << 
        " | High: " << high_val << 
        " | Low: " << low_val << 
        " | Close: " << close_val << 
        " | Volume: " << total_volume << 
        endl; 

        // skip until valid day for indicator compute
        if (curr_day < min_start_day) {
            cout << "No signal yet. Need more information. Skipping current day." << endl;
            continue;
        }

        // compute indicator values per indicator type
        for (auto indicator : strategy.indicators) {
            value = compute_indicator(
                market_data, curr_day, indicator.type, indicator.source, indicator.period
            );
            current_values[indicator.name] = value;  // store
            cout << "Indicator " + indicator.name + ": " + to_string(value) << endl;
        }

        vector<bool> entry_condition_outcomes{};  // store condition true/false

        // check if entry rule is valid
        for (auto condition : strategy.entry_rule.conditions) {
            string left_operand = condition.left_operand;
            string right_operand = condition.right_operand;
            string operation = condition.operation;  // forms --> (left operator right)
            bool entry_condition_res = operation_handler(
                operation, curr_day, left_operand, right_operand, 
                current_values, previous_values, market_data, bar_fields
            );

            cout << "Entry Condition (0,1): " << entry_condition_res << endl;
            entry_condition_outcomes.push_back(entry_condition_res);  // store bool

        //! need to improve to handle multiple conditions and operators (currently only 2) -> bad long term
        // check entry rule conditions with operator
        if (validate_condition_operator(
            entry_condition_outcomes[0], 
            entry_condition_outcomes[1], 
            strategy.entry_rule.logic_operator  // AND/OR
        ) && !in_entry_cycle) {  // if not already entered
            cout << "Day: " << curr_day << " passed the entry rule...entering trade." << endl;
            pnl_update_val = 0.0;
            price_exited_at = 0.0;
            price_entered_at = open_val;  // bar entry
            in_entry_cycle = true;
        }

        }

        // check if currently entered trade
        if (in_entry_cycle) {
            cout << "Inside of an entry cycle trade..." << endl;

            vector<bool> exit_condition_outcomes{};  // store condition true/false

            // check exit conditions incase trade is invalid
            for (auto exit_rule : strategy.exit_rules) {
                cout << "Exit rule action: " + exit_rule.action << endl;
                for (auto condition : exit_rule.conditions) {                    
                    string left_operand = condition.left_operand;
                    string right_operand = condition.right_operand;
                    string operation = condition.operation;  // forms --> (left operator right)

                    //! remove hardcoded condition for exit equation --> need to breakdown somehow
                    bool exit_condition_res{};
                    if (right_operand == "entry_price*0.97") {  // case: sub-equation
                        exit_condition_res = operation_handler(
                            operation, curr_day, left_operand, right_operand, 
                            current_values, previous_values, market_data, 
                            bar_fields, price_entered_at  // optional entry price
                        );
                    } else {
                        exit_condition_res = operation_handler(
                            operation, curr_day, left_operand, right_operand, 
                            current_values, previous_values, market_data, bar_fields
                        );
                    }
                    cout << "Exit Condition (0,1): " << exit_condition_res << endl;
                    exit_condition_outcomes.push_back(exit_condition_res);  // store bool
                }
            }

            // any instance of exit rule being true
            bool exit_true = any_of(  // lambda func
                exit_condition_outcomes.begin(), 
                exit_condition_outcomes.end(), 
                [](bool b){ return b; }  // bool val
            );

            // compute num shares to use in trade
            share_quantity = compute_share_quantity(
                price_entered_at, capital, strategy.pos_settings.size_percentage
            );

            // state conditionals during entry
            if (curr_day == last_day_in_data) {  // case: hit last day (while still in trade)
                cout << "End of search days reached while still in trade." << endl;
                final_pnl += pnl_update_val;  // add to final pnl
            }
            else if (exit_true) {  // case: exit rule hit
                cout << "Exit rule has been hit while in trade." << endl;
                price_exited_at = close_val;  // bar close
                realized_pnl = compute_current_pnl(
                    share_quantity, price_entered_at, 
                    price_exited_at, strategy.entry_rule.direction
                );
                final_pnl += realized_pnl;  // add to final pnl
                pnl_update_val = 0.0;  // not in trade
                price_entered_at = 0.0;
                in_entry_cycle = false;  // exit cycle
            } else {  // case: continue
                cout << "Still in trade. Adding unrealized pnl." << endl;
                unrealized_pnl = compute_current_pnl(
                    share_quantity, price_entered_at, 
                    close_val, strategy.entry_rule.direction
                );
                pnl_update_val += unrealized_pnl;  // running cycle sum

            }
        }

        // update previous day values
        previous_values = current_values;  // after using

    }

    float remaining_capital = capital + final_pnl;
    float capital_diff_percent = compute_pnl_percentage_diff(remaining_capital, capital);  // % pnl

    cout << "\nFinal PnL: $" << final_pnl << endl;
    cout << "Remaining Capital: $" << remaining_capital << endl;

    if (remaining_capital < capital) {  // case: loss
        cout << "Percent Difference in Capital: (-" << capital_diff_percent << "%)" << endl;
    } else {  // case: gain
        cout << "Percent Difference in Capital: (+" << capital_diff_percent << "%)" << endl;
    }

    return 0;
}