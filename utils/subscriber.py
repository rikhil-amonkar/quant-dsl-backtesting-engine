import zmq

# connect to publisher socket
context = zmq.Context()
socket = context.socket(zmq.SUB)  # collector
socket.connect('tcp://localhost:5555')  # local

# subscribe to all messages
socket.setsockopt_string(zmq.SUBSCRIBE, "")

for i in range(5):
    
    content = socket.recv_string()  # get data
    print("Time Recieved:", content)