"""
    Module network có chức năng tạo các luồng đi và đến server
    SendESP32 là luồng gửi dữ liệu đến thiết bị
    Speech là luồng nhận dữ liệu từ micro
"""


import threading
import time
import speech
import wave
import os


# Lớp cha chứa thông tin thề socket
class MultiSocket(threading.Thread):
    def __init__(self, client_socket, client_address):
        super().__init__()
        self.client_socket = client_socket
        self.client_address = client_address


# Kết thừa từ lớp multisocket có chức năng gửi data đến esp32(tại esp32 điều khiến)
class SendESP32(MultiSocket):
    def __init__(self, client_socket, client_address):
        super().__init__(client_socket, client_address)

        """
            Thiết lập trạng thái cũ và trạng thái mới nhận được,
            nếu tráng thái mới đã thay đổi so với trạng thái cũ
            thì gửi data đến esp32
        """
        self.old_status = -1
        self.new_status = -1


    def run(self):

        """
            Kiểm tra trạng thái và gửi data đến esp32 đã đăng ký
        """
        while True:
            if self.old_status != self.new_status:
                self.client_socket.send(str(self.new_status).encode())
                self.old_status = self.new_status
                time.sleep(2)

    """
        Khi trạng thái thay đổi, cập nhận trạng thái mới từ luồng
        khác bằng hàm update
    """
    def update(self, status):
        self.new_status = status


"""
    Luồng speech đăng ký là các thiết bị micro để nhận giọng nói,
    xử lý và trả về các thông tin đã nhận dạng
"""
class Speech(MultiSocket):
    def __init__(self, client_socket, client_address):
        super().__init__(client_socket=client_socket, client_address=client_address)
        self.audio = None
        self.audio_path = "micro" + ".wav"
        self.complete = 0
    def run(self):
        while True:
            data = self.client_socket.recv(1024)
            if data:
                # Mở tệp âm thanh ở chế độ đọc
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
                        print(control)
                except:
                    pass
                # Mở tệp âm thanh ở chế độ ghi (viết lại toàn bộ dữ liệu)
                with wave.open(self.audio_path, 'wb') as wav_file:
                    # Cấu hình thông số của tệp âm thanh
                    wav_file.setnchannels(1)
                    wav_file.setsampwidth(2)
                    wav_file.setframerate(16000)


                    # Ghi lại toàn bộ dữ liệu âm thanh đã đọc và dữ liệu mới vào cuối tệp
                    if frames:
                        wav_file.writeframes(frames + data)
                    else:
                        wav_file.writeframes(data)
                    wav_file.close()
                # self.complete = 1
            # time.sleep(2)
        

        