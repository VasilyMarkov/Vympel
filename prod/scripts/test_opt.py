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


with open('gauss.json', 'r') as file:
    y_data = json.load(file)


def objective_function(params):
    a = params[0]
    x0 = params[1]
    sigma = params[2]
    polyCoeffs = params[3:]
    y_pred = gaussian(x_data, a, x0, sigma, polyCoeffs)
    return np.sum((y_data - y_pred)**2)

initial_guess = [1, 0, 1, 1, 1, 1] 

# result = minimize(objective_function, initial_guess, method='COBYQA')
# result = minimize(objective_function, initial_guess, method='Powell')
result = minimize(objective_function, initial_guess, method='Nelder-Mead')

optimal_coeffs = result.x

a = optimal_coeffs[0]
x0 = optimal_coeffs[1]
sigma = optimal_coeffs[2]
polyCoeffs = optimal_coeffs[3:]


res_gaussian = gaussian(x_data, a, x0, sigma, polyCoeffs)

print(f"Error: {np.sqrt(np.mean((np.array(y_data) - np.array(res_gaussian))**2))}")

plt.scatter(x_data, y_data, label='Data')
plt.plot(x_data, res_gaussian, 'r')
plt.xlabel('x')
plt.ylabel('y')
plt.legend()
plt.show()

print("Fitted coefficients:", optimal_coeffs)
