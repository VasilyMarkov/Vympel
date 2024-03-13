import cv2 as cv
import numpy as np
import matplotlib.pyplot as plt
import os, sys
import time

filters = {
    'sharpness': np.array([[-1,-1,-1], [-1,9,-1], [-1,-1,-1]]), 
    'blur': np.ones((5,5), np.float32)/25
    }


def filter(frame):
    frame = cv.filter2D(frame, -1, filters["sharpness"])
    return frame


def cornerDetector(frame):
    th_lower = 100
    th_upper = 150
    frame = cv.Canny(frame, th_lower, th_upper)
    return frame


def video_handler(cap, chain):
    while cap.isOpened():
        ret, frame = cap.read()
        if ret:
            cv.imshow('frame', frame)
            for alg in chain:
                frame = alg(frame)           
            cv.imshow('process', frame)
            time.sleep(0.05)
        if cv.waitKey(1) == ord('q'):
            break
    cap.release()
    cv.destroyAllWindows()


file = ''.join(sys.argv[1:])

if not os.path.isfile(file):
    print('File not exists')
else:
    print(file)
    cap = cv.VideoCapture(file)
    alg_chain = [filter, cornerDetector]
    video_handler(cap, alg_chain)
    cv.waitKey(0) 