import cv2 as cv
import matplotlib.pyplot as plt
import numpy as np
import os
import sys
from scipy.signal import argrelextrema
from scipy.signal import find_peaks


def cutFrame(cap, num_frame):
    cnt_frame = 0
    while cap.isOpened():
        ret, frame = cap.read()
        if ret:
            if cnt_frame == num_frame:
                cv.imwrite('frame.png', frame)
                return
            cnt_frame += 1

def regminima(img):
    x = 0
    minima = argrelextrema(img, np.less)
    out = np.zeros([img.shape[0], img.shape[1]])
    for i in range(img.shape[0]):
        for j in range(img.shape[1]):
            if i == minima[0][x] and j == minima[1][x] :
                out[i][j] = 1
                if x == (minima[0].shape[0]-1):
                    break
                x += 1
    return out


def detect(cap):
    while cap.isOpened():
        ret, frame = cap.read()
        if not ret:
            cap.release()
            cv.destroyAllWindows()
        hsv = cv.cvtColor(frame, cv.COLOR_BGR2HSV)
        lower_gray = np.array([0, 5, 50], np.uint8)
        upper_gray = np.array([179, 50, 255], np.uint8)
        mask_gray = cv.inRange(hsv, lower_gray, upper_gray)
        img_res = cv.bitwise_and(frame, frame, mask = mask_gray)
        cv.imshow('result', img_res)
        if cv.waitKey(1) == ord('q'):
            break  
    return

def black(cap):
    threshold = 120
    while cap.isOpened():
        ret, frame = cap.read()
        if not ret:
            cap.release()
            cv.destroyAllWindows()
        gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
        minima = regminima(gray)
        
        ret, thresh = cv.threshold(gray, threshold, 255, cv.THRESH_BINARY_INV)
        cv.imshow('thresh', minima)
        if cv.waitKey(1) == ord('q'):
            break  
    return


def unchange(cap):
    while cap.isOpened():
        ret, frame = cap.read()
        if not ret:
            cap.release()
            cv.destroyAllWindows()
        gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
        cv.imshow('frame', gray)
        if cv.waitKey(1) == ord('q'):
            break  
    return


def process(file, func):
    if not os.path.isfile(file):
        print('File not exists')
    else:
        cap = cv.VideoCapture(file)
        func(cap)

file = "new_video/hc-20w-40(octan).mp4"

# process(''.join(sys.argv[1:]))
# process(file, black)



# arr = np.array ([   
#     [5,5,5,5,5],
#     [5,2,2,5,5],
#     [5,5,5,5,5],
#     [5,5,3,3,5],
#     [5,5,5,5,5]
# ])
# cap = cv.VideoCapture(file)
# cutFrame(cap, 200)

img = cv.cvtColor(cv.imread('frame.png'), cv.COLOR_BGR2HSV)
v = img[:, :, 2]
blur = cv.GaussianBlur(v,(3,3),0)
minima = regminima(blur)
plt.imshow(minima, cmap="gray")
plt.show()
cv.waitKey(0) 
