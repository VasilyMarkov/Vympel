import cv2 as cv
import numpy as np
import time

def time_measure(f):
    def decorated(*args, **kwargs):
        start = time.time()
        ret = f(*args, **kwargs)
        end = time.time()
        print(f'Time execute: {(round((end-start)*1000, 3))} ms')
        return ret 
    return decorated


filters = {
    'sharpness': np.array([[-1,-1,-1], [-1,9,-1], [-1,-1,-1]]), 
    'blur': np.ones((5,5), np.float32)/25,
    'laplace': np.array([[1,1,1], [1,8,1], [1,1,1]])
    }


def filter(frame):
    return cv.filter2D(frame, -1, filters["blur"])

def filter_wrapper(frame, filter):
    return cv.filter2D(frame, -1, filter) 

def toHSV(frame):
    result = cv.cvtColor(frame, cv.COLOR_BGR2HSV)
    h = result[..., 2]
    return h

def gray(frame):
    return cv.cvtColor(frame, cv.COLOR_BGR2GRAY)

def sobel(frame):
    result = cv.Sobel(frame, cv.CV_64F, 1, 1, ksize=3)
    return result


def cornerDetector(frame):
    th_lower = 75
    th_upper = 150
    result = cv.Canny(frame, th_lower, th_upper)
    return result


def adjust_gamma(frame):
    gamma = 0.65
    invGamma = 1.0 / gamma
    table = np.array([((i / 255.0) ** invGamma) * 255
        for i in np.arange(0, 256)]).astype("uint8")
    return cv.LUT(frame, table)

# not working
def match(frame):
    template = np.load("point.npy")
    return cv.matchTemplate(frame,template,5)


def detect_black_points(frame):
    _, binary_image = cv.threshold(frame, 125, 255, cv.THRESH_BINARY_INV)
    contours, hierarchy = cv.findContours(binary_image, cv.RETR_TREE, cv.CHAIN_APPROX_SIMPLE)
    cv.drawContours(frame, contours, -1, (0, 0, 255), 2)
    return frame

def color_segmentation(frame):
    hsv = cv.cvtColor(frame, cv.COLOR_BGR2HSV)

    lower_blue = np.array([18, 190, 200]) 
    upper_blue = np.array([250, 255, 255]) 
    mask = cv.inRange(hsv, lower_blue, upper_blue) 
      
    result = cv.bitwise_and(frame, frame, mask = mask) 
    return mask


@time_measure
def detect_color_gradient(frame):
    hsv_image = cv.cvtColor(frame, cv.COLOR_BGR2HSV)

    h, s, v = cv.split(hsv_image)

    scale = 1
    delta = 0
    ddepth = cv.CV_16S

    grad_h_x = cv.Sobel(h, ddepth, 1, 0, ksize=3, scale=scale, delta=delta, borderType=cv.BORDER_DEFAULT)
    grad_h_y = cv.Sobel(h, ddepth, 0, 1, ksize=3, scale=scale, delta=delta, borderType=cv.BORDER_DEFAULT)
    
    abs_grad_x = cv.convertScaleAbs(grad_h_x)
    abs_grad_y = cv.convertScaleAbs(grad_h_y)
    grad = cv.addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0)

    cr_mask = circleMask(grad.shape, 280)
    masked_grad_h = cv.bitwise_and(grad, grad, mask = cr_mask) 

    _, th_v = cv.threshold(masked_grad_h, 100, 255, cv.THRESH_BINARY)

    return th_v


def detect_black(frame):
    h, s, v = cv.split(frame)
    mask = cv.inRange(v, 50, 255) 
    # th = 100
    # _, th_v = cv.threshold(v, th, 255, cv.THRESH_BINARY)
    # proc_frame = cv.merge([h, s, th_v])
    # h, s, v = cv.split(proc_frame)
    scale = 1
    delta = 0
    ddepth = cv.CV_16S
    grad_v_x = cv.Sobel(v, ddepth, 1, 0, ksize=3, scale=scale, delta=delta, borderType=cv.BORDER_DEFAULT)
    grad_v_y = cv.Sobel(v, ddepth, 0, 1, ksize=3, scale=scale, delta=delta, borderType=cv.BORDER_DEFAULT)
    
    abs_grad_x = cv.convertScaleAbs(grad_v_x)
    abs_grad_y = cv.convertScaleAbs(grad_v_y)
    grad = cv.addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0)
    return grad

def circleMask(frame_size, circle_size):
    row, col = frame_size
    test = np.ones([row, col])

    for y in range(row):
        for x in range(col):
            if np.sqrt((x-col/2)**2 + (y-row/2)**2) > circle_size:
                test[y][x] = 0
    return test.astype(np.uint8)

@time_measure
def threshold(frame):
    threshold = 20
    gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
    crop = gray[100:600, 0:600]
    ret, thresh = cv.threshold(crop, threshold, 255, cv.THRESH_BINARY)
    array_frame = np.asarray(thresh)
    # white_pix = (array_frame > threshold).sum()
    return thresh
# img = np.ones([11, 11]).astype(np.uint8)
# masked = np.zeros([11, 11])
# mask = circleMask(img.shape, 3).astype(np.uint8)
# result = cv.bitwise_and(img, img, mask = mask)
# print(mask)
# print("-------------------------------")
# print(result)
