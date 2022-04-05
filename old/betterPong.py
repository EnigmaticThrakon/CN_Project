# Import and initialize pygame
import pygame
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
    def __init__(self, x, y, size):
        self.x = x
        self.y = y
        self.size = size
        self.xVel = self.VEL
        self.yVel = self.VEL

    def draw(self, window):
        pygame.draw.rect(window, self.COLOR, (self.x, self.y, self.size, self.size))

    def move(self):
        self.x += self.xVel
        self.y += self.yVel

class Paddle:
    SPEED = 4
    def __init__(self, x, y, width, height):
        self.x = x
        self.y = y
        self.width = width
        self.height = height
    def draw(self, window):
        pygame.draw.rect(window, WHITE, (self.x, self.y, self.width, self.height))

    def move(self, up=True):
        if up:
            self.y -= self.SPEED
        else:
            self.y += self.SPEED


def draw(window, paddles, ball):
    window.fill(BLACK)

    for paddle in paddles:
        paddle.draw(window)

    ball.draw(window)

    pygame.display.update()

def paddleMovementHandler(keys, leftPaddle, rightPaddle):
    if keys[pygame.K_UP] and rightPaddle.y - rightPaddle.SPEED >= 0:
        rightPaddle.move(up=True)
    if keys[pygame.K_DOWN] and rightPaddle.y + rightPaddle.SPEED + rightPaddle.height <= HEIGHT:
        rightPaddle.move(up=False)

    if keys[pygame.K_w] and leftPaddle.y - leftPaddle.SPEED >= 0:
        leftPaddle.move(up=True)
    if keys[pygame.K_s] and leftPaddle.y + leftPaddle.SPEED + leftPaddle.height <= HEIGHT:
        leftPaddle.move(up=False)

def collisionHandler(ball, leftPaddle, rightPaddle):
    if ball.y < 0 or ball.y > HEIGHT - ball.size:
        ball.yVel *= -1
    if ball.xVel > 0:
        if ball.y >= rightPaddle.y and ball.y <= rightPaddle.y + rightPaddle.height:
            if ball.x + ball.size >= rightPaddle.x:
                ball.xVel *= -1
    else:
        if ball.y >= leftPaddle.y and ball.y <= leftPaddle.y + leftPaddle.height:
            if ball.x <= leftPaddle.x + leftPaddle.width:
                ball.xVel *= -1


# Main function
def main():
    run = True
    clock = pygame.time.Clock()

    leftPaddle = Paddle(10, HEIGHT // 2 - PADDLE_HEIGHT // 2, PADDLE_WIDTH, PADDLE_HEIGHT)
    rightPaddle = Paddle(WIDTH - 10 - PADDLE_WIDTH, HEIGHT // 2 - PADDLE_HEIGHT // 2, PADDLE_WIDTH, PADDLE_HEIGHT)

    ball = Ball(WIDTH // 2 - BALL_SIZE // 2, HEIGHT // 2 - BALL_SIZE // 2, BALL_SIZE)

    while run:
        clock.tick(FPS)
        draw(WINDOW, [leftPaddle, rightPaddle], ball)
        # If the window is closed, end the program
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                run = False
                break

        keys = pygame.key.get_pressed()
        paddleMovementHandler(keys, leftPaddle, rightPaddle)

        ball.move()
        collisionHandler(ball, leftPaddle, rightPaddle)

    pygame.quit()

# Begin the program
if __name__ == '__main__':
    main()