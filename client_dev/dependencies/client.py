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

    def disconnect(self):
        self.send_msg("~~~")

    def recv_msg(self):
        try:
            # Receives a message (string format) from the server and decodes it
            response = str(self.client.recv(2048).decode(self.format))
            # Format = <player_x,score_x:ball_x,ball_y:player_score,opponent_score> (all three digit integers)
            valid_response = re.search(r'<\d{3},\d{3}:\d{3},\d{3}:\d{3},\d{3}>', response)
            if valid_response:
                self.temp_response = valid_response.group()
            # Search through response for a message matching the desired format and return
            return self.temp_response
        except Exception as ex:
            print(ex)

    def send_msg(self, msg):
        try:
            # Send a message (string format) to the server
            self.client.send(msg.encode(self.format))
        except Exception as ex:
            print(ex)

    def set_blocking(self, blocking_enum):
        # Allows change of blocking
        self.client.setblocking(blocking_enum)
