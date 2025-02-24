from scipy.optimize import minimize
import numpy as np
import sys
import struct
from scipy.signal import butter, filtfilt

def butter_lowpass(cutoff, fs, order=5):
    nyquist = 0.5 * fs
    normal_cutoff = cutoff / nyquist
    b, a = butter(order, normal_cutoff, btype='low', analog=False)
    return b, a


fs = 500.0  # Sampling frequency
cutoff = 20.0  # Desired cutoff frequency of the filter, Hz
order = 5  # Order of the filter

def lowpass_filter(data, cutoff, fs, order=5):
    b, a = butter_lowpass(cutoff, fs, order=order)
    y = filtfilt(b, a, data)
    return y

def genGaussianData(coeffs, sigma, x):
    func = coeffs[0]*np.exp(-0.5 * ((x - coeffs[1]) / coeffs[2])**2)
    random_data = np.random.normal(0, sigma/5, x.shape[0])
    return func + random_data

def polynomial(x, coeffs):
    return np.polyval(coeffs, x)

def gaussian(x, a, x0, sigma, poly_coeffs):
    x_shifted = x - x0
    polynomial_value = np.polyval(poly_coeffs, x_shifted)

    return a * np.exp((-1/2) * (polynomial_value / sigma) ** 2)

def normalize(data):
    data -= np.min(data)
    data /= np.max(data)
    return data


x_data = []
y_data = []

def objective_function(params):
    a = params[0]
    x0 = params[1]
    sigma = params[2]
    polyCoeffs = params[3:]
    y_pred = gaussian(x_data, a, x0, sigma, polyCoeffs)
    return np.sum((y_data - y_pred)**2)

def read_exact(n):
    data = b''
    while len(data) < n:
        chunk = sys.stdin.buffer.read(n - len(data))
        if not chunk:
            raise EOFError("Unexpected end of input")
        data += chunk
    return data

def readData():
    size_data = read_exact(4)
    size = struct.unpack('!I', size_data)[0]
    double_data = read_exact(size * 8)
    return struct.unpack(f'!{size}d', double_data)

def writeData(data):
    output_data = struct.pack('!I', len(data))  # Size
    output_data += struct.pack(f'!{len(data)}d', *data)  # Data
    sys.stdout.buffer.write(output_data)
    sys.stdout.buffer.flush()

def main():
    try:
        y_data = lowpass_filter(readData(), cutoff, fs, order)
        y_data = normalize(y_data)
        x_data = np.linspace(0, len(y_data)-1, len(y_data))
        guess_shift = np.argmax(y_data)
        initial_guess = [1, guess_shift, 1, 1, 1, 1] #x^3+x^2+x+a
        result = minimize(objective_function, initial_guess, method='Powell').x

        writeData(result)

    except Exception as e:
        sys.stderr.write(f"Error: {str(e)}")
        sys.exit(1)

if __name__ == "__main__":
    main()
# func = gaussian(result, x_data)
