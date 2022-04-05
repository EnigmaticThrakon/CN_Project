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
        self.is_right = bool(self.client.recv(2048).decode(self.format))
        self.connected = True


    def send_msg(self, msg):
        try:
            self.client.send(msg.encode(self.format))
        except socket.error as e:
            print(e)

    def recv_msg(self):
        try:
            while(1 == 1):
                response = (str)(self.client.recv(2048))#.decode()#(self.format)

                openingBracketPos = (int)(response.index('<'))
                closingBracketPos = (int)(response.index('>', openingBracketPos))

                if(openingBracketPos is not None and closingBracketPos is not None):
                    try:
                        temp_response = response[openingBracketPos:closingBracketPos + 1]

                        if(temp_response[1:4].isnumeric()):
                            return temp_response
                    except Exception as temp:
                        return "<000:000,000:000,000>"
                else:
                    return "<000:000,000:000,000>"
        except socket.error as e:
            print(e)
        except Exception as ex:
            print(ex)

    def disconnect(self):
        self.send_msg("~~~")