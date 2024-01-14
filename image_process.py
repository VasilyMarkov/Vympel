import cv2
import matplotlib.pyplot as plt
import numpy as np
n = 1000
t = np.linspace(-3, 3, n)
x = np.cos(30 * t) + np.sin(10 * t) + 0.3 * (t ** 2)

image = cv2.imread("fig3.png", 0)
