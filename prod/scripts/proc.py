import numpy as np
import cv2 as cv
import time
import os

def create_gaussian_kernel(height, width, sigma=100):
    kernel_height = cv.getGaussianKernel(height, sigma)
    kernel_width = cv.getGaussianKernel(width, sigma)
    kernel_2d = np.outer(kernel_height, kernel_width)
    return kernel_2d

def run(cap):
    while cap.isOpened():
        ret, frame = cap.read()
        if ret:
            gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
            gaussian = create_gaussian_kernel(gray.shape[0], gray.shape[1]) * 3e4
            blurred_image = cv.filter2D(gray, -1, gaussian)
            # print(gaussian)
            cv.imshow('frame', gray)
            # cv.imshow('gauss', gaussian)
            # cv.imshow('blured', blurred_image)
            if cv.waitKey(1) == ord('q'):
                break
        else:
            break
        time.sleep(0.02)
    cap.release()
    cv.destroyAllWindows()

if __name__ == "__main__":
    parentDir = os.path.dirname(os.path.dirname(__file__))

    fileDir = parentDir+'/files/DecanW-21,8HC+5_1.mp4'
    cap = cv.VideoCapture(fileDir)
    run(cap)