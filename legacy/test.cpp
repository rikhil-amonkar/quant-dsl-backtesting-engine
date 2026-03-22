#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "tokenize.h"

using namespace std;

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