import keyboard

class Game:
    def __init__(self, client):
        self.client = client
        self.y_max = 800
        self.y_min = 0
        self.y_move = 1
        self.player_pos = 400
        self.opponent_pos = 400
        self.player_score = 0
        self.opponent_score = 0
        self.ball_x_pos = 500
        self.ball_y_pos = 400

    def game_loop(self):
        while True:
            # Move player up
            if keyboard.read_key() == "up":
                if self.player_pos > self.y_min:
                    self.player_pos -= self.y_move
            # Move player down
            if keyboard.read_key() == "down":
                if self.player_pos < self.y_max:
                    self.player_pos += self.y_move
            self.client.send_msg(str(self.player_pos))

    def set_game_info(self):
        try:
            game_info = self.client.recv_msg().split(":")
            # Get opponent pos
            self.opponent_pos = int(game_info[0][1:-1])
            # Get ball pos
            self.ball_x_pos = int(game_info[1].split(",")[0])
            self.ball_y_pos = int(game_info[1].split(",")[1])
            # Get player and opponent score
            self.player_score = int(game_info[2].split(",")[0])
            self.opponent_score = int(game_info[2].split(",")[1][0:-1])
            print(self.__dict__)
        except:
            pass


            
        


