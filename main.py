import server
import broadcast
import threading




if __name__ == "__main__":
    threading.Thread(target=broadcast.broadcast).start()
    server.start_server()