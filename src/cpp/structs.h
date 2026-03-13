#ifndef STRATEGY_STRUCTS_H
#define STRATEGY_STRUCTS_H

#include <string>
#include <vector>

using namespace std;

// rules for entry/exit
struct Conditions {
    string operation;
    string left_operand;
    string right_operand;
};

// vars used in rules (always updating)
struct Indicators {
    string name;
    string type;
    string source;
    int period;
};

// rule to enter backtest
struct EntryRule {
    string direction;  // long/short
    vector<Conditions> conditions; 
    string logic_operator;  // AND,OR
};

// when to exit backtest
struct ExitRules {
    string action;
    vector<Conditions> conditions;
};

// profit/loss marginal sizes
struct PositionSettings {
    float size_percentage;
    int max_positions;
    bool allow_short;  // yes/no
};

// parent struct strategy
struct Strategy {
    string name;  // strat
    vector<Indicators> indicators;  // SMA
    EntryRule entry_rule;  // single rule
    vector<ExitRules> exit_rules;
    PositionSettings pos_settings;  // thresholds
};

#endif