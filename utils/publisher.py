import zmq
import time
from datetime import datetime

# create and initialize socket
context = zmq.Context()
socket = context.socket(zmq.PUB)  # sender
socket.bind('tcp://localhost:5555')  # local

while True:
    
    # create data to be sent
    current_time = datetime.now()
    formatted_time = current_time.strftime("%Y-%m-%d %H:%M:%S")
    print("Time Sent:", formatted_time)
    
    # send data to socket
    socket.send_string(formatted_time)
    time.sleep(1)
    
    