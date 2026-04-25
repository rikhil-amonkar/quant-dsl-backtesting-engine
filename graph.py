import matplotlib.pyplot as plt
from matplotlib.ticker import MaxNLocator

# plot pnl results and market prices to graph visual
def plot_pnl_values_to_graph(running_capital, daily_hash_update) -> None:
    
    daily_info_dump_parsed = [open_val for open_val in list(daily_hash_update.values())]  # open val
    
    # ohlcv breakdown --> volume too large
    day_open_prices = [val[0] for val in daily_info_dump_parsed]
    day_high_prices = [val[1] for val in daily_info_dump_parsed]
    day_low_prices = [val[2] for val in daily_info_dump_parsed]
    day_close_prices = [val[3] for val in daily_info_dump_parsed]
            
    x_vals = [(n + 1) for n in range(0, len(day_open_prices))]  # days
    # print(len(day_open_prices), len(x_vals), len(y_vals))

    fig, ax1 = plt.subplots(figsize=(14, 6))  # width/height
    ax1.plot(x_vals, day_open_prices, label="Open Price")
    ax1.plot(x_vals, day_close_prices, label="Close Price")
    ax1.plot(x_vals, day_high_prices, label="High Price")
    ax1.plot(x_vals, day_low_prices, label="Low Price")
        
    ax2 = ax1.twinx()
    ax2.plot(x_vals, running_capital, label="Current Capital", drawstyle="steps-post")  # hold val till next point
    
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
        
    ax1.set_title("Simulated Capital Loss/Gain Over Market Data Period")
    ax1.set_xlabel("Days")
    
    plt.show()