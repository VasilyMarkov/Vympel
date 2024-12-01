import socket
import json
import os


dirname = os.path.dirname(__file__)
config_file = os.path.join(dirname, './conf/config.json')

jdata = {
    'brightness': float
}

with open(config_file) as json_data:
    config_data = json.load(json_data)
    json_data.close()

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((config_data['network']['clientIp'], config_data['network']['clientPort']))
while True:
    msg, adr = sock.recvfrom(1024)
    print(msg)