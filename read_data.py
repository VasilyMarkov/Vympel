import numpy as np
import matplotlib.pyplot as plt
from utils import *


if __name__ == "__main__":
    data_path = "data/old_video_data/"
    files = get_file_list(data_path)
    print(files)
    file = files[0]
    data = np.load(data_path+file)
    fig, axis = plt.subplots()
    axis.plot(data, 'g-')
    axis.set_title(file)
    plt.show()
    