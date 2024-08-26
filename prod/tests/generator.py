import numpy as np
import matplotlib.pyplot as plt
import os

def saveData(data):
    cur_dir = os.path.dirname(os.path.abspath(__file__))
    np.savetxt(f'{cur_dir}/testData', data)

def normalRandomData(mean, sigma, size):
    data = np.random.normal(mean, sigma, size)
    saveData(data)
    return data


def increaseBrightness(mean, sigma, size):
    gauss_sigma = 500
    gauss_mean = size*0.8
    random_data = np.random.normal(mean, sigma, size+1000)
    x = np.linspace(0, size, size)
    data = 10*np.exp(-0.5 * ((x - gauss_mean) / gauss_sigma)**2)

    init_data = np.zeros(size+1000)
    init_data[1000:size+1000] = data
    init_data += random_data
    init_data += 5
    saveData(init_data)
    return init_data


def decreaseBrightness(mean, sigma, size):


    first_peace = np.zeros(700)
    x = np.linspace(0, 10, 100)
    second_peace = -x
    third_peace = np.full(200, -10)

    gauss_sigma = 500
    gauss_mean = size*0.8
    x = np.linspace(0, size, size)
    fourth_peace = 10*np.exp(-0.5 * ((x - gauss_mean) / gauss_sigma)**2)-10

    curr_pos = 0 

    init_data = np.zeros(first_peace.shape[0]+second_peace.shape[0]+third_peace.shape[0]+fourth_peace.shape[0])
    curr_pos += first_peace.shape[0]
    init_data[0:curr_pos] = first_peace
    
    init_data[curr_pos:curr_pos+second_peace.shape[0]] = second_peace
    curr_pos += second_peace.shape[0]
    init_data[curr_pos:curr_pos+third_peace.shape[0]] = third_peace
    curr_pos += third_peace.shape[0]
    init_data[curr_pos:curr_pos+fourth_peace.shape[0]] = fourth_peace


    random_data = np.random.normal(mean, sigma, init_data.shape[0])

    init_data += random_data
    init_data += 5
    saveData(init_data)
    return init_data


data = decreaseBrightness(0, 1, 2000)
print(os.path.dirname(os.path.abspath(__file__)))
plt.plot(data)
plt.show()