import socket

class Client():
    def __init__(self, header=64, port=8888, format='utf-8', disconnect_mess='!DISCONNECT',
                 server='marvin.webredirect.org'):
        self.header = header
        self.port = port
        self.format = format
        self.disconnect_message = disconnect_mess
        self.server = server
        self.address = (server, port)

        self.client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.client.connect(self.address)

    def send_msg(self, msg):
        message = msg.encode(self.format)
        msg_length = len(message)
        send_length = str(msg_length).encode(self.format)
        send_length += b' ' * (self.header - len(send_length))
        self.client.send(send_length)
        self.client.send(message)
        print(self.client.recv(2048).decode(self.format))

    def disconnect(self):
        self.send_msg(self.disconnect_message)