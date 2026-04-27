# Quant-Based DSL Backtesting Engine

# Summary of Project

This project is a quant-based backtesting engine built in C++. I made this project because I have always been interested in investing and the stock market but my knowledge was in the realm of trading. I never knew how to know when to invest and buy/sell trades, until I heard of backtesting. Backtesting allows a certain trading strategy to be applied to past data where trades are simulated against previous markets which, after a ton of accumulation in PnL results, can help a person determine how well that strategy may perform in today's market. I wanted to also use this project as a way to continue building my C++ skills and become a better overall developer. I learned a ton about proper use of structs and enumurators during lexical tokenization of custom languages such as my strategy file syntax. I practiced pillars of programming such as encapsulation which allowed me to be more efficient with my variables and function arguments when many blocks of code all related to similar purpose. This also introduced me to private and public methods and how to properly use them in classes, along with constructos to better initialize object instances such as the child-structs of my main Strategy object. This project took a lot of time and the main backtesting engine was a big part of that time contribution. I will continue to improve this project to better parse input strategy files to allow for more abstract syntax and error checks. I will make the lexer more robust to adapt to different conditions on entry and exit rules. I will also further clean up the code and add better documentation for other developers to check out and even non-technical people to read through and understand how backtesting and software development can combine into a great use-case.

# Programming Languages
- C++20
- Python 3.12

# Dependencies

- **yfinance** : fetch market data in Python
- **pybind11** : call Python from C++ via an embedded interpreter (market download + plotting)
- **matplotlib** : plot OHLC prices and stepped portfolio capital after the backtest finishes

# Environment Setup

Set up the virtual environment for Python dependencies:

```sh
python3.12 -m venv venv
pip install -r requirements.txt
```

Configure the [CMakeLists.txt](./CMakeLists.txt) initially and after any changes to this file:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
```

Build the CMake build system for compile automation and external C++ libraries when source files change:

```sh
cmake --build build
```

# Run Engine

To run the engine, initialize the CMake build system and run the main binary from the project root. You can change the ticker and date range in [main.cpp](./main.cpp) (defaults are `NVDA` with `2024-01-01` through `2025-01-01`) and edit the strategy in [backtesting_strategy.strat](./backtesting_strategy.strat). The embedded Python layer expects the `venv` from the setup steps so `yfinance` and `matplotlib` resolve correctly.

```sh
./build/main
```

During the run, the engine also writes a day-by-day trace to `log.txt` in the working directory.

When the simulation completes, the program saves a chart to **`simulated_backtesting_pnl_graph.png`**: left axis is daily open/high/low/close, right axis is portfolio value (stepped), with dotted reference lines for initial and final capital and shaded bands between capital changes. A plot window may also open if your environment supports `matplotlib`’s interactive backend.

**Example chart** (NVDA-style run; OHLC on the left axis, stepped capital and reference lines on the right):

![Portfolio value over simulated backtest period — OHLC prices and capital](./images/simulated_backtesting_pnl_graph.png)

# Engine Architecture

The backtesting engine I developed in C++ serves to take in custom domain-specific programming language (DSL) with syntax relating to grammer used for creating strategies to test again historic market data. The engine first opens the file content from the custom strategy and parses the text into a single string. That string is then sent to a lexical parser class which converts the vocabulary types into token pairs of enumeurator constants and string values from the strategy itself. These tokenized values are stored into a vector and sent to a custom rule function which serves to assign the core structs of the engine with their corresponding attributes and members. When iterating through the lexicalized vector of tokens, the rule parser validates and associates the token types and string values with their corresponding struct attributes such as "Indicators" or "Entry Rule" conditions. Once all the structs have been populated with the strategy values, the engine begins to iterate through the market data and apply the values from the structs to mathematical computations and state checks. The engine navigates through the hashmap of data day-by-day, and then procceeds to update mutable values such as "Indicator" variables (short_sma, long_sma, ect.), as well as price values like price entered and price exited at during a trade. Th state is another important factor to the trading engine as it determines if the exit conditions needs to be checked and if a realized PnL needs to be updated or if the unrealized can continue to increment if already inside of a trade. Once the entry conditions are all hit, the trade is set to "in entry" and the following iterations will continiously check the exit conditions until any flag indicating to exit has been hit. After the simulation of days has been completed, the program prints aggregate results (total entries, final realized PnL, updated capital, and percent change) and invokes Python to render the PnL chart described above.

# Backtesting Engine Example

The section below walks from the on-disk strategy file through representative console output. Numeric values are illustrative; live runs use real Yahoo Finance history for your chosen ticker and dates. The lexer produces token types such as `KEYWORD`, `FUNCTION`, `COVERAGE`, and `INTEGER_LITERAL` internally; the program does not dump the full token list to the terminal by default.

**Custom DSL Input Strategy File** (same shape as the default [backtesting_strategy.strat](./backtesting_strategy.strat))

```
STRATEGY SMA_Crossover_With_Volume

