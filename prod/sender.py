import socket
import time

def send_data(host='localhost', port=63000, message='Hello, Server!'):
    # Create a socket object
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    try:
        # Connect to the server
        client_socket.connect((host, port))
        print(f"Connected to {host}:{port}")

        # Send data to the server
        client_socket.sendall(message.encode('utf-8'))
        print(f"Sent message: {message}")

    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    while(True):
        send_data()
        time.sleep(0.2)