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

def img_proc(path):
    files = [f for f in os.listdir(path) if f.endswith(".png")]
    fig, axs = plt.subplots(1,len(files))
    files = sorted(files)
    for i in range(len(files)):
        img = cv.cvtColor(cv.imread(f'{path}{files[i]}'), cv.COLOR_BGR2GRAY) 
        img = img[..., :np.min(img.shape)]
        edges = cv.Canny(img, 100, 200)
        gaussian_window = cv.getGaussianKernel(img.shape[0], 250)
        gaussian_window = gaussian_window*gaussian_window.T
        windowed_edges = edges*gaussian_window
        fft = fft_img(windowed_edges)
        axs[i].imshow(fft, cmap = 'gray')
        axs[i].axis('off')
        axs[i].set_title(files[i])

# decan_fft()

cap = cv.VideoCapture("decan.mp4") 
img = cv.imread('img/decan/decan_150.png')


edges = cv.Canny(img, 100, 200)
edges = cv.cvtColor(edges, cv.COLOR_BGR2RGB)

img_proc('./img/decan/')
# plt.imshow(edges)

plt.show()
cv.waitKey(0) 