import cv2 as cv
import numpy as np
from numpy import asarray
import matplotlib.pyplot as plt

cap = cv.VideoCapture("test.mp4")
cnt = 0
array_frame = []
signal = []
cnt_frame = 0

while cap.isOpened():
    ret, frame = cap.read()
    if ret:
        cnt_frame += 1
        gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
        crop = gray[100:600, 0:600]
        ret, thresh = cv.threshold(crop, 50, 255, cv.THRESH_BINARY)
        array_frame = asarray(thresh)
        white_pix = (array_frame > 50).sum()
        signal.append(white_pix)
        cv.imshow('frame', thresh)
    if cv.waitKey(1) == ord('q'):
        break
print("End")

t = np.arange(0, cnt_frame, 1)

fig, axis = plt.subplots()
axis.plot(t, signal)
plt.show()

cap.release()
cv.destroyAllWindows()
