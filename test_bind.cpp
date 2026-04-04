#include <iostream>
#include <string>
#include <map>
#include "utils/binder.h"

using namespace std;
namespace py = pybind11;

int main() {

    string ticker = "AAPL";
    string start = "2023-01-01";
    string end = "2023-01-10";

    auto market_data = get_market_data_over_timeline(ticker, start, end);

    // for (auto const& [day, data] : market_data) {
    //     cout << "Day: " << day << endl;
    // }

    cout << "Day: " << market_data << endl;

}