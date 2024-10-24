import socket

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(('', 64000))
while True:
    msg, adr = sock.recvfrom(1024)
    print(msg)