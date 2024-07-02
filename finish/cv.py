import cv2 as cv
import numpy as np
import matplotlib.pyplot as plt
import time
from scipy.signal import butter, lfilter, fftconvolve

# cap = cv.VideoCapture("video.mp4")

# data = []
# while cap.isOpened():
#     ret, frame = cap.read()
#     if ret:
#         cv.imshow('frame', frame)
#         data.append(frame)
#     if cv.waitKey(1) == ord('q'):
#         break

# np.save('data', data)

def butter_lowpass(cutoff, fs, order=5):
    return butter(order, cutoff, fs=fs, btype='low', analog=False)

def butter_lowpass_filter(data, cutoff, fs, order=5):
    b, a = butter_lowpass(cutoff, fs, order=order)
    y = lfilter(b, a, data)
    return y

file = 'data.npy'
start = time.time()

data = np.load(file)
end = time.time()
bright = np.sum(data, axis=(1,2))
init_data = bright[:500]
mean_init_data = np.mean(init_data)
bright = bright - mean_init_data
bright /= np.max(bright)
calib = bright[:500]
std = np.std(calib)

fs = 1000
cutoff = 3
order = 6
out_signal = butter_lowpass_filter(bright, cutoff, fs, order)
# out_signal = bright
th_args_x = np.argwhere(out_signal>std)

print(np.min(th_args_x), np.max(th_args_x))
print(f'Mean: {mean_init_data}, std: {std}')
# print(init_data)
print(f'{end-start} s')

fig, axs = plt.subplots(1, 2)

# axs.plot(x, out_signal, x, derivative)
axs[0].plot(out_signal)
axs[0].scatter(np.min(th_args_x), out_signal[np.min(th_args_x)], c = 'r')
axs[0].scatter(np.max(th_args_x), out_signal[np.max(th_args_x)], c = 'r')
axs[1].hist(calib, bins = 50)
# fig = plt.figure()
# plt.imshow(data[2000], cmap="gray")
plt.show()