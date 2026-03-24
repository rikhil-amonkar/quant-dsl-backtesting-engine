#ifndef BACKTEST_ENGINE_H
#define BACKTEST_ENGINE_H

#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <optional>
#include <cmath>

#include "./structs.h"

using namespace std;

// backtesting engine for simulating strategy with data
class BacktestingEngine {

private:  // local

    map<int, vector<int>> market_data;
    array<string, 5> bar_fields;
    float capital;
    Strategy strategy;

    // get market data index from hashmap layout
    int get_data_index(string source) {

        int source_index{};

        // map source index based on market data position
        if (source == "open") {
            source_index = 0;
        } else if (source == "close") {
            source_index = 3;
        } else if (source == "high") {
            source_index = 2;
        } else if (source == "low") {
            source_index = 1;
        } else {
            source_index = 4;  // volume
        }

        return source_index;

    }

    // calculate result value from arithmitic string expression
    float solve_string_arithmitic(string arithmitic_operation, float value) {

        string text;
        stringstream stream(arithmitic_operation);
        vector<string> operands;

        while (stream >> text) {
            operands.push_back(text);
        }

        if (operands.size() != 3) {
            cout << "Not a simple arithmitic. Invalid." << endl;
            return 0.0f;
        };

        // solve based on operation type
        if (operands[1] == "+") {
            return (value + stof(operands[2]));
        } else if (operands[1] == "-") {
            return (value - stof(operands[2]));
        } else if (operands[1] == "*") {
            return (value * stof(operands[2]));
        } else if (operands[1] == "/") {
            return (value / stof(operands[2]));
        } else {
            cout << "Invalid arithmitic operator." << endl;
            return 0.0f;
        }

    }

    // compute operand values from condition parts
    float evaluate_condition_vals(int curr_day, string operand, map<string, float> current_values, float entry_price) {

        auto search_it = find(begin(bar_fields), end(bar_fields), operand);  // search for operand
        
        // TODO: be able to handle equations too
        if (operand.find("entry_price") != string::npos) {  // case: sub-equation
            return solve_string_arithmitic(operand, entry_price);  //! temp solution (handles arithmitic)
        } else if (search_it != end(bar_fields)) {  // case: in bar fields
            int source_index = get_data_index(operand);  // get op index from data
            return market_data[curr_day][source_index];
        } else {  // case: in curr vals map
            return current_values[operand];
        }

    }

    // check if condition expression is true/false (crossover/crossunder)
    bool cross_condition_operation(
        float left_op_val, float right_op_val, string operation, int curr_day, string left_operand, 
        string right_operand, map<string, float> previous_values, float entry_price
    ) {

        int prev_day = curr_day - 1;  // previous day

        // using previous day --> "current values" equiv
        float prev_left_val = evaluate_condition_vals(prev_day, left_operand, previous_values, entry_price);  // prev day left op val
        float prev_right_val = evaluate_condition_vals(prev_day, right_operand, previous_values, entry_price);  // prev day right op val

        // validate comparison ops
        if (operation == "crossunder") {  // crossover --> left <= right prev, left > right curr
            if (prev_left_val <= prev_right_val && left_op_val > right_op_val) {
                return true;
            }
        } else {  // crossunder --> left >= right prev, left < right curr
            if (prev_left_val >= prev_right_val && left_op_val < right_op_val) {
                return true;
            }
        }

        return false;  // op not true

    }

    // check if condition expression is true/false (only >, <, >=, <= ops)
    bool basic_condition_operation(float left_op_val, float right_op_val, string operation) {

        // validate comparison ops
        if (operation == ">") {
            if (left_op_val > right_op_val) {
                return true;
            }
        } else if (operation == ">=") {
            if (left_op_val >= right_op_val) {
                return true;
            }
        } else if (operation == "<") {
            if (left_op_val < right_op_val) {
                return true;
            }
        } else {  // case: "<="
            if (left_op_val <= right_op_val) {
                return true;
            }
        }

        return false;  // op not true
    }

    // compute indicator values from strategy
    float compute_indicator(int curr_day, string type, string source, int period) {

        float value{};
        int source_index = get_data_index(source);  // get data index

        // define time interval for compute
        int start_day = curr_day - period + 1;
        int end_day = curr_day;

        // base on indicator type
        if (type == "SMA") {
            float total_sum{};  // 0.0f
            for (int i = start_day; i <= end_day; i++) {
                total_sum += market_data[i][source_index];  // day, bar field
            }
            value = total_sum / period;  // short sma 
        }

        return value;

    }

    // break line into vector parts
    vector<string> str_to_vec(string text_line) {

        vector<string> split_parts;
        string s;  // curr token
        stringstream stream(text_line);  // input stream
        
        while (stream >> s) {  // bitwise char read
            split_parts.push_back(s);  // append
        }

        return split_parts;

    }

