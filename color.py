import numpy as np
import matplotlib.pyplot as plt
import cv2 as cv
from algorithms import *

def showHist(frame):
    hist_h = cv.calcHist([frame],[0],None,[180],[0,180]).reshape(180)
    hist_s = cv.calcHist([frame],[1],None,[100],[0,100]).reshape(100)
    hist_v = cv.calcHist([frame],[2],None,[100],[0,100]).reshape(100)
    hist_h[0] = 0
    hist_s[0] = 0
    hist_v[0] = 0
    fig, axs = plt.subplots(3,1)
    color = ('b','g','r')
    axs[0].plot(hist_h, color = color[0])
    axs[1].plot(hist_s, color = color[1])
    axs[2].plot(hist_v, color = color[2])


def color_segmentation(frame):
    hsv = cv.cvtColor(frame, cv.COLOR_BGR2HSV)

    lower_blue = np.array([25, 30, 30]) 
    upper_blue = np.array([30, 255, 255]) 
    mask = cv.inRange(hsv, lower_blue, upper_blue) 

    result = cv.bitwise_and(frame, frame, mask = mask) 
    return result



img = cv.imread('img/decan/decan_150.png')
# img = cv.imread('fr8.png')

    # Convert the image to the LAB color space
lab_image = cv.cvtColor(img, cv.COLOR_BGR2LAB)
hsv_image = cv.cvtColor(img, cv.COLOR_BGR2HSV)

# hsv_image =  adjust_gamma(hsv_image)

l, a, b = cv.split(lab_image)
h, s, v = cv.split(hsv_image)

# h_f = filter(h)
scale = 1
delta = 0
ddepth = cv.CV_16S

grad_h_x = cv.Sobel(h, ddepth, 1, 0, ksize=3, scale=scale, delta=delta, borderType=cv.BORDER_DEFAULT)
grad_h_y = cv.Sobel(h, ddepth, 0, 1, ksize=3, scale=scale, delta=delta, borderType=cv.BORDER_DEFAULT)
 
abs_grad_x = cv.convertScaleAbs(grad_h_x)
abs_grad_y = cv.convertScaleAbs(grad_h_y)
grad = cv.addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0)
# grad_mag_norm = cv.normalize(grad_mag, None, 0, 255, cv.NORM_MINMAX, cv.CV_8U)
# mask = cv.inRange(v, 100, 255) 
# masked_grad_h = cv.bitwise_and(h, h, mask = mask) 
cr_mask = circleMask(grad.shape, 280)
masked_grad_h = cv.bitwise_and(grad, grad, mask = cr_mask) 

# result = cv.bitwise_and(masked_grad_h, masked_grad_h, mask = cr_mask)

_, th_v = cv.threshold(masked_grad_h, 100, 255, cv.THRESH_BINARY)

# result = th_v
# masked_grad_s = cv.bitwise_and(grad_s, grad_s, mask = mask)
# grad_mag = np.hypot(masked_grad_h, masked_grad_s) 

# hist_h = cv.calcHist([masked_grad_h],[0],None,[180],[0,180]).reshape(180)
# fig, axs = plt.subplots(1)
# axs.plot(hist_h)
# plt.plot(hist_h)
cv.imshow("img", img)
# cv.imshow("grad", masked_grad_h)
# cv.imshow("result", cr_mask)
cv.imshow("hm", th_v)
# cv.imshow("s", masked_grad_s)
# cv.imshow("h", grad_h)
# cv.imshow("s", grad_s)
# cv.imshow("mask", mask)

# cv.imshow('img', img)
# cv.imshow('img', gradient_l)
# showHist(hsv)
cv.waitKey(0)
plt.show()