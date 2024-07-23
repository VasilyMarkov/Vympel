import cv2 as cv
import numpy as np
from numpy import asarray
import matplotlib.pyplot as plt
from scipy.signal import butter, lfilter, fftconvolve
import numpy.fft as fft
import socket
from struct import pack
import collections 

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
host, port = '127.0.0.1', 65000
server_address = (host, port)

def butter_lowpass(cutoff, fs, order=5):
    return butter(order, cutoff, fs=fs, btype='low', analog=False)

def butter_lowpass_filter(data, cutoff, fs, order=5):
    b, a = butter_lowpass(cutoff, fs, order=order)
    y = lfilter(b, a, data)
    return y

def threshold(cap):
    signal = []
    cnt_frame = 0
    threshold = 50
    buffer = collections.deque(maxlen=20)
    while cap.isOpened():
        ret, frame = cap.read()
        if ret:
            cnt_frame += 1
            gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
            crop = gray[100:600, 0:600]
            ret, thresh = cv.threshold(crop, threshold, 255, cv.THRESH_BINARY)
            array_frame = asarray(thresh)
            white_pix = (array_frame > threshold).sum()
            buffer.append(white_pix)
            avg = np.mean(np.array(buffer)).astype(int)
            msg = pack('1I', avg)
            sock.sendto(msg, server_address) 	
            cv.imshow('frame', thresh)
        if cv.waitKey(1) == ord('q'):
            break

    t = np.arange(0, cnt_frame, 1)

    fs = 1000
    cutoff = 70
    order = 6
    # out_signal = butter_lowpass_filter(signal, cutoff, fs, order)
    fig, axis = plt.subplots()
    # axis.plot(t, signal)
    axis.plot(t, signal, 'g-')
    plt.show()
    return stop

def correlation(cap, max_frame):
    corr = []
    cnt = 0
    while cap.isOpened():
        ret, frame = cap.read()
        gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
        fft_frame = fft.fft2(gray)
        # fft_centered = np.fft.fftshift(fft_frame)
        fft_normalized = np.log(1 + abs(fft_frame))
        cv.imshow('fft', fft_normalized)
        cv.putText(frame,f'Frame:{cnt}',(50, 50),1,2,(0, 0, 0), cv.LINE_4)
        # if ret:
        #     corr.append(fftconvolve(gray, max_tframe, mode='full'))
        if cv.waitKey(1) == ord('q'):
            break
        cnt += 1
    plt.plot(corr)
    plt.show()

def extract_max_frame(cap):
    cnt_frame = 0
    max_frame = 0
    while cap.isOpened():
        ret, frame = cap.read()
        if ret:
            cv.imshow('frame', frame)
            gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
            if cnt_frame == 300:
                max_frame = gray
            cnt_frame += 1

        if cv.waitKey(1) == ord('q'):
            cv.imwrite('max_frame.png', max_frame)
            print(max_frame.shape)
            break


def mat2vec(A):
    return np.reshape(np.flipud(A), np.prod(A.shape))

cap = cv.VideoCapture("new_video/hc-20w-40(octan)6.avi")

# extract_max_frame(cap)
# max_frame = cv.cvtColor(cv.imread('max_frame.png'), cv.COLOR_BGR2GRAY)
#
# mat2vec(max_frame)
#
# U, S, V = np.linalg.svd(max_frame)
#
# comp = U[..., :50] @ np.diag(S[:50]) @ V[:50, ...]
#
# fft_frame = fft.fft2(max_frame)
# fft_centered = np.fft.fftshift(fft_frame)
# fft_normalized = np.log(1 + abs(fft_centered))
# correlation(cap, None)
threshold(cap)
# plt.imshow(fft_normalized, cmap="gray")
# plt.show()


plt.imshow(threshold(cap))
plt.show()