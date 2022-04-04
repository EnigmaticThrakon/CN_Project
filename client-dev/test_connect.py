import socket
import msvcrt

HEADER = 64
PORT = 8888
FORMAT = 'utf-8'
DISCONNECT_MESSAGE = "!DISCONNECT"
SERVER = "marvin.webredirect.org"
ADDR = (SERVER, PORT)

client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client.connect(ADDR)

def send(msg):
    message = msg.encode(FORMAT)
    msg_length = len(message)
    send_length = str(msg_length).encode(FORMAT)
    send_length += b' ' * (HEADER - len(send_length))
    #client.send(send_length)
    client.send(message)
    print(client.recv(2048).decode(FORMAT))

index = 0
while(1 == 1):
    if(index == 0):
        send("50")

    print(client.recv(2048).decode(FORMAT))

    # if(msvcrt.kbhit()):
    #     client.send("100".encode(FORMAT))
        #send("100")

#send(DISCONNECT_MESSAGE)
