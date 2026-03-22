#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "./utils/data.h"
#include "./utils/structs.h"
#include "./utils/lexer.h"
#include "./utils/parser.h"
#include "./utils/engine.h"

using namespace std;

// run command: g++ -std=c++20 main.cpp -o main.out && ./main.out

// read lines from strategy file
Strategy read_strategy_file(string filename) {  // ref to struct

    // pull temp backtesting strategy file
    string strat_text;
    ifstream strat_file(filename);  // read, name, file
    string full_text{};

    while (getline(strat_file, strat_text)) {
        full_text += strat_text + "\n";  // one large string
    }

    LexicalTokenizer build_lexer(full_text);  // text lexer
    vector<Token> tokens = build_lexer.tokenize();  // tokenize

    TokenParser build_parser(tokens);  // parse tokens
    Strategy strategy = build_parser.parse_rules();  // assign tokens to strategy struct

    cout << "\n============================\n" << endl;
    cout << "PARSED STRATEGY:\n" << endl;
    cout << full_text << endl;
    cout << "============================\n" << endl;

    strat_file.close();  // for memory space

    return strategy;  // populated structs

}

int main() {

    cout << "Backtesting Engine!\n" << endl;

    string filename = "./backtesting_strategy.strat";
    Strategy strategy = read_strategy_file(filename);  // built strategy struct

    cout << "Strategy struct's members have been populate with file attributes!\n" << endl;

    float capital = 100000.0f;  // initial portfolio capital
    BacktestingEngine engine(market_data, bar_fields, capital, strategy);  // main engine
    auto [final_pnl, updated_capital, capital_diff_percent] = engine.run_data_through_engine_logic();  // simulate pnl

    cout << "\n============================" << endl;
    cout << "\nFinal Realized PnL: $" << final_pnl << endl;  // realized --> after exit
    cout << "Updated Capital: $" << updated_capital << endl;

    if (updated_capital < capital) {  // case: loss
        cout << "Percent Difference in Capital: (-" << capital_diff_percent << "%)" << endl;
    } else if (updated_capital > capital) {  // case: gain 
        cout << "Percent Difference in Capital: (+" << capital_diff_percent << "%)" << endl;
    } else {  // case: constant
        cout << "Percent Difference in Capital: (" << capital_diff_percent << "%)" << endl;
    }

    return 0;

}