import numpy as np
import cv2 as cv
from utils import *
from pathlib import Path
# Saving treshold data from video to .npy

def threshold(frame):
    threshold = 50
    gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
    brightness = np.mean(frame)
    ret, thresh = cv.threshold(gray, threshold, 255, cv.THRESH_BINARY)
    array_frame = np.asarray(thresh)
    white_pix = (array_frame > threshold).sum()
    return white_pix, thresh, brightness


def write_data(cap, file):
    data = []
    data_br = []
    cnt = 0
    sub_level = 0
    while cap.isOpened():
        ret, frame = cap.read()
        if ret:
            # if cnt == 100:
            #     sub_level, _, _ = threshold(frame)
            wh_px, th, br = threshold(frame)
            data.append(wh_px - sub_level)
            data_br.append(br)
            cv.imshow('frame', frame)
            cv.imshow('th', th)
            if cv.waitKey(1) == ord('q'):
                break
        else:
            break
        cnt += 1
    np.save('test_data/'+file, data)
    np.save('test_data/'+file+'_br', data_br)
    cap.release()
    cv.destroyAllWindows()


if __name__ == "__main__":
    # files = get_file_list(str(Path.cwd()) + "/C10H22_Decan_Video/h/")
    # files = get_file_list(str(Path.cwd()) + "/new_video/h/")
    # video_dir = "new_video/h/"
    # data_dir = "new_data/"
    # data = []
    # for file in files:
    #     # print(file)
    #     cap = cv.VideoCapture(video_dir+file)
    #     while cap.isOpened():
    #         ret, frame = cap.read()
    #         if ret:
    #             cv.imshow('frame', frame)
    #             data.append(threshold(frame))
    #             if cv.waitKey(1) == ord('q'):
    #                 break
    #         else:
    #             break
    #     np.save(data_dir+file, data)
    #     cap.release()
    #     cv.destroyAllWindows()
    video_dir = '!finish/'
    file = 'video.mp4'
    print(video_dir+file)
    cap = cv.VideoCapture(video_dir+file)
    write_data(cap, file)
  