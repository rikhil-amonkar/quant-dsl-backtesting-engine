import matplotlib.pyplot as plt
import numpy as np

# plot pnl results and market prices to graph visual
def plot_pnl_values_to_graph(y_vals, daily_hash_update) -> None:
    
    day_open_prices = [open_val for open_val in list(daily_hash_update.values())]  # open val
    day_open_prices_adj = [val[0] for i, val in enumerate(day_open_prices) if i < len(y_vals)]
    
    # print("No errors after list compression.")

    # with open("log.txt") as f:
    #     data = f.readlines()
            
    x_vals = [(n + 1) for n in range(0, len(y_vals))]  # days
    # y_raw_vals = [d for d in data]  # prices
    # y_vals = [float(raw.split()[4]) for raw in y_raw_vals]
    
    # print(len(day_open_prices_adj), len(x_vals), len(y_vals))

    fig, ax = plt.subplots()
    ax.plot(x_vals, y_vals, label="PnL")
    ax.plot(x_vals, day_open_prices_adj, label="Open Price")
    ax.legend()
    
    # print("No errors after plot.")    
    # print(y_vals)
    # print(day_open_prices_adj)    

    y_min, y_max = min(y_vals+day_open_prices_adj), max(y_vals+day_open_prices_adj)
    step = (y_max - y_min) / len(y_vals)  # max-min/intervals
    plt.yticks(np.arange(y_min, y_max, step))
    plt.show()