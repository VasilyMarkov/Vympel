import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import minimize
import json 

def gaussian(x, a, x0, sigma, poly_coeffs):
    x_shifted = x - x0
    polynomial_value = np.polyval(poly_coeffs, x_shifted)

    return a * np.exp((-1/2) * (polynomial_value / sigma) ** 2)

def polynomial(x, coeffs):
    return np.polyval(coeffs, x)

x_data = np.linspace(-10, 10, 200)

poly_coeffs = [1, 0, -1]
a = 1
x0 = 0
sigma = 2


y_data = gaussian(x_data, a, x0, sigma, poly_coeffs) + 0.05 * np.random.normal(size=x_data.size)
print(type(y_data))

file_name = 'gauss.json'

with open(file_name, 'w') as json_file:
    json.dump(y_data.tolist(), json_file, indent=4)