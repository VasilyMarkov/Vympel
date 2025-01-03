import numpy as np
import cv2 as cv
import json
from pathlib import Path
import os
from os import listdir
from os.path import isfile, join

dataDir = '/data'


jdata = {
    'brightness': float
}

def get_file_list(path):
    file_list = [f for f in listdir(path) if isfile(join(path, f))]
    return file_list


def write_data(cap, cnt, fileName):
    data = []
    while cap.isOpened():
        ret, frame = cap.read()
        if ret:
            gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
            brightness = np.sum(gray)
            data.append(float(brightness))
            # cv.imshow('frame', frame)
            if cv.waitKey(1) == ord('q'):
                break
        else:
            break

    with open(f"{fileName}.json", 'w') as json_file:
        json.dump(data, json_file, indent=4)

    cap.release()
    cv.destroyAllWindows()

if __name__ == "__main__":
    parentDir = os.path.dirname(os.path.dirname(__file__))

    files = get_file_list(parentDir + "/files")
    cnt_file = 0
    for file in files:
        fileDir = parentDir+'/files/'+file
        cap = cv.VideoCapture(fileDir)
        write_data(cap, cnt_file, file)

        cnt_file += 1
