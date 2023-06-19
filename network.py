import threading
import time
import speech

class MultiSocket(threading.Thread):
    def __init__(self, client_socket, client_address):
        super().__init__()
        self.client_socket = client_socket
        self.client_address = client_address

class SendESP32(MultiSocket):
    def __init__(self, client_socket, client_address):
        super().__init__(client_socket, client_address)
        self.old_status = -1
        self.new_status = -1
    def run(self):
        while True:
            if self.old_status != self.new_status:
                self.client_socket.send(str(self.new_status).encode())
                self.old_status = self.new_status
                time.sleep(2)
    def update(self, status):
        self.new_status = status

class Speech(MultiSocket):
    def __init__(self, client_socket, client_address):
        super().__init__(client_socket=client_socket, client_address=client_address)
        # self.audio = None
        # self.audio_path = client_address + ".wav"
        # self.complete = 0
    def run(self):
        pass
        # while True:
        #     if complete == 0:
        #         with open(self.audio_path, "wb") as file:
        #             while True:
        #                 data = self.client_socket.recv(1024)
        #                 if not data:
        #                     break
        #                 file.write(data)
        #         complete = 1
        #     time.sleep(2)
        

        