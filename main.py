import cv2
from matplotlib import pyplot as plt

images = [cv2.cvtColor(cv2.imread('frame1.jpg'), cv2.COLOR_BGR2GRAY), cv2.cvtColor(cv2.imread('frame2.jpg'), cv2.COLOR_BGR2GRAY),
          cv2.cvtColor(cv2.imread('frame3.jpg'), cv2.COLOR_BGR2GRAY), cv2.cvtColor(cv2.imread('frame4.jpg'), cv2.COLOR_BGR2GRAY),
          cv2.cvtColor(cv2.imread('frame5.jpg'), cv2.COLOR_BGR2GRAY), cv2.cvtColor(cv2.imread('frame6.jpg'), cv2.COLOR_BGR2GRAY)]
color = ('b', 'g', 'r')
hists = []
hists = [cv2.calcHist([images[0]], [0], None, [256], [0, 256]), cv2.calcHist([images[1]], [0], None, [256], [0, 256]),
         cv2.calcHist([images[2]], [0], None, [256], [0, 256]), cv2.calcHist([images[3]], [0], None, [256], [0, 256]),
         cv2.calcHist([images[4]], [0], None, [256], [0, 256]), cv2.calcHist([images[5]], [0], None, [256], [0, 256])]

plt.subplot(6, 1, 1)
plt.plot(hists[0], color=None)
plt.subplot(6, 1, 2)
plt.plot(hists[1], color=None)
plt.subplot(6, 1, 3)
plt.plot(hists[2], color=None)
plt.subplot(6, 1, 4)
plt.plot(hists[3], color=None)
plt.subplot(6, 1, 5)
plt.plot(hists[4], color=None)
plt.subplot(6, 1, 6)
plt.plot(hists[5], color=None)

crop = images[3][100:600, 0:600]
crops = []
for i in range(len(images)):
    crops.append(images[i][100:600, 0:600])

ret1, thresh1 = cv2.threshold(crops[0], 75, 255, cv2.THRESH_BINARY)
ret2, thresh2 = cv2.threshold(crops[1], 75, 255, cv2.THRESH_BINARY)
ret3, thresh3 = cv2.threshold(crops[2], 75, 255, cv2.THRESH_BINARY)
ret4, thresh4 = cv2.threshold(crops[3], 75, 255, cv2.THRESH_BINARY)
ret5, thresh5 = cv2.threshold(crops[4], 75, 255, cv2.THRESH_BINARY)
ret6, thresh6 = cv2.threshold(crops[5], 75, 255, cv2.THRESH_BINARY)

cv2.imshow('frame1', thresh1)
cv2.imshow('frame2', thresh2)
cv2.imshow('frame3', thresh3)
cv2.imshow('frame4', thresh4)
cv2.imshow('frame5', thresh5)
cv2.imshow('frame6', thresh6)

print(cv2.countNonZero(thresh1), cv2.countNonZero(thresh2),
      cv2.countNonZero(thresh3), cv2.countNonZero(thresh4),
      cv2.countNonZero(thresh5), cv2.countNonZero(thresh6))
# cv2.imshow('frame1', thresh1)
# plt.show()
cv2.waitKey(0)

