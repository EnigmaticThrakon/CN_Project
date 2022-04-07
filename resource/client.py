import socket
import re

class Client():
    def __init__(self):
        # Initialize server variables
        self.temp_response = None
        self.header = 64
        self.port = 8888
        self.format = 'utf-8'
        self.server = 'marvin.webredirect.org'
        self.address = (self.server, self.port)
        # Initialize client and connect
        self.client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.client.connect(self.address)
        self.connected = True
        # Receive player screen side. Format = <000,000:000,000:000:000> (left) or <111,111:111,111:111:111> (right)
        self.player_screen_side = self.recv_msg()
        # TODO: Remove this
        self.send_msg("debug")
        # Receive initial paddle and score position. Format = <player_x,score_x:xxx,xxx:xxx:xxx>  (all three digit integers)
        self.start_pos = self.recv_msg()

    def send_msg(self, msg):
        try:
            self.client.send(msg.encode(self.format))
        except Exception as ex:
            print(ex)

    def recv_msg(self):
        try:
            response = str(self.client.recv(2048).decode(self.format))
            valid_response = re.search(r'<\d{3}:\d{3},\d{3}:\d{3},\d{3}>', response)
            if valid_response:
                self.temp_response = valid_response.group()
            return self.temp_response
        except Exception as ex:
            print(ex)

    def disconnect(self):
        self.send_msg("~~~")