INDICATOR short_sma = SMA(close, 3)
INDICATOR long_sma = SMA(close, 5)
INDICATOR vol_avg = SMA(volume, 3)

ENTRY long crossover(short_sma, long_sma)
ENTRY long volume > vol_avg

EXIT close_position crossunder(short_sma, long_sma)
EXIT close_position close <= entry_price * 0.97

POSITION size_percent = 100
POSITION max_positions = 1
POSITION allow_short = false
```

↓

**Strategy Values Post-Tokenization via Lexer** (representative token categories)

```
Type: KEYWORD, Value: STRATEGY
Type: NAME, Value: SMA_Crossover_With_Volume
...
Type: FUNCTION, Value: SMA(close, 3)
...
Type: KEYWORD, Value: ENTRY
Type: COVERAGE, Value: long
Type: FUNCTION, Value: crossover(short_sma, long_sma)
...
Type: INTEGER_LITERAL, Value: 100
```

↓

**Console output while backtesting** (structure matches [utils/engine.h](./utils/engine.h); `main.cpp` echoes the parsed strategy first, then the engine prints per-day diagnostics)

```
Backtesting Engine!

============================

PARSED STRATEGY:

STRATEGY SMA_Crossover_With_Volume
...

============================

Strategy struct's members have been populate with file attributes!

Initial Capital: $100000

Day: 1 | Open: ... | High: ... | Low: ... | Close: ... | Volume: ...
Current-Day PnL: 0
----------------------------
No signal yet. Need more information. Skipping current day.

...

Day: 5 | Open: ... | High: ... | Low: ... | Close: ... | Volume: ...
Current-Day PnL: 0
----------------------------
Indicator short_sma: ...
Indicator long_sma: ...
Indicator vol_avg: ...
----------------------------
-- Entry Conditions --
Rule: crossover(48.2, 47.9) | 0
Rule: 1.234e+08 > 9.876e+07 | 1
----------------------------

...

Day: 12 | Open: ... | High: ... | Low: ... | Close: ... | Volume: ...
Current-Day PnL: 0
----------------------------
Indicator short_sma: ...
Indicator long_sma: ...
Indicator vol_avg: ...
----------------------------
-- Entry Conditions --
Rule: crossover(51.1, 50.4) | 1
Rule: 1.5e+08 > 1.1e+08 | 1
----------------------------
All entry rules have passed for trade.
----------------------------

...

Entered trade!
----------------------------

...

Currently in trade...
----------------------------
-- Exit Conditions --
Rule: crossunder(49.5, 50.1) | 0
Rule: 48.2 <= 47.254 | 0
----------------------------
Entry Price: 50
Exit Price: 49.25
Shares: 2000
Current Unrealized PnL: $-1500

...

An exit rule has been hit while in trade.

...

Exited trade!
----------------------------
Entry Price: 50
Exit Price: 49
Shares: 2000
Current Unrealized PnL: $-2000

============================

Total Entries: 2
Final Realized PnL: $-3500
Updated Capital: $96500
Percent Difference in Capital: (-3.5%)
```

After the summary line, matplotlib writes **`simulated_backtesting_pnl_graph.png`** in the project directory (and may display the figure interactively).

# Developer

**Rikhil Amonkar** 
- CS @ *Drexel University*
- Software Dev. @ *Exo Dynamics*
- Prev. ML Engineer @ *Lockheed Martin*

**Contact Info**
- LinkedIn: *https://www.linkedin.com/in/rikhil-amonkar/*
- Email: *rikhilma@gmail.com*