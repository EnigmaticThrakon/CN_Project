import keyboard

class Player:
    def __init__(self, client, y_min=0, y_max=800, y_move=1, y_pos=400):
        self.client = client
        self.y_max = y_max
        self.y_min = y_min
        self.y_move = y_move
        self.y_pos = y_pos

    def player_move(self):
        while True:
            if keyboard.read_key() == "up":
                if self.y_pos > self.y_min:
                    self.y_pos -= self.y_move
            if keyboard.read_key() == "down":
                if self.y_pos < self.y_max:
                    self.y_pos += self.y_move
            print(self.y_pos)
            self.client.send_msg(str(self.y_pos))
