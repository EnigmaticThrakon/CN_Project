import pygame

# Game variables
SCREEN_WIDTH, SCREEN_HEIGHT = 1000, 800             # Width and height of game window
FPS = 180                                           # Set max FPS to 60
WHITE = (255, 255, 255)                             # RGB values for white
BLACK = (0, 0, 0)                                   # RGB values for black
# Ball variables
BALL_SIZE = 20
BALL_STARTING_POS = (SCREEN_WIDTH / 2 - BALL_SIZE / 2, SCREEN_HEIGHT / 2 - BALL_SIZE / 2)
# Paddle variables
PADDLE_SPEED = 3
PADDLE_HEIGHT = 100
PADDLE_WIDTH = 20
PADDLE_STARTING_Y = SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2
PADDLE_STARTING_RIGHT_X = 970
PADDLE_STARTING_LEFT_X = 10
# Score variables
STARTING_SCORE = 0
SCORE_SIZE = 120
SCORE_STARTING_Y = 50
SCORE_STARTING_RIGHT_X = 650
SCORE_STARTING_LEFT_X = 290


pygame.init()  # Import and initialize pygame
WINDOW = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))  # Generate a window of size SIZE
pygame.display.set_caption("\"Ping\" Pong")  # Set window caption