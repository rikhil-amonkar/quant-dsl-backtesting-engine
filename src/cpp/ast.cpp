#include <iostream>
#include <string>
#include <vector>
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
    FUNCTION,  // SMA, crossunder
    VARIABLE,  // short_sma, long_sma
    INTEGER_LITERAL,  // 1, 2, 3
    FLOAT_LITERAL,  // 0.0, 1.0
    BOOL_LITERAL,  // true, false
    DELIMETER,  // ()
    LOGIC_OPERATOR,  // AND, OR
    ARITHMITIC_OPERATOR,  // +, -, *, /
    COMPARISON_OPERATOR,  // <, >
    COVERAGE  // long, close_position
};

struct Token {
    TokenType type;
    string value;
    Token(TokenType t, const string& v) : type(t), value(v) {}
};

bool check_if_keyword(const string& text) {
    if (
        text == "STRATEGY" ||
        text == "INDICATOR" ||
        text == "ENTRY" ||
        text == "EXIT" ||
        text == "POSITION"
    ) {
        return true;
    }
    return false;
} 

bool check_if_variable(const string& text) {
    if (
        text == "short_sma" ||
        text == "long_sma" ||
        text == "vol_avg" ||
        text == "size_percent" ||
        text == "max_positions" ||
        text == "allow_short"
    ) {
        return true;
    }
    return false;
} 

bool check_if_function(const string& text) {
    if (
        (text.find("SMA") != string::npos) ||
        (text.find("crossunder") != string::npos) ||
        (text.find("crossover") != string::npos)
    ) {
        return true;
    }
    return false;
} 

bool check_if_coverage(const string& text) {
    if (
        text == "long" ||
        text == "short" ||
        text == "close_position" ||
        text == "open_position"
    ) {
        return true;
    }
    return false;
} 

vector<string> split_text_to_vector(const string& text, const char& delim) {
    vector<string> words;
    stringstream stream(text);
    string token;
    while (getline(stream, token, delim)) {
        words.push_back(token);
    }
    return words;
}

vector<tuple<Token, string>> lexical_token_parser(const vector<string>& parts, vector<tuple<Token, string>>& tokens) {
    for (auto p : parts) {
        if (check_if_keyword(p)) {
            Token token(TokenType::KEYWORD, p);  // keyword
            tokens.push_back(make_tuple(token, "KEYWORD"));
        } else if (check_if_variable(p)) {
            Token token(TokenType::VARIABLE, p);  // variable
            tokens.push_back(make_tuple(token, "VARIABLE"));
        }  else if (p == "=") {
            Token token(TokenType::ASSIGNMENT, p);  // assignment
            tokens.push_back(make_tuple(token, "ASSIGNMENT"));
        } else if (p == ">" || p == "<" || p == ">=" || p == "<=") {
            Token token(TokenType::COMPARISON_OPERATOR, p);  // comparison
            tokens.push_back(make_tuple(token, "COMPARISON_OPERATOR"));
        } else if (check_if_function(p)) {
            vector<string> operation = split_text_to_vector(p, '(');
            Token token(TokenType::FUNCTION, operation[0]);  // function
            tokens.push_back(make_tuple(token, "FUNCTION"));
        } else if (check_if_coverage(p)) {
            Token token(TokenType::COVERAGE, p);  // coverage
            tokens.push_back(make_tuple(token, "COVERAGE"));
        } else if (p == "AND" || p == "OR") {
            Token token(TokenType::LOGIC_OPERATOR, p);  // logic
            tokens.push_back(make_tuple(token, "LOGIC_OPERATOR"));
        } else if (p == "true" || p == "false") {
            Token token(TokenType::BOOL_LITERAL, p);  // boolean
            tokens.push_back(make_tuple(token, "BOOL_LITERAL"));
        } else if ((p >= "0" && p <= "9") && (p.find(".") == string::npos)) {
            Token token(TokenType::INTEGER_LITERAL, p);  // integer
            tokens.push_back(make_tuple(token, "INTEGER_LITERAL"));
        } else if ((p >= "0" && p <= "9") && (p.find(".") != string::npos)) {
            Token token(TokenType::FLOAT_LITERAL, p);  // float
            tokens.push_back(make_tuple(token, "FLOAT_LITERAL"));
        } else if (p == "+" || p == "-" || p == "*" || p == "/") {
            Token token(TokenType::ARITHMITIC_OPERATOR, p);  // arithmitic
            tokens.push_back(make_tuple(token, "ARITHMITIC_OPERATOR"));
        } else {
            Token token(TokenType::NAME, p);  // strategy name
            tokens.push_back(make_tuple(token, "NAME"));
        } 
    }
    return tokens;
}

int main() {

    string text;
    ifstream strategy("./improved_temp_strat.txt");
    vector<tuple<Token, string>> tokens;
    vector<string> parts;
    while (getline(strategy, text)) {
        if (!(text.empty())) {
            parts = split_text_to_vector(text, ' ');
            tokens = lexical_token_parser(parts, tokens);
        }
    }

    for (auto t : tokens) {
        cout << "Type: " << get<1>(t) << ", Value: " << get<0>(t).value << endl;
    }

    return 0;
}