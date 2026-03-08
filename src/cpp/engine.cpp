#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
using namespace std;

// run command: g++ -std=c++20 engine.cpp -o engine.out && ./engine.out

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

// parse rule from read line
string parse_rules(string text_line) {

    // npos = not found, check if keyword exists
    if (text_line.find("STRATEGY") != string::npos) {
        return "STRATEGY";
    } else if (text_line.find("INDICATOR") != string::npos) {
        return "INDICATOR";
    } else if (text_line.find("ENTRY") != string::npos) {
        return "ENTRY";
    } else if (text_line.find("EXIT") != string::npos) {
        return "EXIT";
    } else if (text_line.find("POSITION") != string::npos) {
        return "POSITION";
    } else {
        return "INVALID COMMAND";  // fallback
    }

}

// read lines from strategy file
void read_strat(string filename) {

    string strat_text;

    // pull temp backtesting strategy file
    ifstream strat_file("./temp_strat.txt");  // read, name, file

    while (getline (strat_file, strat_text)) {
        if (!(strat_text.empty())) {  // skip empty lines
            cout << parse_rules(strat_text) << endl;
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