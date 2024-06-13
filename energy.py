import cv2 as cv
import numpy as np
import matplotlib.pyplot as plt
import time

def energy(frame):
    frame_f = frame.astype(float)
    frame_f = np.power(frame_f, 2)
    return frame

def kmeans_color_quantization(image, clusters=8, rounds=1):
    h, w = image.shape[:2]
    samples = np.zeros([h*w,3], dtype=np.float32)
    count = 0

    for x in range(h):
        for y in range(w):
            samples[count] = image[x][y]
            count += 1

    compactness, labels, centers = cv.kmeans(samples,
            clusters, 
            None,
            (cv.TERM_CRITERIA_EPS + cv.TERM_CRITERIA_MAX_ITER, 10000, 0.0001), 
            rounds, 
            cv.KMEANS_RANDOM_CENTERS)

    centers = np.uint8(centers)
    res = centers[labels.flatten()]
    return res.reshape((image.shape))

img2 = cv.imread('img/fig3.png')

# img2 = img2[50:700,50:700]
res = img2.copy()
start = time.time()
kmeans = kmeans_color_quantization(res, clusters = 2)
gray = cv.cvtColor(kmeans, cv.COLOR_BGR2GRAY)
thresh = cv.threshold(gray, 100, 255, cv.THRESH_BINARY + cv.THRESH_OTSU)[1]

points_list = []
size_list = []
cnts, _ = cv.findContours(thresh, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)[-2:]
AREA_THRESHOLD = 2
for c in cnts:
    area = cv.contourArea(c)
    if area < AREA_THRESHOLD:
        cv.drawContours(thresh, [c], -1, 0, -1)
    else:
        (x, y), radius = cv.minEnclosingCircle(c)
        points_list.append((int(x), int(y)))
        size_list.append(area)

# Apply mask onto original image
result = cv.bitwise_and(res, res, mask=thresh)
result[thresh==255] = (36,255,12)
end = time.time()
print(end-start)
cv.imshow('kmeans', kmeans)
cv.imshow('original', res)
cv.imshow('thresh', thresh)
# cv.imshow('result', result)
cv.waitKey(0) 
# cap = cv.VideoCapture(file)
# while cap.isOpened():
#     ret, frame = cap.read()
#     gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
#     en = energy(frame)
#     if ret:
#         cv.imshow('frame', gray)
#         # time.sleep(0.01)
#     if cv.waitKey(1) == ord('q'):
#         break
# cap.release()
# cv.destroyAllWindows()

# cv.waitKey(0) 