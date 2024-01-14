import cv2 as cv
import numpy as np
import numpy.fft as fft
import matplotlib.pyplot as plt
import time
import sys
import os


def cutFrame(cap, numFrame):
    cnt = 0
    while cap.isOpened(): 
        ret, frame = cap.read()
        if cnt == numFrame:            
            break 
        cnt += 1
        if cv.waitKey(1) == ord('q'):
            break  
    return frame

def fft_video(cap):
    while cap.isOpened():
        ret, frame = cap.read()
        start = time.time()
        fft_frame = fft.fft2(frame)
        fft_centered = np.fft.fftshift(fft_frame)
        fft_normalized = np.log(1 + abs(fft_centered))
        # cv.imshow('fft', abs(fft_frame))
        end = time.time()
        print(f'{end-start}, s')
        if cv.waitKey(1) == ord('q'):
            break    

def fft_img(img):
    fft_frame = fft.fft2(img)
    fft_centered = fft.fftshift(fft_frame)
    fft_normalized = np.log(1 + np.abs(fft_centered)) 
    return fft_normalized

cap = cv.VideoCapture("decan.mp4")
img = cv.cvtColor(cv.imread('img/decan/decan_150.png'), cv.COLOR_BGR2GRAY)

# total = int(cap.get(cv.CAP_PROP_FRAME_COUNT))
# print(total)
path = './img/decan/'
files = [f for f in os.listdir(path)]
fig, axs = plt.subplots(2, 3)
files = sorted(files)
print(files)
fft_img(cv.cvtColor(cv.imread(f'{path}{files[0]}'), cv.COLOR_BGR2GRAY))
axs[0, 0].imshow(fft_img(cv.cvtColor(cv.imread(f'{path}{files[0]}'), cv.COLOR_BGR2GRAY)), cmap = 'gray')
axs[0, 1].imshow(fft_img(cv.cvtColor(cv.imread(f'{path}{files[1]}'), cv.COLOR_BGR2GRAY)), cmap = 'gray')
axs[0, 2].imshow(fft_img(cv.cvtColor(cv.imread(f'{path}{files[2]}'), cv.COLOR_BGR2GRAY)), cmap = 'gray')
axs[1, 0].imshow(fft_img(cv.cvtColor(cv.imread(f'{path}{files[3]}'), cv.COLOR_BGR2GRAY)), cmap = 'gray')
axs[1, 1].imshow(fft_img(cv.cvtColor(cv.imread(f'{path}{files[4]}'), cv.COLOR_BGR2GRAY)), cmap = 'gray')
# axs[0, 0].imshow(fft_img(img), cmap = 'gray')
# plt.imshow(fft_img(img), cmap='gray')
plt.show()
cv.waitKey(0) 