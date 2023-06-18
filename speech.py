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
    # Khởi tạo API
    wit = pywit.Wit(access_token=access_token)
    recognizer = sr.Recognizer()
    # Sử dụng Microphone laptop để thu âm giọng nói
    with sr.Microphone() as source:
        print("Hãy nói gì đó...")
        audio = recognizer.listen(source)
        text = ""
        try:
            """
                Chuyển đổi giọng nói thành văn bản bằng PyWit
                response sẽ trả về một distionary và chỉ cần lấy dữ liệu nhận dạng
            """
            # Sử dụng Google Speech Recognition để nhận dạng giọng nói
            text = recognizer.recognize_google(audio, language='vi-VN')
            print("Đoạn văn bản đã nhận dạng: " + text)
            response = wit.message(audio)
            extracted_text = response['text']
            print("Đoạn văn bản đã trích xuất từ PyWit: " + extracted_text)
        except sr.UnknownValueError:
            print("Không thể nhận dạng giọng nói.")
            print(sr.UnknownValueError)
        except sr.RequestError as e:
            print("Lỗi trong quá trình nhận dạng; {0}".format(e))
    return text