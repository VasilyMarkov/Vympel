import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import minimize, curve_fit
import json
from scipy.signal import butter, filtfilt

def butter_lowpass(cutoff, fs, order=5):
    nyquist = 0.5 * fs
    normal_cutoff = cutoff / nyquist
    b, a = butter(order, normal_cutoff, btype='low', analog=False)
    return b, a

def lowpass_filter(data, cutoff, fs, order=5):
    b, a = butter_lowpass(cutoff, fs, order=order)
    y = filtfilt(b, a, data)
    return y

fs = 500.0  # Sampling frequency
cutoff = 1.0  # Desired cutoff frequency of the filter, Hz
order = 5  # Order of the filter

with open('log', 'r') as file:
    data = json.load(file)

data = lowpass_filter(data, cutoff, fs, order)

data = np.array(data) / np.max(data)

data = data - np.mean(data)

def polynomial(x, coeffs):
    return np.polyval(coeffs, x)

def func(x, a, b, c, d, e, f, g):
    return np.exp(-(a*x**6 + b*x**5 + c*x**4 + d*x**3 + e*x**2 + f*x + g))

# x_data = np.linspace(-10, 10, 200)
# y_data = np.exp(-(0.1 * x_data**2 + 0.2 * x_data + 0.3)) + 0.1 * np.random.normal(size=x_data.size)
x_data = np.linspace(-len(data)/2, (len(data)-1)/2, len(data))
y_data = data


# Step 3: Define the objective function
def objective_function(coeffs, x, y):
    model = np.exp(-polynomial(x, coeffs))
    return np.sum((y - model) ** 2)


initial_guess = [1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0]


result = minimize(objective_function, initial_guess, method='Nelder-Mead', args=(x_data, y_data))

popt, pcov = curve_fit(func, x_data, y_data)
print(popt)
# Optimal coefficients
optimal_coeffs = result.x
print(optimal_coeffs)
# Step 5: Transform the data using the fitted polynomial
fitted_polynomial = polynomial(x_data, optimal_coeffs)
fitted_model = np.exp(-fitted_polynomial)
grad = np.gradient(fitted_model)
hess = np.gradient(grad)
# Plot the results
plt.scatter(x_data, y_data, label='Data')
plt.plot(x_data, fitted_model, 'r')
# plt.plot(x_data, func(x_data, *popt), 'r')
# plt.plot(x_data, hess, 'black')
plt.xlabel('x')
plt.ylabel('y')
plt.legend()
plt.show()

print("Fitted coefficients:", optimal_coeffs)