#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

// run command: g++ -std=c++20 ast.cpp -o ast.out && ./ast.out

// define token types
enum class TokenType {  // enum --> type saftey
    KEYWORD,  // INDICATOR, EXIT
    NAME,  // sma_crossover_with_volume
    ASSIGNMENT,  // =
    OPERATION,  // <=, SMA, crossunder
    VARIABLE,  // short_sma, long_sma
    INTEGER_LITERAL,  // 1, 2, 3
    FLOAT_LITERAL,  // 0.0, 1.0
    BOOL_LITERAL,  // true, false
    LOGIC_OPERATOR,  // AND, OR
    COVERAGE  // long, close_position
};

struct Token {
    TokenType type;
    string value;

    // Token(TokenType t, const string& v) {
    //     type = t;
    //     value = v;
    // }
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

vector<string> split_text_to_vector(const string& text) {
    vector<string> tokens;
    string t;
    string delim = " ";
    stringstream stream(text);
    while (stream >> t) {
        tokens.push_back(t);
    }
    return tokens;
}

int main() {

    string text;
    ifstream strategy("./improved_temp_strat.txt");

    vector<Token> tokens;
    vector<string> words;

    while (getline(strategy, text)) {
        if (!(text.empty())) {

            words = split_text_to_vector(text);
            for (auto w : words) {
                // cout << text << endl;
                // Token token;
                // token.type = TokenType::KEYWORD;
                // token.value = text;
                if (check_if_keyword(w)) {
                    Token token(TokenType::KEYWORD, w);
                    tokens.push_back(token);
                }
            }

        }
    }

    for (auto t : tokens) {
        cout << "Parsed: " << t.value << endl;
    }

    return 0;
}