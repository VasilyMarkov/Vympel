import cv2 as cv
import numpy as np
import numpy.fft as fft
import matplotlib.pyplot as plt
import time
import sys
import os


def time_measure(f):
    def decorated(*args, **kwargs):
        start = time.time()
        ret = f(*args, **kwargs)
        end = time.time()
        print(f'Time execute: {(round(end-start, 5)*1000)} ms')
        return ret 
    return decorated



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



def vido_proc(cap, alg):
    while cap.isOpened():
        ret, frame = cap.read()
        grey = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
        grey = grey[..., :np.min(grey.shape)]
        cv.imshow('img', grey)
        cv.imshow('alg', (alg(frame)))
        if cv.waitKey(1) == ord('q'):
            break    



def fft_img(img):
    fft_frame = fft.fft2(img)
    fft_centered = fft.fftshift(fft_frame)
    fft_normalized = np.log(1 + np.abs(fft_centered)) 
    return fft_normalized



@time_measure
def img_alg(img, gauss_gamma = 250):
    img = cv.cvtColor(img, cv.COLOR_BGR2GRAY) #3-channel img to 1-channel img
    img = img[..., :np.min(img.shape)]
    edges = cv.Canny(img, 100, 200)
    gaussian_window = cv.getGaussianKernel(img.shape[0], gauss_gamma)
    gaussian_window = gaussian_window*gaussian_window.T
    windowed_edges = edges*gaussian_window
    fft = fft_img(windowed_edges)
    return fft



def images_proc(path):
    files = [f for f in os.listdir(path) if f.endswith(".png")]
    fig, axs = plt.subplots(1,len(files))
    files = sorted(files)
    for i in range(len(files)):
        img = cv.imread(f'{path}{files[i]}')
        axs[i].imshow(img_alg(img), cmap = 'gray')
        axs[i].axis('off')
        axs[i].set_title(files[i])



cap = cv.VideoCapture("decan.mp4") 
img = cv.imread('img/decan/decan_50.png')


vido_proc(cap, img_alg)

# images_proc('./img/decan/')
# plt.imshow(img_alg(img), cmap = 'gray')
plt.show()
cv.waitKey(0) 