import cv2 as cv
import numpy as np
import numpy.fft as fft
import matplotlib.pyplot as plt
import time
import os


def time_measure(f):
    def decorated(*args, **kwargs):
        start = time.time()
        ret = f(*args, **kwargs)
        end = time.time()
        print(f'Time execute: {(round((end-start)*1000, 3))} ms')
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



def vido_proc(cap, alg, scale = 1):
    while cap.isOpened():
        ret, frame = cap.read()
        grey = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
        # grey = grey[..., :np.min(grey.shape)]
        result = alg(grey, scale = scale)
        cv.imshow('edges', result[0])
        cv.imshow('fft', result[1])
        if cv.waitKey(1) == ord('q'):
            break  
    cv.destroyAllWindows()



def fft_img(img):
    fft_frame = fft.fft2(img)
    fft_centered = fft.fftshift(fft_frame)
    fft_normalized = np.log(1 + np.abs(fft_centered)) 
    return fft_normalized



@time_measure
def img_alg(img, gauss_gamma = 250, scale = 1):
    img = img[..., :np.min(img.shape)]
    img = cv.resize(img, (0,0), fx=scale, fy=scale) 
    edges = cv.Canny(img, 100, 300)
    gaussian_window = cv.getGaussianKernel(img.shape[0], gauss_gamma)
    gaussian_window = gaussian_window*gaussian_window.T
    windowed_edges = edges*gaussian_window
    fft = fft_img(windowed_edges)
    return edges, fft



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
img = cv.imread('img/decan/decan_050.png')
img = cv.cvtColor(img, cv.COLOR_BGR2GRAY) #3-channel img to 1-channel img
# plt.imshow(img, cmap = 'gray')
vido_proc(cap, img_alg)

def matching(img = None):   
    img = np.pad(img, pad_width=8, mode='constant', constant_values=0) 
    print(img.shape)
    pattern = np.ones([3,3])
    pattern = np.pad(pattern, pad_width=3, mode='constant', constant_values=0)
    pat1 = np.ones([8,8])
    a = img.shape[0]//2-pattern.shape[0]//2
    b = img.shape[0]//2+pattern.shape[0]//2
    steps = (img.shape[0]//2)//pattern.shape[0]
    picks = np.zeros(steps)
    # print(pat1.shape, steps)
    for i in range(0, (img.shape[0]//2)-1, pat1.shape[0]):
        # horizontal_window = img[a+step:b+step, a:b] * pattern
        # vertical_window = img[a:b, a+step:b+step] * pattern
        # print(img[a:b, a+i:b+i].shape, i)
        # print(a+i, b+i)
        # img[a:b, a+i:b+i] = pat1
        picks[i] = np.average(img[a:b, a+i:b+i], weights=pattern)
    return picks
        

# plt.imshow(pattern, cmap = 'gray')
img = img[..., :np.min(img.shape)]
# print(img.shape)
matching(img)
plt.show()
cv.waitKey(0) 