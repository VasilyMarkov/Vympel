import cv2 as cv
import numpy as np
import time
import socket
import json
import os


dirname = os.path.dirname(__file__)
video_file = os.path.join(dirname, './application/video.mp4')
config_file = os.path.join(dirname, './conf/config.json')

jdata = {
    'brightness': float
}

with open(config_file) as json_data:
    config_data = json.load(json_data)
    json_data.close()

cap = cv.VideoCapture(video_file)

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
host, port = config_data['network']['clientIp'], config_data['network']['clientPort']
server_address = (host, port)

while cap.isOpened():
    ret, frame = cap.read()
    mean = 0
    var = 0
    if ret:
        gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
        brightness = np.sum(gray)

        jdata['brightness'] = float(brightness)

        cv.imshow('frame', frame)
        json_data = json.dumps(jdata)
        bytes_data = json_data.encode('utf-8')
        sock.sendto(bytes_data, server_address) 	
        time.sleep(0.025)
        
    if cv.waitKey(1) == ord('q'):
        break

