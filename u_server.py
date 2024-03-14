import socket, pickle 
import numpy as np
import matplotlib.pyplot as plt
import datetime


def receiveData():
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind(('0.0.0.0', 65000))
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
    time = datetime.datetime.now()
    time = time.strftime("%Y-%m-%d %H:%M")
    file = f"data/{time}_data.npy"
    np.save(file, data)
    fig, axis = plt.subplots()
    axis.plot(data, 'g-')
    plt.show()
    arr_loaded = np.load(file)
    # print(arr_loaded)

