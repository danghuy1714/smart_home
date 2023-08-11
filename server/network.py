"""
    Module network có chức năng tạo các luồng đi và đến server
    SendESP32 là luồng gửi dữ liệu đến thiết bị
"""

import threading
import wave
import os
import speech

# Lớp cha chứa thông tin thề socket
class MultiSocket():
    def __init__(self, client_socket, client_address):
        super().__init__()
        self.client_socket = client_socket
        self.client_address = client_address


# Kết thừa từ lớp multisocket có chức năng gửi data đến esp32(tại esp32 điều khiến)
class SendESP32(MultiSocket):
    def __init__(self, client_socket, client_address):
        super().__init__(client_socket, client_address)


    def send(self, new_status):

        """
            Kiểm tra trạng thái và gửi data đến esp32 đã đăng ký
        """
        self.client_socket.send(str(new_status).encode())

# Tạo các list chứa các thiết bị đã được đăng ký
devices = {"light" : []}
micro = []

"""
    Luồng speech đăng ký là các thiết bị micro để nhận giọng nói,
    xử lý và trả về các thông tin đã nhận dạng
"""  
class Speech(MultiSocket, threading.Thread):
    def __init__(self, client_socket, client_address, name):
        super().__init__(client_socket=client_socket, client_address=client_address)
        self.audio_path = name + ".wav"
    def run(self):
        while True:
            # data = self.client_socket.recv(1024)
            data = self.client_socket.recv(65536)

            if data:
                frames = 0
                try:
                    with wave.open(self.audio_path, 'rb') as wav_file:
                        # Đọc dữ liệu âm thanh hiện có
                        frames = wav_file.readframes(wav_file.getnframes())
                        wav_file.close()
                except:
                    print(f"Tao file: {self.audio_path}")

                try:
                    if os.path.getsize(self.audio_path) > 300000:
                        frames = 0
                        control = speech.speech(self.audio_path)
                        self.audio = control
                        if "bật tình yêu lên" in  self.audio.lower():
                            for i in range(10000):
                                devices["light"][0].send(1)
                        elif "tắt đèn đi" in self.audio.lower():
                            for i in range(10000):
                                devices["light"][0].send(0)
                        else:
                            devices["light"][0].send(5)

                except:
                    pass
                # Mở tệp âm thanh ở chế độ ghi (viết lại toàn bộ dữ liệu)
                with wave.open(self.audio_path, 'wb') as wav_file:
                    # Cấu hình thông số của tệp âm thanh
                    wav_file.setnchannels(1)
                    wav_file.setsampwidth(2)
                    wav_file.setframerate(44100)


                    # Ghi lại toàn bộ dữ liệu âm thanh đã đọc và dữ liệu mới vào cuối tệp
                    if frames:
                        wav_file.writeframes(frames + data)
                    else:
                        wav_file.writeframes(data)
                    wav_file.close()