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
fs = 1000
cutoff = 3
order = 6
out_signal = butter_lowpass_filter(bright, cutoff, fs, order)
th_args = np.argwhere(out_signal>0.2)
print(np.min(th_args), np.max(th_args))
print(f'{end-start} s')

fig, axs = plt.subplots(1, 1)
axs.plot(out_signal)
# axs[1].plot(derivative)
# fig = plt.figure()
# plt.imshow(data[2000], cmap="gray")
plt.show()