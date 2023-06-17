import socket
import speech
import server
import broadcast
import threading




if __name__ == "__main__":
    threading.Thread(target=broadcast.broadcast).start()
    threading.Thread(target=server.start_server).start()
    while True:
        pass