import cv2 as cv
import numpy as np
import matplotlib.pyplot as plt
import os, sys
import time
from algorithms import *

window = np.zeros([14, 14])

def video_handler(cap, chain):
    cnt = 0
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
    cap = cv.VideoCapture(file)
    alg_chain = [gray, match]
    video_handler(cap, alg_chain)
    cv.waitKey(0) 