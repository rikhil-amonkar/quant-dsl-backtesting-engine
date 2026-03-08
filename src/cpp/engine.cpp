#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>

using namespace std;

// run command: g++ -std=c++20 engine.cpp -o engine.out && ./engine.out

struct Conditions {
    string operation;
    string left_operand;
    string right_operand;
};

struct Indicators {
    string name;
    string type;
    string source;
    int period;
};

struct EntryRule {
    string direction;  // long/short
    vector<Conditions> conditions; 
    string logic_operator;  // AND,OR
};

struct ExitRules {
    string action;
    vector<Conditions> conditions;
};

struct PositionSettings {
    float size_percentage;
    int max_positions;
    bool allow_short;  // yes/no
};

struct Strategy {
    string name;  // strat
    vector<Indicators> indicators;  // SMA
    EntryRule entry_rule;  // single rule
    vector<ExitRules> exit_rules;
    PositionSettings pos_settings;  // thresholds
};

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
};

// break line into vector parts
vector<string> str_to_vec(string text_line) {
    vector<string> split_parts;
    string s;  // curr token
    string delim = " ";
    stringstream stream(text_line);  // input stream
    
    while (stream >> s) {  // bitwise char read
        split_parts.push_back(s);  // append
    }

    return split_parts;
}

// parse rule from read line
string parse_rules(string text_line, Strategy strategy) {

    vector<string> parts = str_to_vec(text_line);  // break apart str

    // npos = not found, check if keyword exists
    if (parts[0] == "STRATEGY") {
        strategy.name = parts[1];  // strat name
        return "STRATEGY " + strategy.name;
    } else if (parts[0] == "INDICATOR") {
        Indicators indicator;  // init
        indicator.name = parts[1];
        indicator.type = parts[2];
        indicator.source = parts[3];
        indicator.period = stoi(parts[4]);  // str to int
        strategy.indicators.push_back(indicator);  // append
        return "INDICATOR " + indicator.name;
    } else if (parts[0] == "ENTRY") {
        EntryRule entry_rule;  // init
        Conditions entry_conditions;  // init
        entry_rule.direction = parts[1];
        entry_rule.logic_operator = parts[5];
        entry_conditions.operation = parts[2];
        entry_conditions.left_operand = parts[3];
        entry_conditions.right_operand = parts[4];
        entry_rule.conditions.push_back(entry_conditions);  // append cond1
        entry_conditions.operation = parts[7];
        entry_conditions.left_operand = parts[6];
        entry_conditions.right_operand = parts[8];
        entry_rule.conditions.push_back(entry_conditions);  // append cond2
        return "ENTRY " + entry_rule.direction;
    } else if (parts[0] == "EXIT") {
        ExitRules exit_rules;  // init
        Conditions entry_conditions;  // init
        exit_rules.action = parts[1];
        entry_conditions.operation = parts[2];
        entry_conditions.left_operand = parts[3];
        entry_conditions.right_operand = parts[4];
        exit_rules.conditions.push_back(entry_conditions);  // append
        return "EXIT " + exit_rules.action;
    } else if (parts[0] == "POSITION") {
        PositionSettings pos_settings;  // init
        pos_settings.size_percentage = stoi(parts[2]);  // str to int
        pos_settings.max_positions = stoi(parts[4]);  // str to int
        if (parts[6] == "true") {
            pos_settings.allow_short = true;
        } else {
            pos_settings.allow_short = false;
        }
        return "POSITION " + parts[2];
    } else {
        return "INVALID COMMAND";  // fallback
    }

}

// read lines from strategy file
void read_strat(string filename) {

    string strat_text;

    Strategy strategy;

    // pull temp backtesting strategy file
    ifstream strat_file("./temp_strat.txt");  // read, name, file

    while (getline(strat_file, strat_text)) {
        if (!(strat_text.empty())) {  // skip empty lines
            cout << parse_rules(strat_text, strategy) << endl;
        }
    }

    strat_file.close();  // for memory space

}

int main() {

    cout << "Backtesting Engine!" << endl;

    string filename = "./temp_strat.txt";
    read_strat(filename);

    // // print each data point from hashmap
    // for (auto market_val : market_data) {  // auto data type
    //     cout << "Day: " << market_val.first << 
    //     " | Open: " << market_val.second[0] << 
    //     " | High: " << market_val.second[1] << 
    //     " | Low: " << market_val.second[2] << 
    //     " | Close: " << market_val.second[3] << 
    //     " | Volume: " << market_val.second[4] << 
    //     endl; 
    // }

    return 0;
}