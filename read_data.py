import numpy as np
import matplotlib.pyplot as plt
from utils import *


if __name__ == "__main__":
    data_path = "data/"
    files = get_file_list(data_path)
    print(files)
    data = np.load(data_path+files[2])
    fig, axis = plt.subplots()
    axis.plot(data, 'g-')
    plt.show()
    