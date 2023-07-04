import pyaudio
import wave
import wit as pywit


access_token = "B6UBYQENUSQB3WZPPDKU2I3YDBHEKSDO"
client = pywit.Wit(access_token=access_token)

def record_audio(filename, duration):
    CHUNK = 1024
    FORMAT = pyaudio.paInt16
    CHANNELS = 1
    RATE = 16000

    p = pyaudio.PyAudio()

    stream = p.open(format=FORMAT,
                    channels=CHANNELS,
                    rate=RATE,
                    input=True,
                    frames_per_buffer=CHUNK)

    print("Recording...")

    frames = []

    for i in range(0, int(RATE / CHUNK * duration)):
        data = stream.read(CHUNK)
        frames.append(data)

    print("Finished recording.")

    stream.stop_stream()
    stream.close()
    p.terminate()

    wf = wave.open(filename, 'wb')
    wf.setnchannels(CHANNELS)
    wf.setsampwidth(p.get_sample_size(FORMAT))
    wf.setframerate(RATE)
    wf.writeframes(b''.join(frames))
    wf.close()

def transcribe_audio(filename):
    with open(filename, 'rb') as f:
        resp = client.speech(f, {'Content-Type': 'audio/wav'})

        if 'text' in resp:
            return resp['text']
        else:
            return None

filename = "micro.wav"
# record_audio(filename, 5)
transcription = transcribe_audio(filename)
print("Transcription:", transcription)

