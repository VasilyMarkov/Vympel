import cv2 as cv
import numpy as np
import matplotlib.pyplot as plt
import os, sys
import time
from algorithms import *

window = np.zeros([14, 14])

def showHist(frame):
    hist = np.zeros([256, 3])
    hist[:, 0] = cv.calcHist([frame],[0],None,[256],[0,256]).reshape(256)
    hist[:, 1] = cv.calcHist([frame],[1],None,[256],[0,256]).reshape(256)
    hist[:, 2] = cv.calcHist([frame],[2],None,[256],[0,256]).reshape(256)
    fig, axs = plt.subplots(3,1)
    color = ('b','g','r')
    axs[0].plot(hist[:, 0],color = color[0])
    axs[1].plot(hist[:, 1],color = color[1])
    axs[2].plot(hist[:, 2],color = color[2])

nums = [50, 100, 150, 200, 250, 300, 350, 400, 450]
pixels = []
def video_handler(cap, chain):
    cnt = 0
    num = 0
    threshold = 50
    while cap.isOpened():
        ret, frame = cap.read()
        if ret:
            cv.imshow('frame', frame)
            for alg in chain:
                frame = alg(frame)  
            array_frame = np.asarray(frame)
            white_pix = (array_frame > threshold).sum()
            pixels.append(white_pix)
            cv.imshow('process', frame)
            time.sleep(0.04)

            cnt += 1
        if cv.waitKey(1) == ord('q'):
            break
    plt.plot(pixels)
    cap.release()
    cv.destroyAllWindows()


file = ''.join(sys.argv[1:])

if not os.path.isfile(file):
    print('File not exists')
else:
    cap = cv.VideoCapture(file)
    alg_chain = [threshold]
    video_handler(cap, alg_chain)
    cv.waitKey(0) 
