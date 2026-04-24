import matplotlib.pyplot as plt
from matplotlib.ticker import MaxNLocator
import numpy as np

# plot pnl results and market prices to graph visual
def plot_pnl_values_to_graph(y_vals, daily_hash_update) -> None:
    
    daily_info_dump_parsed = [open_val for open_val in list(daily_hash_update.values())]  # open val
    # day_open_prices = [val[0] for i, val in enumerate(daily_info_dump_parsed) if i < len(y_vals)]
    day_open_prices = [val[0] for val in daily_info_dump_parsed]
    day_high_prices = [val[1] for val in daily_info_dump_parsed]
    day_low_prices = [val[2] for val in daily_info_dump_parsed]
    day_close_prices = [val[3] for val in daily_info_dump_parsed]
    
    # print("No errors after list compression.")

    # with open("log.txt") as f:
    #     data = f.readlines()
            
    x_vals = [(n + 1) for n in range(0, len(day_open_prices))]  # days
    # y_raw_vals = [d for d in data]  # prices
    # y_vals = [float(raw.split()[4]) for raw in y_raw_vals]
    
    # print(len(day_open_prices), len(x_vals), len(y_vals))

    fig, ax = plt.subplots()
    # ax.plot(x_vals, y_vals, label="PnL")
    ax.plot(x_vals, day_open_prices, label="Open Price")
    ax.plot(x_vals, day_close_prices, label="Close Price")
    ax.plot(x_vals, day_high_prices, label="High Price")
    ax.plot(x_vals, day_low_prices, label="Low Price")
    ax.legend()
    
    # print("No errors after plot.")    
    # print(y_vals)
    # print(day_open_prices)    

    # y_ax_min, y_ax_max = min(y_vals+day_open_prices), max(y_vals+day_open_prices)
    # step = (y_ax_max - y_ax_min) / len(y_vals)  # max-min/intervals
    # plt.yticks(np.arange(y_ax_min, y_ax_max, step))
    ax.yaxis.set_major_locator(MaxNLocator(nbins=10))
    plt.show()