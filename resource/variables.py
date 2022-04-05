import pygame

# Variable Initialization
SCREEN_WIDTH, SCREEN_HEIGHT = 1000, 800             # Width and height of game window
FPS = 60                                            # Set max FPS to 60
WHITE = (255, 255, 255)                             # RGB values for white
BLACK = (0, 0, 0)                                   # RGB values for black

pygame.init()  # Import and initialize pygame
WINDOW = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))  # Generate a window of size SIZE
pygame.display.set_caption("\"Ping\" Pong")  # Set window caption