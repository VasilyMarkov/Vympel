import json
import matplotlib.pyplot as plt
import numpy as np
from scipy.signal import butter, filtfilt
from scipy.optimize import minimize

def polynomial(x, coeffs):
    return np.polyval(coeffs, x)

def gaussian(x, a, x0, sigma, poly_coeffs):
    x_shifted = x - x0
    polynomial_value = np.polyval(poly_coeffs, x_shifted)

    return a * np.exp((-1/2) * (polynomial_value / sigma) ** 2)


def objective_function(params):
    a = params[0]
    x0 = params[1]
    sigma = params[2]
    polyCoeffs = params[3:]
    y_pred = gaussian(x_data, a, x0, sigma, polyCoeffs)
    return np.sum((y_data - y_pred)**2)


def butter_lowpass(cutoff, fs, order=5):
    nyquist = 0.5 * fs
    normal_cutoff = cutoff / nyquist
    b, a = butter(order, normal_cutoff, btype='low', analog=False)
    return b, a

def lowpass_filter(data, cutoff, fs, order=5):
    b, a = butter_lowpass(cutoff, fs, order=order)
    y = filtfilt(b, a, data)
    return y

def normalize(data):
    data -= np.min(data)
    data /= np.max(data)
    return data

def guessCoeffs(a, mu, sigma, poly_coeffs):
    return [a, mu, sigma, poly_coeffs]


fs = 500.0  # Sampling frequency
cutoff = 7.0  # Desired cutoff frequency of the filter, Hz
order = 5  # Order of the filter

data = np.load('data/12.npy')
y_data = data
x_data = np.linspace(0, len(y_data)-1, len(y_data))
# y_data = lowpass_filter(y_data, cutoff, fs, order)
y_data = normalize(y_data)

#find mean of data
guess_shift = np.argmax(y_data)
initial_guess = [1, guess_shift, 1, 1, 1, 1, 1] #x^3+x^2+x+a

#optimize data
result = minimize(objective_function, initial_guess, method='Powell')
optimal_coeffs = result.x

a = optimal_coeffs[0]
x0 = optimal_coeffs[1]
sigma = optimal_coeffs[2]
polyCoeffs = optimal_coeffs[3:]

res_gaussian = gaussian(x_data, a, x0, sigma, polyCoeffs)

print(f"MSE: {np.sqrt(np.mean((np.array(y_data) - np.array(res_gaussian))**2))}")

fig, ax = plt.subplots(2,1)
ax[0].plot(x_data, y_data, label='Filtered data')
ax[0].plot(x_data, res_gaussian, 'r', label='Fit')
ax[1].plot(x_data, data, label='data')
ax[0].legend()
ax[1].legend()
plt.show()