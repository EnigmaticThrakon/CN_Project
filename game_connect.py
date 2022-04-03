# import socket
#
# HEADER = 64
# PORT = 8888
# FORMAT = 'utf-8'
# DISCONNECT_MESSAGE = "!DISCONNECT"
# SERVER = "marvin.webredirect.org"
# ADDR = (SERVER, PORT)
#
# client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# client.connect(ADDR)
#
# def send(msg):
#     message = msg.encode(FORMAT)
#     msg_length = len(message)
#     send_length = str(msg_length).encode(FORMAT)
#     send_length += b' ' * (HEADER - len(send_length))
#     client.send(send_length)
#     client.send(message)
#     print(client.recv(2048).decode(FORMAT))
#
# send("Hello World!")
# input()
# send(DISCONNECT_MESSAGE)

from client import Client
from player import Player

client = Client()
player = Player(client)
player.player_move()
client.disconnect()