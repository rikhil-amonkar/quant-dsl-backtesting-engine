#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <map>
#include <fstream>
#include <chrono>
#include <algorithm>

#include "../utils/binder.h"
#include "../utils/structs.h"
#include "../utils/lexer.h"
#include "../utils/parser.h"
#include "../utils/engine.h"

using namespace std;
using namespace std::chrono;

// possible bar fields (immutable)
const array<string, 5> bar_fields = {"open", "high", "low", "close", "volume"};

// strategy sent through lexer and parser
Strategy read_strategy_file(const string &filename)
{

    string strat_text;
    ifstream strat_file(filename);
    string full_text{};

    while (getline(strat_file, strat_text))
    {
        full_text += strat_text + "\n";
    }
    strat_file.close();

    LexicalTokenizer build_lexer(full_text);
    vector<Token> tokens = build_lexer.tokenize();

    TokenParser build_parser(tokens);
    return build_parser.parse_rules();
}

int main()
{

    cerr << "Backtesting Engine Benchmark\n"
         << endl;

    // vector of test strategy file paths
    vector<string> strategy_files = {
        "./backtesting_strategy.strat",
        "./benchmark/strategies/sma_fast_crossover.strat",
        "./benchmark/strategies/sma_slow_crossover.strat",
        "./benchmark/strategies/sma_mean_reversion_exit.strat"};

    // long-history tickers (for testing)
    vector<string> tickers = {
        "AAPL", "MSFT", "SPY", "QQQ", "JPM", "XOM"};

    // time-span (20 years)
    string start = "2005-01-01";
    string end = "2025-01-01";

    float capital = 100000.0f;

    cerr << "Fetching " << tickers.size() << " tickers from " << start << " to " << end << " ..." << endl;

    auto fetch_start = high_resolution_clock::now(); // begin timer

    map<string, map<int, vector<float>>> market_data_by_ticker;
    for (const auto &ticker : tickers)
    {
        market_data_by_ticker[ticker] = get_market_data_over_timeline(ticker, start, end);
    }

    auto fetch_end = high_resolution_clock::now(); // end timer
    double fetch_seconds = duration<double>(fetch_end - fetch_start).count();

    long total_days = 0;
    for (const auto &entry : market_data_by_ticker)
    {
        total_days += entry.second.size(); // sum days per year
    }
    double avg_days_per_ticker = static_cast<double>(total_days) / tickers.size();
    double years_spanned = avg_days_per_ticker / 252.0; // about 252 trading days per year

    cerr << "Data fetch complete in " << fetch_seconds << "s (network I/O, excluded from throughput below)\n"
         << endl;

    const int repetitions = 3; // benchmark repetition
    vector<double> run_seconds;
    long total_events_per_rep = 0;

    for (int rep = 0; rep < repetitions; ++rep)
    {

        auto compute_start = high_resolution_clock::now();
        long events_this_rep = 0;

        for (const auto &strat_file : strategy_files)
        {
            for (const auto &ticker : tickers)
            {

                Strategy strategy = read_strategy_file(strat_file);
                const auto &market_data = market_data_by_ticker.at(ticker);

                BacktestingEngine engine(market_data, bar_fields, capital, strategy);
                auto result = engine.run_data_through_engine_logic();

                events_this_rep += market_data.size();
            }
        }

        auto compute_end = high_resolution_clock::now();
        run_seconds.push_back(duration<double>(compute_end - compute_start).count());
        total_events_per_rep = events_this_rep;

        cerr << "Repetition " << (rep + 1) << "/" << repetitions
             << " completed in " << run_seconds.back() << "s" << endl;
    }

    double min_seconds = *min_element(run_seconds.begin(), run_seconds.end());
    double avg_seconds = 0.0;
    for (double s : run_seconds)
        avg_seconds += s;
    avg_seconds /= run_seconds.size();

    double events_per_sec_min = total_events_per_rep / min_seconds;
    double events_per_sec_avg = total_events_per_rep / avg_seconds;

    // display benchmark results
    cerr << "\n============================" << endl;
    cerr << "BENCHMARK RESULTS" << endl;
    cerr << "Strategies:        " << strategy_files.size() << endl;
    cerr << "Symbols:           " << tickers.size() << endl;
    cerr << "Years per symbol:  ~" << years_spanned << endl;
    cerr << "Bar-events / run:  " << total_events_per_rep << endl;
    cerr << "Repetitions:       " << repetitions << endl;
    cerr << "Min compute time:  " << min_seconds << "s" << endl;
    cerr << "Avg compute time:  " << avg_seconds << "s" << endl;
    cerr << "Events/sec (min):  " << events_per_sec_min << endl;
    cerr << "Events/sec (avg):  " << events_per_sec_avg << endl;
    cerr << "============================" << endl;

    return 0;
}
