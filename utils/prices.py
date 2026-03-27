import yfinance as yf

# set parameters
ticker = 'AAPL'
start = '2023-01-01'
end = '2023-01-10'

# initialize ticker
data = yf.download(ticker, start, end)

# get selected data as dataframe
subset = data[['Open', 'High', 'Low', 'Close', 'Volume']]

print(subset)