#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>

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

//! IMPROVE: temp data --> replace with webscraped later
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

// 

// compute operand values from condition parts
float evaluate_condition_vals(
    int curr_day, 
    string operand, 
    map<string, float> current_values, 
    map<int, vector<int>> market_data,
    array<string, 5> bar_fields
) {

    auto search_it = find(start(bar_fields), end(bar_fields), operand);  // search for operand
    
    if (search_it != end(bar_fields)) {  // case: in bar fields
        int source_index = get_data_index(operand);  // get op index from data
        return market_data[curr_day][source_index];
    } else {  // case: in curr vals map
        return current_values[operand];
    }

}

// compute indicator values from strategy
float compute_indicator(
    map<int, vector<int>> market_data, 
    int curr_day, string type, 
    string source, int period
) {

    float value{};
    int source_index = get_data_index(string source);  // get data index

    // define time interval for compute
    int start_day = curr_day - period + 1;
    int end_day = curr_day;

    // base on indicator type
    if (type == "SMA") {
        float total_sum{};  // 0.0f
        for (int i = start_day; i <= end_day; i++) {
            total_sum += market_data[i][source_index];
        }
        value = total_sum / period;  // short sma 
    }

    return value;

}

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

//! IMPROVE: hardcoded for now need to use AST later 
// parse rule from read line
string parse_rules(string text_line, Strategy& strategy) {  // ref to modify -> not copy

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
        strategy.entry_rule = entry_rule;
        return "ENTRY " + entry_rule.direction;
    } else if (parts[0] == "EXIT") {
        ExitRules exit_rules;  // init
        Conditions entry_conditions;  // init
        exit_rules.action = parts[1];
        entry_conditions.operation = parts[3];
        entry_conditions.left_operand = parts[2];
        entry_conditions.right_operand = parts[4];
        exit_rules.conditions.push_back(entry_conditions);  // append
        strategy.exit_rules.push_back(exit_rules);
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
        strategy.pos_settings = pos_settings;
        return "POSITION " + parts[2];
    } else {
        return "INVALID COMMAND";  // fallback
    }

}

// read lines from strategy file
void read_strat(string filename, Strategy& strategy) {  // ref to struct

    string strat_text;

    // pull temp backtesting strategy file
    ifstream strat_file("./temp_strat.txt");  // read, name, file

    while (getline(strat_file, strat_text)) {
        if (!(strat_text.empty())) {  // skip empty lines
            cout << parse_rules(strat_text, strategy) << endl;
        }
    }

    cout << "\nCompiled Strategy..." << endl;

    // cout << "\nStrategy: " + strategy.name << endl;

    // cout << "Indicators:" << endl;
    // for (auto indicator : strategy.indicators) {
    //     cout << "   - " + indicator.name + " " + indicator.type + " " + indicator.source + " " + to_string(indicator.period) << endl;
    // }

    // cout << "Entry Rule:" << endl;
    // cout << "   - " + strategy.entry_rule.direction + " " + strategy.entry_rule.logic_operator << endl;
    // cout << "   - Conditions:" << endl;
    // for (auto condition : strategy.entry_rule.conditions) {
    //     cout << "       - " + condition.left_operand + " " + condition.operation + " " + condition.right_operand << endl;
    // }

    // cout << "Exit Rules:" << endl;
    // for (auto rule : strategy.exit_rules) {
    //     cout << "   - " + rule.action << endl;
    //     cout << "   - Conditions:" << endl;
    //     for (auto condition : rule.conditions) {
    //         cout << "       - " + condition.left_operand + " " + condition.operation + " " + condition.right_operand << endl;
    //     }
    // }

    // cout << "Position Settings:" << endl;
    // cout << "   - " + to_string(strategy.pos_settings.size_percentage) << endl;
    // cout << "   - " + to_string(strategy.pos_settings.max_positions) << endl;
    // cout << "   - " + to_string(strategy.pos_settings.allow_short) << endl;

    strat_file.close();  // for memory space

}

int main() {

    cout << "Backtesting Engine!\n" << endl;

    Strategy strategy;  // init strategy

    string filename = "./temp_strat.txt";
    read_strat(filename, strategy);

    // store updates indicator values
    map<string, float> current_values;
    float value{};
    int min_start_day{};  // set to 0

    // immutable array to define possible bar fields
    const array<string, 5> bar_fields = {"open", "high", "low", "close", "volume"};

    // get minimum start day needed for compute
    for (auto indicator : strategy.indicators) {
        if (min_start_day < indicator.period) {
            min_start_day = indicator.period;  // update
        }
    }

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
            cout << "Skipping day value. Need more info for compute." << endl;
            continue;
        }

        // compute indicator values per indicator type
        for (auto indicator : strategy.indicators) {
            value = compute_indicator(
                market_data, curr_day, indicator.type, indicator.source, indicator.period
            );
            current_values[indicators.name] = value;  // store
            cout << "Indicator " + indicator.name + ": " + to_string(value) << endl;
        }

        // check if entry rule is valid
        for (auto condition : strategy.entry_rule.conditions) {
            string left_operand = condition.left_operand;
            string right_operand = condition.right_operand;
            string operation = condition.operation;  // forms --> (left operator right)
            float left_val = evaluate_condition_vals(  // left op val
                curr_day, left_operand, current_values, market_data, bar_fields
            );
            float right_val = evaluate_condition_vals(  // right op val
                curr_day, right_operand, current_values, market_data, bar_fields
            );
            bool condition_res = perform_condition_operation()  // TODO: finish this op exec func
        }
    }

    return 0;
}