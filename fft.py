import cv2 as cv
import numpy as np
import numpy.fft as fft
import matplotlib.pyplot as plt
import time

def proc(cap):
    while cap.isOpened():
        ret, frame = cap.read()
        start = time.time()
        fft_frame = fft.fft2(frame)
        fft_centered = np.fft.fftshift(fft_frame)
        fft_normalized = np.log(1 + abs(fft_centered))
        # cv.imshow('fft', abs(fft_frame))
        end = time.time()
        print(end-start)
        if cv.waitKey(1) == ord('q'):
            break    

cap = cv.VideoCapture("test2.mp4")

proc(cap)