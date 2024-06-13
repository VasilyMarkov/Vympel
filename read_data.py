import numpy as np
import matplotlib.pyplot as plt
from utils import *
from scipy.signal import find_peaks
from scipy.signal import butter, lfilter, freqz
from scipy import signal
import sys
from pathlib import Path


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
    data_path = "video/"
    files = get_file_list(str(Path.cwd()) + "/new_data/")
    file = files[3]
    file = 'test_data/test.mp4.npy'
    # data = np.load(data_path+file)
    data = np.load(file)
    max = np.max(data)
    data = data/max
    filt_data = butter_lowpass_filter(data, cutoff, fs, order)
    peaks, _ = find_peaks(data, prominence=0.8)
    f, Pxx_den = signal.periodogram(filt_data, fs)    
    lower_bound = 0.15
    upper_bound = 0.2
    indices = np.where((filt_data > lower_bound) & (filt_data < upper_bound))
    
    grad = np.gradient(filt_data)
    file_br = 'test_data/test.mp4_br.npy'
    br = np.load(file_br)
    fig, axis = plt.subplots(1,2)
    axis[0].plot(filt_data, 'g-')
    axis[1].plot(br, 'r-')
    # axis.plot(grad, 'b-')
    # axis.plot(peaks, data[peaks], "x")
    # axis.fill_betweenx(filt_data, 1039, 1170, color='red')
    # axis.fill_betweenx(filt_data, 6279, 6384, color='red')
    # axis.set_title(file)
    # axis.plot(f, Pxx_den)
    # axis.set_xlim(0, 200)
    plt.show()
    