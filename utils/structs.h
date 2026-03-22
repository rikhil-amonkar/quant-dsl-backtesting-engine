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
        string o, string l, string r
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
struct EntryRules {
    string direction;  // long/short
    vector<Conditions> conditions; 
    string logic_operator;  // AND,OR

    EntryRules(  // entry rules constructor
        string d, vector<Conditions> c, string l
    ) : direction(d), conditions(c), logic_operator(l) {};
};

// when to exit backtest
struct ExitRules {
    string action;
    vector<Conditions> conditions;

    ExitRules(  // exit rules constructor
        string a, vector<Conditions> c
    ) : action(a), conditions(c) {};
};

// profit/loss marginal sizes
struct PositionSettings {
    float size_percentage;
    int max_positions;
    bool allow_short;  // yes/no

    PositionSettings(  // position settings constructor
        float s, int m, bool a
    ) : size_percentage(s), max_positions(m), allow_short(a) {};
};

// parent struct strategy
struct Strategy {
    string name;  // strat
    vector<Indicators> indicators;  // SMA
    vector<EntryRules> entry_rules;  // multiple
    vector<ExitRules> exit_rules;  // multiple
    PositionSettings pos_settings;  // thresholds

    Strategy(  // strategy constructor
        string n, vector<Indicators> i, vector<EntryRules> en, vector<ExitRules> ex, PositionSettings p
    ) : name(n), indicators(i), entry_rules(en), exit_rules(ex), pos_settings(p) {};
};

// define token types
enum class TokenType {  // enum --> type saftey
    KEYWORD,  // INDICATOR, EXIT
    NAME,  // sma_crossover_with_volume
    ASSIGNMENT,  // =
    FUNCTION,  // SMA(close, 3)
    VARIABLE,  // short_sma, long_sma
    INTEGER_LITERAL,  // 1, 2, 3
    FLOAT_LITERAL,  // 0.0, 1.0
    BOOL_LITERAL,  // true, false
    DELIMETER,  // ()
    LOGIC_OPERATOR,  // AND, OR
    ARITHMITIC_OPERATOR,  // +, -, *, /
    COMPARISON_OPERATOR,  // <, >
    COVERAGE,  // long, close_position
    UNKNOWN  // invalid
};

// token definition
struct Token {
    TokenType type;
    string value;

    Token(  // token constructor
        TokenType t, const string& v
    ) : type(t), value(v) {};
};

#endif