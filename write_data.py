import numpy as np
import cv2 as cv
from utils import *
from pathlib import Path

def threshold(frame):
    threshold = 50
    gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
    crop = gray[100:600, 0:600]
    ret, thresh = cv.threshold(crop, threshold, 255, cv.THRESH_BINARY)
    array_frame = np.asarray(thresh)
    white_pix = (array_frame > threshold).sum()
    return white_pix


if __name__ == "__main__":
    
    files = get_file_list(str(Path.cwd()) + "/C10H22_Decan_Video/h/")
    video_dir = "C10H22_Decan_Video/h/"
    data_dir = "data/old_video_data/"
    data = []
    for file in files:
        # print(file)
        cap = cv.VideoCapture(video_dir+file)
        while cap.isOpened():
            ret, frame = cap.read()
            if ret:
                cv.imshow('frame', frame)
                data.append(threshold(frame))
                if cv.waitKey(1) == ord('q'):
                    break
            else:
                break
        np.save(data_dir+file, data)
        cap.release()
        cv.destroyAllWindows()
  