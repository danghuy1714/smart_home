"""
    Module server

    - Khởi tạo các list chứa các thiết bị đẵ được đăng kí (devices, micro)
    - Luồng xử lý các dữ liệu đến và đưa ra các quyết định gửi đi

"""
import socket
import config
from network import  SendESP32, Speech
import network

# Đọc IP server
HOST = socket.gethostbyname(socket.gethostname())


def start_server():
    # Tạo socket và liên kết nó với địa chỉ và cổng
    # Tạo socket server
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((HOST, config.PORT_SERVER))
    server_socket.listen(100)

    # Mở cổng chờ các kết nối socket tới server
    while True:
        print(f"Server is listening on {HOST}:{config.PORT_SERVER}")
        client_socket, client_address = server_socket.accept()
        print(f"Connected with {client_address}")
        data = client_socket.recv(1024).decode()
        print(f"Received data: {data}")

        if data == "micro":
            r = Speech(client_socket, client_address, data)
            r.start()
            network.micro.append(r)
        else:
            t = SendESP32(client_socket, client_address)
            network.devices["light"].append(t)