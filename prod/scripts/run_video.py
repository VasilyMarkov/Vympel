import cv2 as cv
import numpy as np
import time
import socket
import json
import os
import sys
import ipaddress
from picamera2 import Picamera2


dirname = os.path.dirname(os.path.dirname(__file__))
video_file = os.path.join(dirname, './application/video.mp4')
config_file = os.path.join(dirname, './conf/config.json')

jdata = {
    'brightness': float
}

with open(config_file) as json_data:
    config_data = json.load(json_data)
    json_data.close()

picam2 = Picamera2()
picam2.configure(picam2.create_preview_configuration(main={"format": 'XRGB8888', "size": (640, 480)}))
picam2.start()


sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
video_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
host, port = config_data['network']['clientIp'], config_data['network']['videoPort']
server_address = (host, port)

video_sock_ip = sys.argv[1:]
ipv4_address = ipaddress.ip_address(video_sock_ip[0]).ipv4_mapped   
service_port = 12345
service_address = (str(ipv4_address), service_port)

frame_cnt = 0
data = []

encode_params = [cv.IMWRITE_JPEG_QUALITY, 70]  

if __name__ == "__main__":
    while True:
        frame = picam2.capture_array()
        _, buffer = cv.imencode('.jpg', frame, encode_params)
        video_sock.sendto(buffer.tobytes(), service_address)

        mean = 0
        var = 0

        gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
        brightness = np.sum(gray)
        jdata['brightness'] = float(brightness)
        jdata['valid'] = True

        frame_cnt += 1

        json_data = json.dumps(jdata)
        bytes_data = json_data.encode('utf-8')
        sock.sendto(bytes_data, server_address) 
    cap.release()
    cv.destroyAllWindows()
