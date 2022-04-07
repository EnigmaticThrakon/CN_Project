import pygame
from resource.variables import *


class Paddle():
    def __init__(self):
        self.width = PADDLE_WIDTH
        self.height = PADDLE_HEIGHT
        self.speed = PADDLE_SPEED
        self.color = WHITE
        self.y = PADDLE_STARTING_Y
        self.x = 0

    def draw(self, window):
        pygame.draw.rect(window, self.color, (self.x, self.y, self.width, self.height))

    def move(self, up=True):
        if up:
            self.y -= self.speed
        else:
            self.y += self.speed

    def set_loc(self, y):
        self.y = y