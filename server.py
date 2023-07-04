"""
    Module server

    - Khởi tạo các list chứa các thiết bị đẵ được đăng kí (devices, micro)
    - Luồng xử lý các dữ liệu đến và đưa ra các quyết định gửi đi

"""
import socket
import config
from network import Speech, SendESP32
import threading
import speech

# Đọc IP server
HOST = socket.gethostbyname(socket.gethostname())

# Tạo các list chứa các thiết bị đã được đăng ký
devices = {"light" : []}
micro = []

"""
    Tạo luồng nhận dữ liệu từ micro, mỗi micro sử dụng một luồng
    
    - Mỗi luồng micro khi nhận được dữ liệu, sẽ nhận dạng rồi đưa ra quyết định
"""
class Receive(threading.Thread):
    def __init__(self, client_socket, client_address):
        super().__init__()
        
        # Gán các thông số về socket cho luồng
        self.client_socket = client_socket
        self.client_address = client_address

        # Tạo ra luồng nhận dữ liệu từ micro
        self.receive = Speech(client_socket, client_address)
        self.receive.start()
    def run(self):
        pass
        # while True:
        #     # if self.receive.complete == 1:
        #     if True:
        #         # self.audio_path = self.receive.audio_path
        #         # device, text = speech.speech(self.audio_path)
        #         device, text = speech.speech()
        #         # self.receive.complete = 0
        #         n = len(devices["light"])
        #         device = int(device)
        #         if device >= 0 and device < n:
        #             if text == '1' or text == '0':
        #                 devices['light'][device].update(text)
        #             else:
                        # print("Khong co trang thai nay")

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
        if data == "device":
            t = SendESP32(client_socket, client_address)
            t.start()
            devices["light"].append(t)
        elif data == "micro":
            r = Receive(client_socket, client_address)
            r.start()
            micro.append(r)