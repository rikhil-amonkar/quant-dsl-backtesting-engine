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
        py::module_::import("matplotlib");
        py::module_::import("numpy");

        py::module_ price_py_script = py::module_::import("prices");  // py
        py::object fetch_market_data = price_py_script.attr("fetch_ohlcv_data_from_market_timeline");  // func

        py::module_ graph_py_script = py::module_::import("graph");  // py
        py::object graph_pnl_data = graph_py_script.attr("plot_pnl_values_to_graph");  // func

        // run and cast result to cpp data type
        py::object result = fetch_market_data(ticker, start, end);
        auto market_data = result.cast<map<int, vector<float>>>();

        // graph pnl --> no return
        vector<float> temp = {100.0, 200.0, 300.0, 350.0, 200.0, 450.0};  //! temp values

        cout << "Running!" << endl;

        graph_pnl_data(temp, market_data);

        cout << "Complete!" << endl;

        return market_data;

    } catch (py::error_already_set &e) {  // fallback
        cout << "Python Error: " << e.what() << endl;
        exit(1);
    }

}

# endif