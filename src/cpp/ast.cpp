#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <tuple>
#include <fstream>
#include <sstream>

using namespace std;

// run command: g++ -std=c++20 ast.cpp -o ast.out && ./ast.out

// define token types
enum class TokenType {  // enum --> type saftey
    KEYWORD,  // INDICATOR, EXIT
    NAME,  // sma_crossover_with_volume
    ASSIGNMENT,  // =
    FUNCTION,  // SMA(close, 3)
    VARIABLE,  // short_sma, long_sma
    INTEGER_LITERAL,  // 1, 2, 3
    FLOAT_LITERAL,  // 0.0, 1.0
    BOOL_LITERAL,  // true, false
    DELIMETER,  // ()
    LOGIC_OPERATOR,  // AND, OR
    ARITHMITIC_OPERATOR,  // +, -, *, /
    COMPARISON_OPERATOR,  // <, >
    COVERAGE,  // long, close_position
    UNKNOWN  // invalid
};

// token definition
struct Token {
    TokenType type;
    string value;
    Token(TokenType t, const string& v) : type(t), value(v) {}  // constructor
};

// lexical parser for tokenization
class LexicalTokenParser {

private:  // local

    string input;
    size_t position;
    unordered_map<string, TokenType> keywords;

    void init_keywords() {
        keywords["STRATEGY"] = TokenType::KEYWORD;
        keywords["INDICATOR"] = TokenType::KEYWORD;
        keywords["ENTRY"] = TokenType::KEYWORD;
        keywords["EXIT"] = TokenType::KEYWORD;
        keywords["POSITION"] = TokenType::KEYWORD;
    }

    bool is_whitespace(const char& c) {  // blanks
        return (c == ' ' || c == '\n');
    }

    bool is_alpha(const char& c) {  // letters
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }

    bool is_digit(const char& c) {  // nums
        return (c >= '0' && c <= '9');
    }

    bool is_alpha_numeric(const char& c) {  // letters + nums
        return (is_alpha(c) || is_digit(c));
    }

    bool is_delimiter(const char& c) {  // brackets
        return (c == '(' || c == ')');
    }

    bool is_comma(const char& c) {  // commas
        return (c == ',');
    }

    bool is_period(const char& c) {  // periods
        return (c == '.');
    }

    bool is_underscore(const char& c) {  // underscore
        return (c == '_');
    }

    bool is_assignment(const char& c) {  // equals
        return (c == '=');
    }

    bool is_comparison_operator(const string& s) {  // comparison
        return (s == ">" || s == "<" || s == ">=" || s == "<=");
    }

    bool is_special_character(const char& c) {  // special
        return (
            is_delimiter(c) || 
            is_comma(c) || 
            is_underscore(c) || 
            is_period(c)
        );
    }

    bool is_logic_operator(const string& s) {  // logic
        return (s == "AND" || s == "OR");
    }

    bool is_boolean_literal(const string& s) {  // bool
        return (s == "true" || s == "false");
    }

    bool is_arithmitic_operator(const char& c) {  // asmd
        return (c == '+' || c == '-' || c == '*' || c == '/');
    }

    // get complete word after first letter
    string get_next_word() {

        size_t start = position;

        while ((position < input.length()) && (
            is_alpha_numeric(input[position]) || 
            input[position] == '_'
        )) {
            position++;
        }

        return input.substr(start, (position - start));

    }

    // get complete number after first num
    string get_next_number() {

        size_t start = position;
        bool has_decimal = false;

        while ((position < input.length()) && (
            is_digit(input[position]) || 
            input[position] == '.'
        )) {
            if (input[position] == '.') {
                if (has_decimal) {  // case: floats
                    break;
                }
                has_decimal = true;
            }
            position++;
        }

        return input.substr(start, (position - start));

    }

    // get functions inside of brackets
    string get_next_function() {

        size_t start = position;
        string prefix{};
        size_t pos_changed{};

        if (position > 0) {  // pos-1
            while ((is_alpha_numeric(input[position-1]))) {  // before first bracket
                position--;
                pos_changed++;  // track
            }
            position += pos_changed;  // snap back pos
        }

        while ((position < input.length()) && (
            is_alpha_numeric(input[position]) || 
            is_whitespace(input[position]) || 
            is_special_character(input[position]) ||
            is_arithmitic_operator(input[position])
        )) {
            if (input[position] == ')') {  // case: end bracket
                position++;
                break;
            }
            position++;
        }

        return input.substr(start - pos_changed, (position - start + pos_changed));  // includes func name
    
    }

    // // get equation inside of brackets
    // string get_next_equation() {

    //     size_t start = position;
    //     bool end_delimiter = false;
        
    //     while ((position < input.length()) && (
    //         is_alpha_numeric(input[position]) || 
    //         is_whitespace(input[position]) || 
    //         is_special_character(input[position]) ||
    //         is_arithmitic_operator(input[position]) ||
    //         is_comparison_operator(string(1, input[position]))
    //     )) {
    //         if (input[position] == ')') {  // case: end bracket
    //             if (end_delimiter) {
    //                 break;
    //             }
    //             end_delimiter = true;
    //         }
    //         position++;
    //     }

