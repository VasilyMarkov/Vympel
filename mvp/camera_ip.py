#!/bin/bash 
import cv2 as cv
from picamera2 import Picamera2
import numpy as np
import socket
from struct import pack
from scipy.signal import butter, lfilter
import collections 
import subprocess, sys 

cv.startWindowThread()

picam2 = Picamera2()
picam2.configure(picam2.create_preview_configuration(main={"format": 'XRGB8888', "size": (640, 480)}))
picam2.start()

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# host, port = '192.168.1.220', 65000
host, port = '127.0.0.1', 65000
server_address = (host, port)

print("UDP server up and listening")

def butter_lowpass_filter(data, cutoff, fs, order=5):
    b, a = butter_lowpass_filter(cutoff, fs, order=order)
    y = lfilter(b,a, data)
    return y

threshold = 51
fs = 1000
cutoff = 70
order = 6

buffer = collections.deque(maxlen=20)
tmp = np.zeros(10)
while True:
    im = picam2.capture_array()

    gray = cv.cvtColor(im, cv.COLOR_BGR2GRAY)
    ret, thresh = cv.threshold(gray, threshold, 255, cv.THRESH_BINARY)
    array_frame = np.asarray(thresh)
    white_pix = (array_frame > threshold).sum()
    buffer.append(white_pix)
    avg = np.mean(np.array(buffer)).astype(int)
    # print(white_pix, avg)   
    msg = pack('1I', avg)
    sock.sendto(msg, server_address) 	
    # subprocess.run("/usr/bin/vcgencmd measure_temp", shell = True, executable="/bin/bash")
    cv.imshow("Camera", im)
    cv.imshow("Threshold", thresh)
    if cv.waitKey(1) & 0xFF == ord('q'):
        break
    # if cv.waitKey(0):
    #     break
    
