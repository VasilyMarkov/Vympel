import socket, pickle 
import numpy as np
import matplotlib.pyplot as plt

def receiveData():
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind(('', 65000))
    values = []
    while True:
        msg = sock.recv(4096)
        obj = pickle.loads(msg)
        print(obj)
        values.append(obj[0])
        if obj[1] == False:
            return values

if __name__ == "__main__":
    data = np.array(receiveData())
    fig, axis = plt.subplots()
    axis.plot(data, 'g-')
    plt.show()
