import socket
import re

class Client():
    def __init__(self):
        self.header = 64
        self.port = 8888
        self.format = 'utf-8'
        self.server = 'marvin.webredirect.org'
        self.address = (self.server, self.port)
        self.client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.client.connect(self.address)
        self.is_right = bool(self.client.recv(2048).decode(self.format))
        self.connected = True
        self.temp_response = None


    def send_msg(self, msg):
        try:
            self.client.send(msg.encode(self.format))
        except socket.error as e:
            print(e)

    def recv_msg(self):
        try:
            response = str(self.client.recv(2048).decode(self.format))
            print(response)
            valid_response = re.search(r'<\d{3}:\d{3},\d{3}:\d{3},\d{3}>', response)
            if valid_response:
                self.temp_response = valid_response.group()
            return self.temp_response
        except Exception as e:
            print(e)

    def disconnect(self):
        self.send_msg("~~~")