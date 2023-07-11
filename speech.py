"""
    Module speech

    Sử dụng thư viện pywit lấy API từ wit.ai để nhận dạng giọng nói
    Trả về giá trị đã tạo trên trang wit.ai
"""

import wit as pywit

# Token API của wit.ai để nhận dạng giọng nói
access_token = "B6UBYQENUSQB3WZPPDKU2I3YDBHEKSDO"
wit = pywit.Wit(access_token=access_token)

def transcribe_audio(filename):
    with open(filename, 'rb') as f:
        resp = wit.speech(f, {'Content-Type': 'audio/wav'})

        if 'text' in resp:
            return resp['text']
        else:
            return None

def speech(path):
    transcription = transcribe_audio(path)
    print("Transcription: ", transcription)
    return transcription