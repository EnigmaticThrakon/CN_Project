import socket

class Client():
    def __init__(self):
        self.header = 64
        self.port = 8888
        self.format = 'utf-8'
        self.server = 'marvin.webredirect.org'
        self.address = (self.server, self.port)
        self.client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.client.connect(self.address)
        # self.is_right = bool(self.client.recv(2048).decode(self.format))
        self.is_right = True
        self.connected = True


    def send_msg(self, msg):
        try:
            self.client.send(msg.encode(self.format))
        except socket.error as e:
            print(e)

    def recv_msg(self):
        try:
            return self.client.recv(2048).decode(self.format)
        except socket.error as e:
            print(e)

    def disconnect(self):
        self.send_msg("~~~")