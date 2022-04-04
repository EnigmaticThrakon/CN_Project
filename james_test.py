# Import and initialize pygame
import pygame
from client import Client

# client = Client()
pygame.init()

# Variable Initialization
WIDTH, HEIGHT = 1000, 800                           # Width and height of game window
WINDOW = pygame.display.set_mode((WIDTH, HEIGHT))   # Generate a window of size SIZE
pygame.display.set_caption("\"Ping\" Pong")         # Set window caption
FPS = 60                                            # Set max FPS to 60

WHITE = (255, 255, 255)                             # RGB values for white
BLACK = (0, 0, 0)                                   # RGB values for black

PADDLE_WIDTH, PADDLE_HEIGHT = 20, 100
BALL_SIZE = 20

class Ball:
    VEL = 4
    COLOR = WHITE
    def __init__(self):
        self.x = WIDTH // 2 - BALL_SIZE // 2
        self.y = HEIGHT // 2 - BALL_SIZE // 2
        self.size = BALL_SIZE
        self.xVel = self.VEL
        self.yVel = self.VEL

    def draw(self, window):
        pygame.draw.rect(window, self.COLOR, (self.x, self.y, self.size, self.size))

    def move(self):
        self.x += self.xVel
        self.y += self.yVel

    def set_loc(self, x, y):
        self.x = x
        self.y = y


class Paddle:
    def __init__(self, x):
        self.x = x
        self.y = HEIGHT // 2 - PADDLE_HEIGHT // 2
        self.width = PADDLE_WIDTH
        self.height = PADDLE_HEIGHT
        self.speed = 4

    def draw(self, window):
        pygame.draw.rect(window, WHITE, (self.x, self.y, self.width, self.height))

    def move(self, up=True):
        if up:
            self.y -= self.speed
        else:
            self.y += self.speed
        #TODO: Send client-side paddle location to server
        # self.client.send_msg(str(self.y))

    def set_loc(self, y):
        self.y = y



class Game():
    def __init__(self):
        # Define game objects
        self.leftPaddle = Paddle(10)
        self.rightPaddle = Paddle(WIDTH - 10 - PADDLE_WIDTH)
        self.ball = Ball()
        # TODO: Pull fake data in from server instead
        self.game_info = "<400:500,400:0,0>"


    def draw(self):
        WINDOW.fill(BLACK)
        for paddle in [self.rightPaddle, self.leftPaddle]:
            paddle.draw(WINDOW)
        self.ball.draw(WINDOW)
        pygame.display.update()

    def paddleMovementHandler(self, keys):
        if keys[pygame.K_UP] and self.rightPaddle.y - self.rightPaddle.speed >= 0:
            self.rightPaddle.move(up=True)
        if keys[pygame.K_DOWN] and self.rightPaddle.y + self.rightPaddle.speed + self.rightPaddle.height <= HEIGHT:
            self.rightPaddle.move(up=False)

    def collisionHandler(self):
        if self.ball.y < 0 or self.ball.y > HEIGHT - self.ball.size:
            self.ball.yVel *= -1
        if self.ball.xVel > 0:
            if self.ball.y >= self.rightPaddle.y and self.ball.y <= self.rightPaddle.y + self.rightPaddle.height:
                if self.ball.x + self.ball.size >= self.rightPaddle.x:
                    self.ball.xVel *= -1

    def set_game_info(self):
        game_info = self.game_info.split(":")
        # Get opponent pos
        self.leftPaddle.set_loc(float(game_info[0][1:-1]))
        # Get ball pos
        self.ball.set_loc(float(game_info[1].split(",")[0]), float(game_info[1].split(",")[1]))
        # # Get player and opponent score
        # self.player_score = float(game_info[2].split(",")[0])
        # self.opponent_score = float(game_info[2].split(",")[1][0:-1])

    # USED FOR DEV
    def fake_game_info(self):
        game_info_splt = self.game_info.split(":")
        game_info_splt[0] = "<" + str(float(game_info_splt[0][1:]) + 1)
        game_info_splt[1] = str(float(game_info_splt[1].split(",")[0]) + 1) + "," + str(float(game_info_splt[1].split(",")[0]) + 1)
        temp = ""
        for x in game_info_splt:
            temp += x + ":"
        self.game_info = temp

# Main function
def main():
    run = True
    clock = pygame.time.Clock()
    game = Game()


    while run:
        # TODO: Pull fake data in from server instead
        game.fake_game_info()
        clock.tick(FPS)
        game.draw()
        # If the window is closed, end the program
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                run = False
                break
        keys = pygame.key.get_pressed()
        game.paddleMovementHandler(keys)
        game.set_game_info()
        game.collisionHandler()

    pygame.quit()

# Begin the program
if __name__ == '__main__':
    main()