    // handle operation function calls (consolidate)
    bool operation_handler(
        string operation, int curr_day, string left_operand, string right_operand, map<string, 
        float> current_values, map<string, float> previous_values, float entry_price
    ) {

        float left_val = evaluate_condition_vals(curr_day, left_operand, current_values, entry_price);  // left op val
        float right_val = evaluate_condition_vals(curr_day, right_operand, current_values, entry_price);  // right op val     

        bool condition_res{};
        if (operation == "crossover" || operation == "crossunder") {  // case: cross ops
            condition_res = cross_condition_operation(
                left_val, right_val, operation,
                curr_day, left_operand, right_operand, 
                previous_values, entry_price
            );
            cout << "Rule (Cross): " << operation << "(" << left_val << ", " << right_val << ")" << " --> " << condition_res << endl;
        } else {  // case: (>, <, >=, <=) --> not cross ops
            condition_res = basic_condition_operation(left_val, right_val, operation);
            cout << "Rule (Basic): " + to_string(left_val) + " " + operation + " " + to_string(right_val) << " --> " << condition_res << endl;
        }

        return condition_res;  // store bool

    }

    // compute share quantity to use for trade
    float compute_share_quantity(float entry_price, float size_percentage) {

        float dollars_to_put = capital * (size_percentage / 100);  // from capital
        int num_shares = static_cast<int>(round(dollars_to_put / entry_price));  // no user warning, floor round
        return num_shares;

    }

    // compute pnl from current entry/exit cycle
    float compute_current_pnl(int share_quantity, float entry_price, float exit_price, string asset_direction) {

        float current_pnl{};
        if (asset_direction == "long") {  // case: long term
            current_pnl = (exit_price - entry_price) * share_quantity;
        } else {  // case: short term (flip order)
            current_pnl = (entry_price - exit_price) * share_quantity;
        }

        cout << "Entry Price: " << entry_price << endl;
        cout << "Exit Price: " << exit_price << endl;
        cout << "Shares: " << share_quantity << endl;
        cout << "Current Unrealized PnL: $" << current_pnl << endl;

        return current_pnl;

    }

    // compute percent difference in pnl from initial capital
    float compute_pnl_percentage_diff(float remaining_capital) {

        float price_diff = abs(capital - remaining_capital);
        return (price_diff / capital) * 100;  // 1-100% scale

    }

public:  // callable outside

    BacktestingEngine(  // engine constructor
        const map<int, vector<int>>& m, const array<string, 5>& b, float c, Strategy& s
    ) : market_data(m), bar_fields(b), capital(c), strategy(s) {};

