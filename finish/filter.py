import numpy as np

from scipy import signal

import matplotlib.pyplot as plt

b, a = signal.iirfilter(10, 2*np.pi*50, rs=200, btype='lowpass', analog=True, ftype='butter')

w, h = signal.freqs(b, a, 1000)

fig = plt.figure()

ax = fig.add_subplot(1, 1, 1)

ax.semilogx(w / (2*np.pi), 20 * np.log10(np.maximum(abs(h), 1e-5)))

ax.set_xlabel('Frequency [Hz]')

ax.set_ylabel('Amplitude [dB]')

ax.axis((10, 1000, -100, 10))

ax.grid(which='both', axis='both')

plt.show()