    //     return input.substr(start, (position - start));
    
    // }
    
    bool is_variable(const string& s) {  // strat vars
        return (
            s == "short_sma" ||
            s == "long_sma" ||
            s == "vol_avg" ||
            s == "size_percent" ||
            s == "max_positions" ||
            s == "allow_short" ||
            s == "open" ||
            s == "close" || 
            s == "high" ||
            s == "low" ||
            s == "volume"
        );
    } 
    
    // TODO: remove later --> replacing with a delimit decider func
    // bool is_function(const string& s) {  // func names
    //     return (

    //         // prevents vars and names with same starts but not funcs
    //         (s.find("SMA") != string::npos) ||
    //         (s.find("crossunder") != string::npos) ||
    //         (s.find("crossover") != string::npos)
        
    //     ); 
    // } 
    
    bool is_coverage(const string& s) {  // pre-line vars
        return (
            s == "long" ||
            s == "short" || 
            s == "close_position" || 
            s == "open_position"
        );
    } 

public:  // callable outside

    LexicalTokenParser(const string& text) : input(text), position(0) {   // constructor
        init_keywords();  // create hashmap
    }

    // parse each token from strategy
    vector<Token> tokenize() {

        vector<Token> tokens;
        
        // loop through each char in strat
        while (position < input.length()) {

            char c = input[position];  // current character

            if (position > 0) {
                if (is_whitespace(c) && (!is_comma(input[position-1]))) {  // skip empty (except mid-func)
                    position++;
                    continue;
                }
            }

            if (c == '(') {  // opening delim
                if (position > 0) {  // pos-1
                    string function = get_next_function();  // has func name before bracket
                    Token token(TokenType::FUNCTION, function);  // function
                    tokens.emplace_back(token);
                    cout << "Function: " << function << endl;
                }
            }

            else if (c == '>' || c == '<') {  // comparison
                if (input[position+1] == '=') {  // case: or equal to
                    string temp;
                    temp += c;
                    temp += '=';
                    Token token(TokenType::COMPARISON_OPERATOR, temp);  // comparison
                    tokens.emplace_back(token);
                    cout << "Comparison: " << temp << endl;
                    position++;  // single --> skip space
                } else {  // case: normal
                    Token token(TokenType::COMPARISON_OPERATOR, string(1, c));  // comparison
                    tokens.emplace_back(token);
                    cout << "Comparison: " << c << endl;
                }
                position++;  // single --> skip space
            }

            else if (is_arithmitic_operator(c)) {  // math
                Token token(TokenType::ARITHMITIC_OPERATOR, string(1, c));  // arithmitic
                tokens.emplace_back(token);
                cout << "Arithmitic: " << c << endl;
                position++;  // single --> skip space
            }

            else if (is_assignment(c)) {  // equals
                Token token(TokenType::ASSIGNMENT, string(1, c));  // assignment
                tokens.emplace_back(token);
                cout << "Assignment: " << c << endl;
                position++;  // single --> skip space
            }

            else if (is_alpha(c)) {

                string word = get_next_word();  // complete word

                if (keywords.find(word) != keywords.end()) {
                    Token token(TokenType::KEYWORD, word);  // keywords
                    tokens.emplace_back(token);
                    cout << "Keyword: " << word << endl;
                } else if (is_variable(word)) {
                    Token token(TokenType::VARIABLE, word);  // variable
                    tokens.emplace_back(token);
                    cout << "Variable: " << word << endl;
                } else if (is_coverage(word)) {
                    Token token(TokenType::COVERAGE, word);  // coverage
                    tokens.emplace_back(token);
                    cout << "Coverage: " << word << endl;
                } else if (is_logic_operator(word)) {
                    Token token(TokenType::LOGIC_OPERATOR, word);  // logic
                    tokens.emplace_back(token);
                    cout << "Logic: " << word << endl;
                } else if (is_boolean_literal(word)) {
                    Token token(TokenType::BOOL_LITERAL, word);  // bool
                    tokens.emplace_back(token);
                    cout << "Boolean: " << word << endl;
                } else {
                    Token token(TokenType::NAME, word);  // bool
                    tokens.emplace_back(token);
                    cout << "Name: " << word << endl;
                }
            }

            // TODO: fix to not only catch above if statement faults
            // fallback for invalid inputs
            else {
                Token token(TokenType::UNKNOWN, string(1, c));   // char -> string copy (unknown)
                tokens.emplace_back(token);
                // cout << "Unknown: " << c << endl;
                position++;
                continue;
            }

        }

        return tokens;
    
    }

};

int main() {

    string text;
    ifstream strategy("./improved_temp_strat.txt");  // stream text
    string full_text{};
    
    while (getline(strategy, text)) {
        full_text += text + "\n";  // one large string
    }

    cout << "Parsed Strategy:\n" << endl;
    cout << full_text << endl;
    cout << "============================\n" << endl;

    LexicalTokenParser token_parser(full_text);  // parse

    vector<Token> tokens = token_parser.tokenize();  // tokenize

    return 0;

}