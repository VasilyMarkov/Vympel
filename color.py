import numpy as np
import matplotlib.pyplot as plt
import cv2 as cv

 
img = cv.imread('img/decan/decan_150.png')
edges = cv.Canny(img, 100, 150)
# r_img[:, :, 2] = 0
cv.imshow("r", edges)
color = ('b','g','r')


histr = np.zeros([256, 3])
histr[:, 0] = cv.calcHist([img],[0],None,[256],[0,256]).reshape(256)
histr[:, 1] = cv.calcHist([img],[1],None,[256],[0,256]).reshape(256)
histr[:, 2] = cv.calcHist([img],[2],None,[256],[0,256]).reshape(256)
histr_sum = np.sum(histr)
histr[:, 0] = histr[:, 0]/histr_sum 
histr[:, 1] = histr[:, 1]/histr_sum 
histr[:, 2] = histr[:, 2]/histr_sum 
fig, axs = plt.subplots(3,1)

axs[0].plot(histr[:, 0],color = color[0])
axs[1].plot(histr[:, 1],color = color[1])
axs[2].plot(histr[:, 2],color = color[2])
# axs[0].xlim([0,256])

# for i,col in enumerate(color):
#     histr = cv.calcHist([img],[i],None,[256],[0,256])
#     cut_histr = histr[50:]
#     plt.plot(cut_histr,color = col)
#     plt.xlim([0,256])
plt.show()