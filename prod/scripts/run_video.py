import cv2 as cv
import numpy as np
import time
import socket
import json
import os


dirname = os.path.dirname(os.path.dirname(__file__))
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
host, port = config_data['network']['clientIp'], config_data['network']['videoPort']
server_address = (host, port)

numOfFrames = cap.get(cv.CAP_PROP_FRAME_COUNT)
frame_cnt = 0
data = []
while cap.isOpened():
    
    if(frame_cnt == numOfFrames): 
        jdata['valid'] = False
        json_data = json.dumps(jdata)
        bytes_data = json_data.encode('utf-8')
        sock.sendto(bytes_data, server_address) 
        break

    ret, frame = cap.read()
    mean = 0
    var = 0

    if ret:
        gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
        brightness = np.sum(gray)
        jdata['brightness'] = float(brightness)
        jdata['valid'] = True
        data.append(float(brightness))
        cv.imshow('frame', frame)
        frame_cnt += 1

    json_data = json.dumps(jdata)
    bytes_data = json_data.encode('utf-8')
    sock.sendto(bytes_data, server_address) 

    if cv.waitKey(1) == ord('q'):
        break

with open("log", 'w') as json_file:
    json.dump(data, json_file, indent=4)


cap.release()
cv.destroyAllWindows()
