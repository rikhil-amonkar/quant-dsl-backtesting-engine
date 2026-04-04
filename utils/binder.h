#ifndef BIND_PY_CPP
#define BIND_PY_CPP

#include <map>
#include <vector>
#include <pybind11/embed.h>
#include <pybind11/stl.h>

using namespace std;
namespace py = pybind11;

auto get_market_data_over_timeline(string ticker, string start, string end) {

    py::scoped_interpreter gaurd{};  // init

    try {

        // route to dependencies
        py::exec(R"(
            import sys
            sys.path.insert(0, "venv/lib/python3.12/site-packages")
        )");
        py::module_::import("yfinance");

        py::module_ price_py_script = py::module_::import("prices");  // py
        py::object fetch_market_data = price_py_script.attr("fetch_ohlcv_data_from_market_timeline");  // func

        // run and cast result to cpp data type
        py::object result = fetch_market_data(ticker, start, end);
        auto market_data = result.cast<map<int, vector<float>>>();

        return market_data;

    } catch (py::error_already_set &e) {  // fallback
        cout << "Python Error: " << e.what() << endl;
        exit(1);
    }

}

# endif