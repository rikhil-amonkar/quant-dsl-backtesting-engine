#include <iostream>
#include <string>
#include <vector>
#include <map>
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

int main() {

    cout << "Backtesting Engine!" << endl;

    // print each data point from hashmap
    for (auto market_val : market_data) {  // auto data type
        cout << "Day: " << market_val.first << 
        " | Open: " << market_val.second[0] << 
        " | High: " << market_val.second[1] << 
        " | Low: " << market_val.second[2] << 
        " | Close: " << market_val.second[3] << 
        " | Volume: " << market_val.second[4] << 
        endl; 
    }

    return 0;
}