from resource.client import Client
from resource.ball import Ball
from resource.paddle import Paddle
from resource.score import Score
from resource.variables import *


class Game():
    def __init__(self):
        # Initialize client
        self.client = Client()
        # Define game objects
        self.player = Paddle(self.client.is_right)
        self.opponent = Paddle(not self.client.is_right)
        self.player_score = Score(self.client.is_right)
        self.opponent_score = Score(not self.client.is_right)
        self.ball = Ball()
        self.prev_server_response = None

    def draw(self):
        WINDOW.fill(BLACK)
        for paddle in [self.player, self.opponent]:
            paddle.draw(WINDOW)
        for score in [self.player_score, self.opponent_score]:
            score.draw(WINDOW)
        self.ball.draw(WINDOW)
        pygame.display.update()

    def playerMovementHandler(self, keys):
        if keys[pygame.K_UP] and self.player.y - self.player.speed >= 0:
            return self.player.move(up=True)
        if keys[pygame.K_DOWN] and self.player.y + self.player.speed + self.player.height <= SCREEN_HEIGHT:
            return self.player.move(up=False)
        # If the player clicks escape quit the game and disconnect from the server
        if keys[pygame.K_ESCAPE]:
            pygame.quit()
            self.client.disconnect()
            return False

    def set_game_info(self):
        # Gets response from server
        server_response = self.client.recv_msg().strip()
        game_info = server_response.split(":")
        # Get opponent pos
        self.opponent.set_loc(int(game_info[0][1:]))
        # Get ball pos
        self.ball.set_loc(int(game_info[1].split(",")[0]), int(game_info[1].split(",")[1]))
        # Get player and opponent score
        self.player_score.score = int(game_info[2].split(",")[0])
        self.opponent_score.score = int(game_info[2].split(",")[1][0:-1])
