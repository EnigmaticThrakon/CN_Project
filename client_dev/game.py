import pygame
from client_dev.objects.ball import Ball
from client_dev.objects.paddle import Paddle
from client_dev.objects.score import Score
from client_dev.client import Client
from client_dev.variables import *


class Game():
    def __init__(self):
        # Initialize pygame
        pygame.init()
        self.window = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
        pygame.display.set_caption("\"Ping\" Pong")
        # Define game objects and initialize
        self.winner = False
        self.player = Paddle()
        self.opponent = Paddle()
        self.player_score = Score()
        self.opponent_score = Score()
        self.ball = Ball()
        # Initialize other variables
        self.prev_server_response = None

    def initialize_client(self):
        # Initialize client and set player x positions
        self.client = Client()
        if self.parse_response(self.client.player_screen_side)[0] == 111:
            self.player_right = True
            self.player.x = PADDLE_STARTING_RIGHT_X
            self.opponent.x = PADDLE_STARTING_LEFT_X
            self.player_score.x = SCORE_STARTING_RIGHT_X
            self.opponent_score.x = SCORE_STARTING_LEFT_X
        else:
            self.player_right = False
            self.player.x = PADDLE_STARTING_LEFT_X
            self.opponent.x = PADDLE_STARTING_RIGHT_X
            self.player_score.x = SCORE_STARTING_LEFT_X
            self.opponent_score.x = SCORE_STARTING_RIGHT_X
        # Respond to player once initial positions are set
        self.client.send_msg("debug")

    def draw(self, objects):
        # Draw all game objects onto the screen
        self.window.fill(BLACK)
        for object in objects:
            object.draw(self.window)
        pygame.display.update()

    def playerMovementHandler(self, keys):
        if keys[pygame.K_UP] and self.player.y - self.player.speed >= 0:
            return self.player.move(up=True)
        if keys[pygame.K_DOWN] and self.player.y + self.player.speed + self.player.height <= SCREEN_HEIGHT:
            return self.player.move(up=False)

    def parse_response(self, server_response=None):
        # Gets response from server, parse, and return
        if server_response is None:
            return [0, 0, 0, 0, 0, 0]
        else:
            server_response = server_response.strip().split(":")
            return [int(server_response[0].split(",")[0][1:]),
                    int(server_response[0].split(",")[1]),
                    int(server_response[1].split(",")[0]),
                    int(server_response[1].split(",")[1]),
                    int(server_response[2].split(",")[0]),
                    int(server_response[2].split(",")[1][0:-1])]

    def set_game_info(self, server_response):
        if server_response[0] == 999:
            return
        # Set ball position
        self.ball.set_loc(server_response[2], server_response[3])
        # Set opponent paddle position and scores
        if self.player_right:
            self.opponent.set_loc(server_response[0])
            self.player_score.set_score(server_response[5])
            self.opponent_score.set_score(server_response[4])
        else:
            self.opponent.set_loc(server_response[1])
            self.player_score.set_score(server_response[4])
            self.opponent_score.set_score(server_response[5])


    def check_exit(self):
        # If the player clicks escape quit the game
        keys = pygame.key.get_pressed()
        if keys[pygame.K_ESCAPE]:
            pygame.quit()
        # Returns true if player quits the game
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                return True
