import cv2 as cv
import numpy as np
import matplotlib.pyplot as plt
import time
import socket
from scipy.signal import butter, lfilter, fftconvolve
from scipy import signal
import json

def exp_smooth(x, s, k = 0.2):
    return k*x + (1-k)*s

def butter_lowpass(cutoff, fs, order=5):
    return butter(order, cutoff, fs=fs, btype='low', analog=False)

def butter_lowpass_filter(data, cutoff, fs, order=5):
    b, a = butter_lowpass(cutoff, fs, order=order)
    y = lfilter(b, a, data)
    return y

b, a = signal.iirfilter(10, 2*np.pi*50, rs=200, btype='lowpass', analog=True, ftype='butter')

jdata = {
    'bright': float,
    'low_passed': float,
    'filtered': float
}

cap = cv.VideoCapture("video.mp4")

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
host, port = '127.0.0.1', 65000
server_address = (host, port)

fs = 1000
cutoff = 3
order = 6
# out_signal = butter_lowpass_filter(bright, cutoff, fs, order)


data = []
brightness = 0
while cap.isOpened():
    ret, frame = cap.read()
    if ret:
        gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
        tmp = np.sum(gray)
        ytmp = signal.iirfilter(b, a, tmp)
        brightness = exp_smooth(tmp, brightness)
        jdata['bright'] = float(tmp)
        jdata['low_passed'] = float(ytmp)
        jdata['filtered'] = float(brightness)
        cv.imshow('frame', frame)
        json_data = json.dumps(jdata)
        bytes_data = json_data.encode('utf-8')
        sock.sendto(bytes_data, server_address) 	
        time.sleep(0.05)
    if cv.waitKey(1) == ord('q'):
        break

