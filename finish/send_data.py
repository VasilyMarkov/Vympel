import cv2 as cv
import numpy as np
import matplotlib.pyplot as plt
import time
import socket
from scipy.signal import butter, lfilter, fftconvolve
import json

def butter_lowpass(cutoff, fs, order=5):
    return butter(order, cutoff, fs=fs, btype='low', analog=False)

def butter_lowpass_filter(data, cutoff, fs, order=5):
    b, a = butter_lowpass(cutoff, fs, order=order)
    y = lfilter(b, a, data)
    return y

jdata = {
    'bright': int
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
while cap.isOpened():
    ret, frame = cap.read()
    if ret:
        gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
        bright = np.sum(gray)
        jdata['bright'] = int(bright)
        cv.imshow('frame', frame)
        json_data = json.dumps(jdata)
        bytes_data = json_data.encode('utf-8')
        sock.sendto(bytes_data, server_address) 	
        time.sleep(0.05)
    if cv.waitKey(1) == ord('q'):
        break

