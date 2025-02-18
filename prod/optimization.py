from scipy.optimize import minimize
import numpy as np
import sys
import struct

def genGaussianData(coeffs, sigma, x):
    func = coeffs[0]*np.exp(-0.5 * ((x - coeffs[1]) / coeffs[2])**2)
    random_data = np.random.normal(0, sigma/5, x.shape[0])
    return func + random_data

def gaussian(coeffs, x):
    a = coeffs[0]
    mean = coeffs[1]
    sigma = coeffs[2]
    return a*np.exp(-0.5 * ((x - mean) / sigma)**2)


# begin = 0
# end = 10
# size = 100

# x_data = np.linspace(begin, end, size)

# y_data = genGaussianData(coeffs = [1,5,1], sigma = 0.2, x=x_data)


# def objective_function(params):
#     y_pred = gaussian(params, x_data)
#     return np.sum((y_data - y_pred)**2)

# initial_guess = [1, 2, 1] 

# result = minimize(objective_function, initial_guess, method='Powell').x

def read_exact(n):
    data = b''
    while len(data) < n:
        chunk = sys.stdin.buffer.read(n - len(data))
        if not chunk:
            raise EOFError("Unexpected end of input")
        data += chunk
    return data

def main():
    try:
        # Read vector size (4-byte unsigned integer, big-endian)
        size_data = read_exact(4)
        size = struct.unpack('!I', size_data)[0]

        # Read vector data (size * 8 bytes for doubles)
        double_data = read_exact(size * 8)
        input_vector = struct.unpack(f'!{size}d', double_data)

        # Process data (example: square each value)
        result = [x ** 2 for x in input_vector]
        # print(result)
        # Serialize result
        output_data = struct.pack('!I', len(result))  # Size
        output_data += struct.pack(f'!{len(result)}d', *result)  # Data
        
        # Write to stdout
        sys.stdout.buffer.write(output_data)
        sys.stdout.buffer.flush()

    except Exception as e:
        sys.stderr.write(f"Error: {str(e)}")
        sys.exit(1)

if __name__ == "__main__":
    main()
# func = gaussian(result, x_data)
