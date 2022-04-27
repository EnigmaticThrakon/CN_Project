import pygame
from dependencies.objects.ball import Ball
from dependencies.objects.paddle import Paddle
from dependencies.objects.score import Score
from dependencies.client import Client
from dependencies.objects.text import Text
from dependencies.objects.button import Button
from dependencies.variables import *
import time


class Game():
    def __init__(self):
        # Initialize pygame
        pygame.init()
        self.window = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
        pygame.display.set_caption("\"Ping\" Pong")
        # Define game objects and initialize
        self.winner = False
        self.player = Paddle(PADDLE_STARTING_RIGHT_X)
        self.opponent = Paddle(PADDLE_STARTING_LEFT_X)
        self.player_score = Score()
        self.opponent_score = Score()
        self.ball = Ball()
        # Initialize other variables
        self.prev_server_response = None

    def check_exit(self):
        # If the player clicks escape quit the game
        keys = pygame.key.get_pressed()
        if keys[pygame.K_ESCAPE]:
            pygame.quit()
        # Returns true if player quits the game
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                return True

    def draw(self, objects):
        # Draw all game objects onto the screen
        for object in objects:
            object.draw(self.window)
        pygame.display.update()

    def end(self):
        # Decided who the winner is and initialize text and button objects
        if self.winner:
            winner_text = Text(75, WHITE, "WINNER", 300, 100)
        else:
            winner_text = Text(75, WHITE, "LOSER", 300, 100)
        seconds_left = 10
        seconds_left_text = Text(75, WHITE, f"{seconds_left}", 450, 500)
        start = time.perf_counter()
        while seconds_left > 0:
            # Clear the window
            self.window.fill(BLACK)
            # If the window is closed, end the program
            if self.check_exit():
                return True
            if time.perf_counter() - start > 1:
                seconds_left -= 1
                seconds_left_text = Text(75, WHITE, f"{seconds_left}", 475, 500)
                start = time.perf_counter()
            self.draw([winner_text, seconds_left_text])
            pygame.display.update()
        return True

    def fade_out(self, objects):
        for rgb in range(255, -1, -1):
            for object in objects:
                object.color = (rgb, rgb, rgb)
            self.draw(objects)
            pygame.display.update()
            time.sleep(.005)

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

    def loop(self):
        while True:
            # If the window is closed, end the program
            if self.check_exit():
                return True
            # Clear the window
            self.window.fill(BLACK)
            self.draw([self.player, self.opponent, self.player_score, self.opponent_score, self.ball])
            self.playerMovementHandler(pygame.key.get_pressed())
            self.client.send_msg("{:03d}".format(int(self.player.y)))
            self.set_info(self.parse_response(self.client.recv_msg()))
            # Check for winner
            if self.player_score.score == 5 or self.opponent_score.score == 5:
                self.draw([self.player, self.opponent, self.player_score, self.opponent_score, self.ball])
                if self.player_score.score == 5:
                    self.winner = True
                return

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

    def playerMovementHandler(self, keys):
        if keys[pygame.K_UP] and self.player.y - self.player.speed >= 0:
            return self.player.move(up=True)
        if keys[pygame.K_DOWN] and self.player.y + self.player.speed + self.player.height <= SCREEN_HEIGHT:
            return self.player.move(up=False)

    def set_info(self, server_response):
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

    def start(self):
        # Initialize start button, title, and background music
        pygame.mixer.music.load(INTRO_SOUND)
        pygame.mixer.music.set_volume(1)
        pygame.mixer.music.play(-1)
        title_text = Text(75, WHITE, "''PING'' PONG", 100, 100)
        start_text = Text(40, BLACK, "START")
        start_button = Button(400, 350, 200, 50, LIGHT_GREY, WHITE, start_text)
        self.window.fill(BLACK)
        # While both players are not connected
        while True:
            # If the window is closed, end the program
            if self.check_exit():
                return True
            # Draw the screen
            self.draw([title_text, self.player, self.opponent])
            pygame.display.update()
            # Fade title screen to black once button is clicked
            if start_button.create_button(self.window):
                # Fade title to black and background music upon starting the game
                pygame.mixer.music.fadeout(1275)
                self.fade_out([title_text, start_button])
                # Initialize client and wait for opponent
                return self.wait_for_opponent()

    def wait_for_opponent(self):
        # Initialize waiting texts
        waiting_count = 0
        waiting_text_zero = Text(75, WHITE, "WAITING", 125, 100)
        waiting_text_one = Text(75, WHITE, "WAITING .", 125, 100)
        waiting_text_two = Text(75, WHITE, "WAITING . .", 125, 100)
        waiting_text_three = Text(75, WHITE, "WAITING . . .", 125, 100)
        left_paddle = Paddle(x=PADDLE_STARTING_LEFT_X)
        right_paddle = Paddle(x=PADDLE_STARTING_RIGHT_X)
        # Initialize the client
        self.initialize_client()
        response = None
        self.client.set_blocking(0)
        while response != 999:
            # If the window is closed, end the program
            if self.check_exit():
                return True
            # Clear the window
            self.window.fill(BLACK)
            # Check server for response
            response = self.parse_response(self.client.recv_msg())[0]
            # Clear the window
            # Draw waiting text to screen
            if waiting_count == 0:
                waiting_text = waiting_text_zero
            elif waiting_count == 1:
                waiting_text = waiting_text_one
            elif waiting_count == 2:
                waiting_text = waiting_text_two
            elif waiting_count == 3:
                waiting_text = waiting_text_three
                waiting_count = -1
            # Draw the screen
            self.draw([waiting_text, self.player, self.opponent])
            waiting_count += 1
            pygame.display.update()
            time.sleep(.5)
        self.fade_out([waiting_text])
        self.client.set_blocking(1)
        # Send connection acknowledgement
        self.client.send_msg("999")
        return False