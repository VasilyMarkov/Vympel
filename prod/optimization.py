from scipy.optimize import minimize
import numpy as np
import sys
import struct
from scipy.signal import butter, filtfilt

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

def normalize(data):
    data -= np.min(data)
    data /= np.max(data)
    return data

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
    y_data = struct.unpack(f'!{size}d', double_data)
    y_data = normalize(y_data)
    x_data = np.linspace(0, len(y_data)-1, len(y_data))
    return x_data, y_data

def writeData(data):
    output_data = struct.pack('!I', len(data))  
    output_data += struct.pack(f'!{len(data)}d', *data)  
    sys.stdout.buffer.write(output_data)
    sys.stdout.buffer.flush()


def findOptimum(x_data, y_data):
    scale = 1
    guessShift = np.argmax(y_data)
    variance = 1
    a = 1
    b = 1
    c = 1
    initCoefficients = [scale, guessShift, variance, a, b, c] #ax^2+bx+c
    resultsCoefficients = []
    stdDevList = []
    for _ in range(8):
        result = minimize(objective_function, initCoefficients, method='Powell').x
        resultsCoefficients.append(result)
        a = result[0]
        x0 = result[1]
        sigma = result[2]
        polyCoeffs = result[3:]       
        res_gaussian = gaussian(x_data, a, x0, sigma, polyCoeffs)
        stdDev = np.sqrt(np.mean((np.array(y_data) - np.array(res_gaussian))**2))
        stdDevList.append(stdDev)
        initCoefficients.append(1)

    minStdDevIndex = np.argmin(stdDevList)
    return resultsCoefficients[minStdDevIndex]

if __name__ == "__main__":
    try:
        x_data, y_data = readData()
        writeData(findOptimum(x_data, y_data))
    except Exception as e:
        sys.stderr.write(f"Error: {str(e)}")
        sys.exit(1)
