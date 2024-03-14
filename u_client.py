#!/usr/bin/python
import pickle, socket, subprocess, re, time
import numpy as np
import cv2 as cv
import collections 

def threshold(cap):
    cnt_frame = 0
    threshold = 50
    buffer = collections.deque(maxlen=20)
    while cap.isOpened():
        ret, frame = cap.read()
        if ret:
            cnt_frame += 1
            gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
            crop = gray[100:600, 0:600]
            ret, thresh = cv.threshold(crop, threshold, 255, cv.THRESH_BINARY)
            array_frame = np.asarray(thresh)
            white_pix = (array_frame > threshold).sum()
            buffer.append(white_pix)
            avg = np.mean(np.array(buffer)).astype(int)
            msg = (avg, True)
            sendData(msg)
            cv.imshow('frame', thresh)
        if cv.waitKey(1) == ord('q'):
            msg = (avg, False)
            sendData(msg)
            break


def sendData(data):
    ip_str = subprocess.run(['hostname', '-I'], stdout=subprocess.PIPE)
    adr = re.findall( r'[0-9]+(?:\.[0-9]+){3}', str(ip_str.stdout))

    ip = {"own": "127.0.0.1", "hostname": str(adr)}

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    server_address = (ip['own'], 65000)

    data_string = pickle.dumps(data)
    sock.sendto(data_string, server_address) 


if __name__ == "__main__":
    cap = cv.VideoCapture("new_video/h/hc-20w-40(octan)6.avi")
    threshold(cap)