    auto run_data_through_engine_logic() {

        // store daily indicator values
        map<string, float> current_values;
        map<string, float> previous_values;

        // state and buy info
        bool in_entry_cycle = false;  // trade state
        float share_quantity{};

        float unrealized_pnl{};  // if were to exit
        float realized_pnl{};  // after official exit
        float final_pnl{};  // final profit/loss

        // track entry/exit prices for pnl
        float price_entered_at{};
        float price_exited_at{};
        bool pending_entry{};
        bool pending_exit{};

        cout << "Initial Capital: $" << capital << endl;

        // get minimum start day needed for compute
        int min_start_day{};  // start to 0
        for (auto indicator : strategy.indicators) {
            if (min_start_day < indicator.period) {
                min_start_day = indicator.period;  // update
            }
        }

        // point to last day from ordered hashmap
        auto last_day_ptr = prev(market_data.end());  // end = after last --> no data, prev
        int last_day_in_data = last_day_ptr->first;  // deref

        // print each data point from hashmap
        for (auto market_val : market_data) {  // auto data type
            
            // map market data to variables init
            int curr_day = market_val.first;
            float open_val = market_val.second[0];
            float high_val = market_val.second[1];
            float low_val = market_val.second[2];
            float close_val = market_val.second[3];
            int total_volume = market_val.second[4];

            cout << "\nDay: " << curr_day << 
            " | Open: " << open_val << 
            " | High: " << high_val << 
            " | Low: " << low_val << 
            " | Close: " << close_val << 
            " | Volume: " << total_volume << 
            endl; 
            cout << "----------------------------" << endl;

            // skip until valid day for indicator compute
            if (curr_day < min_start_day) {
                cout << "No signal yet. Need more information. Skipping current day." << endl;
                continue;
            }

            // enter at the open value of day after
            if (pending_entry) {  // open trade

                cout << "Entered trade!" << endl;
                cout << "----------------------------" << endl;
                price_entered_at = open_val;  // entry price

                // compute num shares to use in trade
                share_quantity = compute_share_quantity(price_entered_at, strategy.pos_settings.size_percentage);
                
                // update states
                in_entry_cycle = true;
                pending_entry = false;

            }

            // exit at the open value of day after
            if (pending_exit) {  // close trade

                cout << "Exited trade!" << endl;
                cout << "----------------------------" << endl;
                price_exited_at = open_val;  // exit price

                // compute num shares to use in trade
                share_quantity = compute_share_quantity(price_entered_at, strategy.pos_settings.size_percentage);

                // update exited pnl
                realized_pnl = compute_current_pnl(
                    share_quantity, price_entered_at, 
                    price_exited_at, strategy.entry_rules[0].direction  //! fix incase directions not all same
                );

                final_pnl += realized_pnl;  // add to final pnl

                // reset values
                unrealized_pnl = 0.0;  // not in trade
                price_entered_at = 0.0;

                // update states
                in_entry_cycle = false;
                pending_exit = false;
                cout << "----------------------------" << endl;

            }

            // compute indicator values per indicator type
            float value{};
            for (auto indicator : strategy.indicators) {
                value = compute_indicator(curr_day, indicator.type, indicator.source, indicator.period);
                current_values[indicator.name] = value;  // store value variables
                cout << "Indicator " + indicator.name + ": " + to_string(value) << endl;
            }
            cout << "----------------------------" << endl;

            // check entry conditions (when not in trade)
            if (!in_entry_cycle) {

                // check if all entry rules are valid
                cout << "-- Entry Conditions --" << endl;
                vector<bool> entry_condition_outcomes{};  // store condition true/false
                for (auto entry_rule : strategy.entry_rules) {
                    for (auto condition : entry_rule.conditions) {

                        // divide condition components
                        string left_operand = condition.left_operand;
                        string right_operand = condition.right_operand;
                        string operation = condition.operation;  // forms --> (left operator right)

                        bool entry_condition_res = operation_handler(
                            operation, curr_day, left_operand, right_operand, 
                            current_values, previous_values, price_entered_at
                        );
                        entry_condition_outcomes.push_back(entry_condition_res);  // store bool
                    
                    }
                }

                // all instances of entry rule being true
                bool entry_true = all_of(  // lambda func
                    entry_condition_outcomes.begin(), 
                    entry_condition_outcomes.end(), 
                    [](bool b){ return b; }  // bool val
                );  //! currently checks if ALL are true, could add OR in future
            
                // check entry rule conditions with operator
                if (entry_true) {

                    // update trade entry variables
                    unrealized_pnl = 0.0;
                    price_exited_at = 0.0;
                    pending_entry = true;  // enter next day

                    // update previous day values
                    previous_values = current_values;  // after using
                    continue;  // skip to next day
                
                }

            } else {  // case: when already in a trade

                cout << "Currently in trade..." << endl;
                cout << "----------------------------" << endl;

                // check exit conditions incase trade is invalid
                cout << "-- Exit Conditions --" << endl;
                vector<bool> exit_condition_outcomes{};  // store condition true/false
                for (auto exit_rule : strategy.exit_rules) {
                    for (auto condition : exit_rule.conditions) {                    

                        // divide condition components
                        string left_operand = condition.left_operand;
                        string right_operand = condition.right_operand;
                        string operation = condition.operation;  // forms --> (left operator right)

                        //! need to breakdown equation sub-conditions (such as * 0.97 case)
                        bool exit_condition_res = operation_handler(
                            operation, curr_day, left_operand, right_operand, 
                            current_values, previous_values, price_entered_at
                        );
                        exit_condition_outcomes.push_back(exit_condition_res);  // store bool

                    }
                }

                cout << "----------------------------" << endl;

                // any instance of exit rule being true
                bool exit_true = any_of(  // lambda func
                    exit_condition_outcomes.begin(), 
                    exit_condition_outcomes.end(), 
                    [](bool b){ return b; }  // bool val
                );  //! currently checks if ALL are true, could add OR in future

                // state conditionals during entry
                if (curr_day == last_day_in_data) {  // case: hit last day (while still in trade)
                    cout << "End of search days reached while still in trade." << endl;
                    final_pnl += unrealized_pnl;  // add all trade pnl's to final pnl

                } else if (exit_true) {  // case: exit rule hit
                    cout << "Exit rule has been hit while in trade." << endl;
                    pending_exit = true;  // exit next day

                } else {  // case: continue
                    cout << "Still in trade. Adding unrealized pnl." << endl;
                    unrealized_pnl += compute_current_pnl(  // running cycle sum
                        share_quantity, price_entered_at, 
                        close_val, strategy.entry_rules[0].direction  //! fix incase directions not all same
                    );

                }
            }

            cout << "----------------------------" << endl;

        }

        // fallback if pending exit after days end
        if (pending_exit) {

            cout << "Exited trade!" << endl;
            cout << "----------------------------" << endl;

            price_exited_at = market_data.at(last_day_in_data)[0];  // open of last day (fallback)

            // compute num shares to use in trade
            share_quantity = compute_share_quantity(price_entered_at, strategy.pos_settings.size_percentage);

            // update exited pnl
            realized_pnl = compute_current_pnl(
                share_quantity, price_entered_at, 
                price_exited_at, strategy.entry_rules[0].direction  //! fix incase directions not all same
            );

            final_pnl += realized_pnl;  // add to final pnl

            // reset values
            unrealized_pnl = 0.0;  // not in trade
            price_entered_at = 0.0;

            // update states
            in_entry_cycle = false;
            pending_exit = false;

        }

        // update capital based on results
        float updated_capital = capital + final_pnl;
        float capital_diff_percent = compute_pnl_percentage_diff(updated_capital);  // % pnl

        return make_tuple(final_pnl, updated_capital, capital_diff_percent);

    }

};

#endif