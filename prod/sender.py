import socket
import time

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
host, port = '192.168.31.36', 65001
server_address = (host, port)

while(True):
    sock.sendto(b'hello', server_address)
    time.sleep(0.1); 	