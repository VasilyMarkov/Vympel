import numpy as np
import matplotlib.pyplot as plt
from utils import *
from scipy.signal import find_peaks
from scipy.signal import butter, lfilter, freqz
from scipy import signal
import sys



def butter_lowpass(cutoff, fs, order=5):
    return butter(order, cutoff, fs=fs, btype='low', analog=False)

def butter_lowpass_filter(data, cutoff, fs, order=5):
    b, a = butter_lowpass(cutoff, fs, order=order)
    y = lfilter(b, a, data)
    return y


order = 6
fs = 10e3      
cutoff = 500

b, a = butter_lowpass(cutoff, fs, order)

if __name__ == "__main__":
    data_path = "data/old_video_data/"
    # files = get_file_list(data_path)
    # file = files[0]
    # data = np.load(data_path+file)
    file = sys.argv[1]
    print(file)
    data = np.load(file)
    max = np.max(data)
    data = data/max
    print(max)
    filt_data = butter_lowpass_filter(data, cutoff, fs, order)
    peaks, _ = find_peaks(data, prominence=0.8)
    print(peaks)
    f, Pxx_den = signal.periodogram(filt_data, fs)    
    lower_bound = 0.15
    upper_bound = 0.2
    indices = np.where((filt_data > lower_bound) & (filt_data < upper_bound))
    # print(indices)
    fig, axis = plt.subplots()
    axis.plot(filt_data, 'g-')
    axis.plot(peaks, data[peaks], "x")
    axis.fill_betweenx(filt_data, 1039, 1170, color='red')
    axis.fill_betweenx(filt_data, 6279, 6384, color='red')
    axis.set_title(file)
    # axis.plot(f, Pxx_den)
    # axis.set_xlim(0, 200)
    plt.show()
    