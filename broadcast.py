"""
    Module sử dụng để gửi các gói broadcast đến tất cả các kết nối trong mạng
"""

import socket
from time import sleep
import config


def broadcast():
    # Lấy thông tin về tất cả các máy chủ của server, lấy tất cả địa chỉ của các cổng, và dùng IPv4
    interfaces = socket.getaddrinfo(host=socket.gethostname(), port=None, family=socket.AF_INET)
    allips = [ip[-1][0] for ip in interfaces]

    # Gửi thông tin về địa chỉ port của server
    msg = str(config.PORT_SERVER).encode()

    while True:
        """
        Sử dụng giao thức UDP để gửi thông tin về port và ip máy chủ đến tất cả các kết nối trong  mạng cục bộ
        """
        for ip in allips:
            # print(f'sending on {ip}')
            sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)  # UDP
            sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
            sock.bind((ip,0))
            sock.sendto(msg, ("255.255.255.255", config.PORT_BROADCAST))
            sock.close()

        sleep(2)