import cv2 as cv
import numpy as np


filters = {
    'sharpness': np.array([[-1,-1,-1], [-1,9,-1], [-1,-1,-1]]), 
    'blur': np.ones((5,5), np.float32)/25
    }


def filter(frame):
    frame = cv.filter2D(frame, -1, filters["sharpness"])
    return frame


def gray(frame):
    frame = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
    return frame


def cornerDetector(frame):
    th_lower = 100
    th_upper = 150
    frame = cv.Canny(frame, th_lower, th_upper)
    return frame


def adjust_gamma(frame):
    gamma = 0.5
    invGamma = 1.0 / gamma
    table = np.array([((i / 255.0) ** invGamma) * 255
        for i in np.arange(0, 256)]).astype("uint8")
    return cv.LUT(frame, table)


def match(frame):
    template = np.load("point.npy")
    result = cv.matchTemplate(frame,template,5)
    return result