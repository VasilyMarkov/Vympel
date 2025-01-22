import numpy as np
import matplotlib.pyplot as plt

# Generate some sample data
np.random.seed(0)
data = np.random.normal(0, 1, 100)  # Normal noise
data[50:] += np.linspace(0, 5, 50)  # Add a growing trend starting at index 50

# CUSUM algorithm
def cusum(data, threshold=1.0):
    cusum = np.zeros_like(data)
    cusum[0] = data[0]
    for i in range(1, len(data)):
        cusum[i] = cusum[i-1] + data[i]
        if cusum[i] < 0:
            cusum[i] = 0
    change_points = np.where(cusum > threshold)[0]
    return change_points, cusum

threshold = 1.0
change_points, cusum_values = cusum(data, threshold)

# Plot the results
plt.figure(figsize=(12, 6))
plt.subplot(2, 1, 1)
plt.plot(data, label='Data')
plt.scatter(change_points, data[change_points], color='red', label='Change Points')
plt.legend()
plt.title('Data with Detected Change Points')

plt.subplot(2, 1, 2)
plt.plot(cusum_values, label='CUSUM')
plt.legend()
plt.title('CUSUM Values')

plt.tight_layout()
plt.show()