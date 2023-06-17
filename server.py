"""
    Module server

    - Khởi tạo server 
    - gửi gói broadcast đến tất cả kết nối về thông tin của server trong mạng 
    - Mở kết nối TCP để giao tiếp với ESP32
"""

import socket
import speech
import config


# Đọc IP server
HOST = socket.gethostbyname(socket.gethostname())

def start_server():
    # Tạo socket và liên kết nó với địa chỉ và cổng
    # Tạo socket server
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((HOST, config.PORT_SERVER))
    server_socket.listen(1)

    # Mở cổng chờ các kết nối socket tới server
    print(f"Server is listening on {HOST}:{config.PORT_SERVER}")
    client_socket, client_address = server_socket.accept()
    print(f"Connected with {client_address}")

    """
        Chờ dữ liệu gửi tới server có tối đa 1024 byte
        decode() giải mã dữ liệu dưới dạng byte sang chuỗi sử dụng encoding
    """
    data = client_socket.recv(1024).decode()
    print(f"Received data: {data}")


    while True:

        """
            Sử dụng module speech để call api và nhận diện giọng nói
        """
        _audio = speech.speech()
        """
                Nếu bật bóng đèn Rơ-le hoạt động ở mức thấp nên đầu tiên mã hóa dưới dạng byte rồi truyền bit thấp về ESP32
                Nếu tắt bóng đèn Rơ-le hoạt động ở mức cao nên mã hóa rồi gửi về ESP32
        """
        if _audio == "light_on":
            msg = "0"
            client_socket.send(msg.encode())
        elif _audio == "light_off":
            msg = "1"
            client_socket.send(msg.encode())
        
# if __name__ == "__main__":
#     start_server()
