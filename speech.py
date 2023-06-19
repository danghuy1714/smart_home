"""
    Module speech

    Sử dụng thư viện pywit lấy API từ wit.ai để nhận dạng giọng nói
    Trả về giá trị đã tạo trên trang wit.ai
"""

# import wit as pywit
# import speech_recognition as sr
# import time

# # Token API của wit.ai để nhận dạng giọng nói
# access_token = "B6UBYQENUSQB3WZPPDKU2I3YDBHEKSDO"

# def speech(file_path):
#     # Khởi tạo API
#     wit = pywit.Wit(access_token=access_token)
#     with open(file_path, "rb") as audio:
#         response = wit.speech(audio)
#         print(response)
#     text = 1
#     return text

# speech()

"""
    Module speech

    Sử dụng thư viện pywit lấy API từ wit.ai để nhận dạng giọng nói
    Trả về giá trị đã tạo trên trang wit.ai
"""

import wit as pywit
import speech_recognition as sr
import time

# Token API của wit.ai để nhận dạng giọng nói
access_token = "B6UBYQENUSQB3WZPPDKU2I3YDBHEKSDO"

def speech():
    device = int(input("Device: "))
    text = input("Text:")
    return device, text
    
    