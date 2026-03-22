# Quant-Based DSL Backtesting Engine

# Summary of Project

This project is a qaunt-based backtesting engine built in C++. I made this project because I have always been interested in investing and the stock market but my knowledge was in the realm of trading. I never knew how to know when to invest and buy/sell trades, until I heard of backtesting. Backtesting allows a certain trading strategy to be applied to past data where trades are simulated against previous markets which, after a ton of accumulation in PnL results, can help a person determine how well that strategy may perform in today's market. I wanted to also use this project as a way to continue building my C++ skills and become a better overall developer. I learned a ton about proper use of structs and enumurators during lexical tokenization of custom languages such as my strategy file syntax. I practiced pillars of programming such as encapsulation which allowed me to be more efficient with my variables and function arguments when many blocks of code all related to similar purpose. This also introduced me to private and public methods and how to properly use them in classes, along with constructos to better initialize object instances such as the child-structs of my main Strategy object. This project took a lot of time and the main backtesting engine was a big part of that time contribution. I will continue to improve this project to better parse input strategy files to allow for more abstract syntax and error checks. I will make the lexer more robust to adapt to different conditions on entry and exit rules. I will also further clean up the code and add better documentation for other developers to check out and even non-technical people to read through and understand how backtesting and software development can combine into a great use-case.

# Programming Languages
- C++20

# Engine Architecture

The backtesting engine I developed in C++ serves to take in custom domain-specific programming language (DSL) with syntax relating to grammer used for creating strategies to test again historic market data. The engine first opens the file content from the custom strategy and parses the text into a single string. That string is then sent to a lexical parser class which converts the vocabulary types into token pairs of enumeurator constants and string values from the strategy itself. These tokenized values are stored into a vector and sent to a custom rule function which serves to assign the core structs of the engine with their corresponding attributes and members. When iterating through the lexicalized vector of tokens, the rule parser validates and associates the token types and string values with their corresponding struct attributes such as "Indicators" or "Entry Rule" conditions. Once all the structs have been populated with the strategy values, the engine begins to iterate through the market data and apply the values from the structs to mathematical computations and state checks. The engine navigates through the hashmap of data day-by-day, and then procceeds to update mutable values such as "Indicator" variables (short_sma, long_sma, ect.), as well as price values like price entered and price exited at during a trade. Th state is another important factor to the trading engine as it determines if the exit conditions needs to be checked and if a realized PnL needs to be updated or if the unrealized can continue to increment if already inside of a trade. Once the entry conditions are all hit, the trade is set to "in entry" and the following iterations will continiously check the exit conditions until any flag indicating to exit has been hit. After the simulation of days has been completed, the final result displays the original capital after its update in value as well as a percentage change to show the loss or gain from the original portfolio value.

# Backtesting Engine Example

The section below is meant to serve as a visual example to better understand the engine through a terminal output example of how the strategy is tokenized and how it is fed through historic data to provide an output to the user of the intermediate values and the final profit/loss result. Note that this is just an example that I made and the numbers may be a bit inaccurate, but the actual engine works great. The exmple just had to be made concise to showcase the flow from strategy all the way to result.

**Custom DSL Input Strategy File**

```
STRATEGY fake_demo_strat

INDICATOR fast = SMA(close, 2)
INDICATOR slow = SMA(close, 4)

ENTRY long crossover(fast, slow)

EXIT close_position crossunder(fast, slow)

POSITION size_percent = 50
```

↓

**Strategy Values Post-Tokenization via Lexer**

```
Type: KEYWORD, Value: STRATEGY
Type: NAME, Value: fake_demo_strat
...
Type: FUNCTION, Value: SMA(close, 2)
...
Type: KEYWORD, Value: ENTRY
Type: COVERAGE, Value: long
Type: FUNCTION, Value: crossover(fast, slow)
...
Type: INTEGER_LITERAL, Value: 50
```

↓

**Output from Backtesting Through Previous Data**

```
Day: 1 | Open: 50 | High: 51 | Low: 49 | Close: 50 | Volume: 200
----------------------------
No signal yet. Need more information. Skipping current day.

...

Day: 4 | Open: 52 | High: 54 | Low: 51 | Close: 53 | Volume: 310
----------------------------
Indicator fast: 52.50
Indicator slow: 51.00
Day: 4 passed the entry rule...entering trade.

...

Day: 8 | Open: 53 | High: 53 | Low: 50 | Close: 51 | Volume: 280
----------------------------
Exit rule has been hit while in trade.
Entry Price: 52.00
Exit Price: 51.00
Shares: 961
Current PnL: -$961.00

...

Final Realized PnL: -$961.00
Remaining Capital: $99039.00
Percent Difference in Capital: (-0.96%)
```

# Developer

**Rikhil Amonkar** 
- CS @ *Drexel University*
- ML Engineer @ *Lockheed Martin*
- Software Dev. @ *Exo Dynamics*

**Contact Info**
- LinkedIn: *https://www.linkedin.com/in/rikhil-amonkar/*
- Email: *rikhilma@gmail.com*