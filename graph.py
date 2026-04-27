import matplotlib.pyplot as plt
from matplotlib.ticker import MaxNLocator

# plot pnl results and market prices to graph visual
def plot_pnl_values_to_graph(running_capital, daily_hash_update) -> None:
    
    daily_info_dump_parsed = [open_val for open_val in list(daily_hash_update.values())]  # open val
    initial_capital = [running_capital[0]] * len(running_capital)
    final_capital = [running_capital[-1]] * len(running_capital)
    
    # ohlcv breakdown --> volume too large
    day_open_prices = [val[0] for val in daily_info_dump_parsed]
    day_high_prices = [val[1] for val in daily_info_dump_parsed]
    day_low_prices = [val[2] for val in daily_info_dump_parsed]
    day_close_prices = [val[3] for val in daily_info_dump_parsed]
            
    x_vals = [(n + 1) for n in range(0, len(day_open_prices))]  # days

    fig, ax1 = plt.subplots(figsize=(14, 8))  # width/height
    open_line = ax1.plot(x_vals, day_open_prices, label="Open Price", color="#1f77b4")
    close_line = ax1.plot(x_vals, day_close_prices, label="Close Price", color="#ff7f0e")
    high_line = ax1.plot(x_vals, day_high_prices, label="High Price", color="#2ca02c")
    low_line = ax1.plot(x_vals, day_low_prices, label="Low Price", color="#d62728")
        
    ax2 = ax1.twinx()
    init_cap_line = ax2.plot(x_vals, initial_capital, linestyle=":", label="Initial Capital", color="grey")  # starting cap (dotted)
    fin_cap_line = ax2.plot(x_vals, final_capital, linestyle=":", label="Final Capital", color="grey")  # starting cap (dotted)
    curr_cap_line = ax2.plot(x_vals, running_capital, label="Current Capital", drawstyle="steps-post", color="black")  # hold val till next point
    
    # set axis titles
    ax1.set_ylabel("Price ($)")
    ax2.set_ylabel("Capital ($)")
    
    # scale y-axis steps
    ax2.yaxis.set_major_locator(MaxNLocator(nbins=10))
    ax1.yaxis.set_major_locator(MaxNLocator(nbins=10))
        
    # track indexes of capital changes
    change_indices = []
    for i in range(1, len(running_capital)):
        if running_capital[i] != running_capital[i - 1]:
            change_indices.append(i)  # store

    # fill area between capital changes
    for j in range(len(change_indices)):
        start_idx = change_indices[j]
        end_idx = change_indices[j + 1] if j + 1 < len(change_indices) else len(running_capital) - 1  # to next change
        
        # update running capital
        old_cap = running_capital[start_idx - 1]
        new_cap = running_capital[start_idx]
        color = "green" if new_cap >= old_cap else "red"  # gain/loss color
        
        ax2.fill_between(  # fill area (box)
            x_vals[start_idx:end_idx + 1],
            old_cap,
            new_cap,
            color=color,
            alpha=0.3
        )

    ax1.set_title("Portfolio Value Over Simulated Backtest Period")
    ax1.set_xlabel("Days")
    
    all_lines = open_line + close_line + high_line + low_line + curr_cap_line  # combine
    all_labels = [line.get_label() for line in all_lines]  # extract labels
    
    ax2.legend(all_lines, all_labels, loc=4).set_zorder(100)
    
    plt.savefig("simulated_backtesting_pnl_graph.png")
    plt.show()