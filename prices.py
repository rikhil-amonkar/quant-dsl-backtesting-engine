import yfinance as yf

# get ohlcv values from market based on time period
def fetch_ohlcv_data_from_market_timeline(ticker: str, start: str, end: str) -> list[float]:

    # initialize ticker
    data = yf.download(ticker, start, end)

    # get selected data as dataframe
    subset = data[["Open", "High", "Low", "Close", "Volume"]]
    
    # convert to only value list
    values = subset.to_numpy().tolist()
    
    # {day, [o, h, l, c, v]} --> format 
    market_data = {}
    for i, ohlcv in enumerate(values):
        market_data[i+1] = ohlcv  # store to hashmap
        
    return dict(market_data)