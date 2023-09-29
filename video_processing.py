import cv2 as cv
import numpy as np
from numpy import asarray
import matplotlib.pyplot as plt
from scipy.signal import butter, lfilter, freqz

cap = cv.VideoCapture("test.mp4")
cnt = 0
array_frame = []
signal = []
cnt_frame = 0

def butter_lowpass(cutoff, fs, order=5):
    return butter(order, cutoff, fs=fs, btype='low', analog=False)

def butter_lowpass_filter(data, cutoff, fs, order=5):
    b, a = butter_lowpass(cutoff, fs, order=order)
    y = lfilter(b, a, data)
    return y
threshold = 100
while cap.isOpened():
    ret, frame = cap.read()
    if ret:
        cnt_frame += 1
        gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
        crop = gray[100:600, 0:600]
        ret, thresh = cv.threshold(crop, threshold, 255, cv.THRESH_BINARY)
        array_frame = asarray(thresh)
        white_pix = (array_frame > threshold).sum()
        signal.append(white_pix)
        cv.imshow('frame', thresh)
    if cv.waitKey(1) == ord('q'):
        break
print("End")

t = np.arange(0, cnt_frame, 1)

fs = 1000
cutoff = 50
order = 6
out_signal = butter_lowpass_filter(signal, cutoff, fs, order)
fig, axis = plt.subplots()
# axis.plot(t, signal)
axis.plot(t, out_signal, 'g-')
plt.show()

cap.release()
cv.destroyAllWindows()
