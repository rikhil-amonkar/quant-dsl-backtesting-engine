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

    Conditions(  // conditions constructor
        string operation, string left_operand, string right_operand
    ) : operation(o), left_operand(l), right_operand(r) {};
};

// vars used in rules (always updating)
struct Indicators {
    string name;
    string type;
    string source;
    int period;

    Indicators(  // indicator constructor
        string n, string t, string s, int p
    ) : name(n), type(t), source(s), period(p) {};
};

// rule to enter backtest
struct EntryRule {
    string direction;  // long/short
    vector<Conditions> conditions; 
    string logic_operator;  // AND,OR

    EntryRule(  // entry rules constructor
        string direction, vector<Conditions> conditions, string logic_operator
    ) : direction(d), conditions(c), logic_operator(l)
};

// when to exit backtest
struct ExitRules {
    string action;
    vector<Conditions> conditions;

    ExitRules(  // exit rules constructor
        string action, vector<Conditions> conditions
    ) : action(a), conditions(c) {};
};

// profit/loss marginal sizes
struct PositionSettings {
    float size_percentage;
    int max_positions;
    bool allow_short;  // yes/no

    PositionSettings(  // position settings constructor
        float size_percentage, int max_positions, bool allow_short
    ) : size_percentage(s), max_positions(m), allow_short(a) {};
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