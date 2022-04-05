import pygame
import threading
from client import Client

# Import and initialize pygame
pygame.init()
# Variable Initialization
WIDTH, HEIGHT = 1000, 800                           # Width and height of game window
WINDOW = pygame.display.set_mode((WIDTH, HEIGHT))   # Generate a window of size SIZE
pygame.display.set_caption("\"Ping\" Pong")         # Set window caption
FPS = 60                                            # Set max FPS to 60

WHITE = (255, 255, 255)                             # RGB values for white
BLACK = (0, 0, 0)                                   # RGB values for black


class Ball():
    def __init__(self):
        self.size = 20
        self.color = WHITE
        self.x = WIDTH / 2 - self.size / 2
        self.y = HEIGHT / 2 - self.size / 2

    def draw(self, window):
        pygame.draw.rect(window, self.color, (self.x, self.y, self.size, self.size))

    def set_loc(self, x, y):
        self.x = x
        self.y = y


class Paddle():
    def __init__(self, is_right, client=None):
        self.client = client
        self.width = 20
        self.height = 100
        self.speed = 4
        self.color = WHITE
        self.y = HEIGHT / 2 - self.height / 2

        if is_right:
            self.x = WIDTH - self.width - 10
        else:
            self.x = 10

    def draw(self, window):
        pygame.draw.rect(window, self.color, (self.x, self.y, self.width, self.height))

    def move(self, up=True):
        if up:
            self.y -= self.speed
        else:
            self.y += self.speed

    def set_loc(self, y):
        self.y = y


class Score:
    def __init__(self, is_right):
        self.color = WHITE
        self.score = 0
        self.size = 100
        self.font = pygame.font.Font('pong_font.ttf', self.size)
        self.y = HEIGHT / 16

        if is_right:
            self.x = (WIDTH / 2) + (WIDTH / 6)
        else:
            self.x = (WIDTH / 2) - (WIDTH / 6) - self.size / 2

    def draw(self, window):
        render = self.font.render(str(int(self.score)), True, self.color)
        window.blit(render, (self.x, self.y))


class Game():
    def __init__(self):
        # Initialize client
        self.client = Client()
        # Define game objects
        self.player = Paddle(self.client.is_right, self.client)
        self.opponent = Paddle(not self.client.is_right)
        self.player_score = Score(self.client.is_right)
        self.opponent_score = Score(not self.client.is_right)
        self.ball = Ball()

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
        if keys[pygame.K_DOWN] and self.player.y + self.player.speed + self.player.height <= HEIGHT:
            return self.player.move(up=False)
        # If the player clicks escape quit the game and disconnect from the server
        if keys[pygame.K_ESCAPE]:
            pygame.quit()
            self.client.disconnect()
            return False

    def set_game_info(self):
        game_info_raw = self.client.recv_msg().strip()
        game_info = game_info_raw.split(":")
        # Get opponent pos
        self.opponent.set_loc(int(game_info[0][1:]))
        # Get ball pos
        self.ball.set_loc(int(game_info[1].split(",")[0]), int(game_info[1].split(",")[1]))
        # Get player and opponent score
        self.player_score.score = int(game_info[2].split(",")[0])
        self.opponent_score.score = int(game_info[2].split(",")[1][0:-1])


# Main function
def main():
    run = True
    clock = pygame.time.Clock()
    game = Game()

    while run:
        try:
            clock.tick(FPS)
            game.draw()
            # If the window is closed, end the program
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    run = False
                    break
            keys = pygame.key.get_pressed()
            # Breaks loop if player presses escape
            if game.playerMovementHandler(keys) is False:
                return
            game.client.send_msg(str(int(game.player.x)))
            game.set_game_info()
        except Exception as e:
            pass

# Begin the program
if __name__ == '__main__':
    main()