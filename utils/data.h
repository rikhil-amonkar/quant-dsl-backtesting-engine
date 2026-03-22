#ifndef MARKET_DATA_H
#define MARKET_DATA_H

#include <map>
#include <vector>
#include <array>

using namespace std;

//! IMPROVE: temp data --> replace with webscraped data later
// temp market data : day, [open, high, low, close, volume]
// data narrative: dull start -> mid-sample run-up -> volume spike -> fade lower again
const map<int, vector<int>> market_data = {  // ordered hashmap (immutable)
    {1, {100, 102, 99, 101, 900}},
    {2, {101, 103, 100, 101, 880}},
    {3, {101, 102, 99, 100, 850}},
    {4, {100, 102, 99, 100, 900}},
    {5, {100, 101, 98, 99, 920}},
    {6, {99, 101, 98, 100, 950}},
    {7, {100, 105, 99, 104, 1200}},
    {8, {104, 109, 103, 108, 1500}},
    {9, {108, 114, 107, 113, 1800}},
    {10, {113, 120, 112, 119, 2100}},
    {11, {119, 128, 118, 127, 2500}},
    {12, {127, 136, 125, 134, 2800}},
    {13, {134, 142, 132, 140, 3000}},
    {14, {140, 148, 138, 145, 3400}},
    {15, {145, 146, 132, 135, 2600}},
    {16, {135, 136, 126, 128, 2200}},
    {17, {128, 130, 120, 122, 2000}},
    {18, {122, 124, 115, 117, 1800}},
    {19, {117, 118, 110, 112, 1700}},
    {20, {112, 113, 104, 106, 1600}},
};

// immutable array to define possible bar fields
const array<string, 5> bar_fields = {"open", "high", "low", "close", "volume"};

#endif