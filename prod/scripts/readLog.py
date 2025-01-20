import json
import matplotlib.pyplot as plt
import numpy as np
from scipy.signal import butter, filtfilt
from scipy.optimize import minimize
from scipy.signal import savgol_filter


def signaltonoise_dB(a, axis=0, ddof=0):
    a = np.asanyarray(a)
    m = a.mean(axis)
    sd = a.std(axis=axis, ddof=ddof)
    return 20*np.log10(abs(np.where(sd == 0, 0, m/sd)))

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


def mowingAverage(data, windowSize = 3): 

    std = np.std(np.gradient(data[10:50]))
    print(np.gradient(data[10:50]))
    th = 5*std
    print(th)

    np.mean(data)
    result = np.zeros(data.shape[0])
    for i in range(data.shape[0] - windowSize):
        if(np.mean(np.gradient(data[i:i+windowSize])) > th):
            result[i] = 1
    
    return result


def bars(data):
    result = np.zeros(int(data.shape[0]/10))
    j = 0
    var = np.var(data[0:10])
    th = 0.05
    for i in range(0, len(data), 10):
        if(i > 0):
            window = data[i:i+10]
            prev_mean = np.mean(data[i-1:i-1+10])    
            mean = np.mean(window)
            v = mean - prev_mean
            b = (mean - prev_mean / var)
            sum = 0
            for k in window:
                sum += (k - prev_mean - v / 2)
            result[j] = (b/var)*sum >= 0.3*1e7
            # result[j] = (b/var)*sum 
            j += 1
    return result


fs = 500.0  # Sampling frequency
cutoff = 5.0  # Desired cutoff frequency of the filter, Hz
order = 5  # Order of the filter

def corr(data, ker):
    return np.correlate(data, ker, mode='valid')


with open('data/video.mp4.json', 'r') as file:
    data = json.load(file)

y_data = data

# y_data = y_data[::1]
x_data = np.linspace(0, len(y_data)-1, len(y_data))
y_data -= np.min(y_data)
y_data /= np.max(y_data)
filtered = lowpass_filter(y_data, cutoff, fs, order)


window_size = 20
poly_order = 10
smoothed = savgol_filter(filtered, window_size, poly_order)

snr=signaltonoise_dB(y_data[0:500], axis=0, ddof=0) 
fsnr=signaltonoise_dB(filtered[0:500], axis=0, ddof=0) 
bars = bars(y_data)

guess_shift = np.argmax(y_data)

initial_guess = [1, guess_shift, 1, 1, 1, 1, 1] #x^3+x^2+x+a

# result = minimize(objective_function, initial_guess, method='Nelder-Mead')
result = minimize(objective_function, initial_guess, method='Powell')
optimal_coeffs = result.x

a = optimal_coeffs[0]
x0 = optimal_coeffs[1]
sigma = optimal_coeffs[2]
polyCoeffs = optimal_coeffs[3:]

res_gaussian = gaussian(x_data, a, x0, sigma, polyCoeffs)
grad = np.gradient(filtered)
grad = lowpass_filter(grad, 1, fs, order)
ref = y_data[:10]

corr_res = corr(y_data, ref)

print(f"Min: {np.argmin(grad)}")
print(f"MSE: {np.sqrt(np.mean((np.array(y_data) - np.array(res_gaussian))**2))}")

# np.save('data/12', y_data[50:510])

mw = mowingAverage(filtered, 10)

fig, ax = plt.subplots(4, 1)
ax[0].plot(x_data, y_data, label='Data')
ax[1].plot(x_data, filtered, label='Data')
ax[2].plot(x_data, grad, label='Data')
ax[3].plot(x_data, mw, label='Data')
# ax[0].plot(x_data[50:510], y_data[50:510])
# ax[1].plot(bars, 'r')

plt.legend()
plt.show()

