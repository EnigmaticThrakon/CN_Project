import pygame
from client_dev.variables import *


class Paddle():
    def __init__(self, x):
        self.width = PADDLE_WIDTH
        self.height = PADDLE_HEIGHT
        self.speed = PADDLE_SPEED
        self.color = WHITE
        self.y = PADDLE_STARTING_Y
        self.x = x

    # Draw paddle on screen
    def draw(self, window):
        pygame.draw.rect(window, self.color, (self.x, self.y, self.width, self.height))

    # Move player paddle up or down
    def move(self, up=True):
        if up:
            self.y -= self.speed
        else:
            self.y += self.speed

    # Sets y location of paddle
    def set_loc(self, y):
        self.y = y