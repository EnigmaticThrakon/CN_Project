import pygame
from client_dev.variables import *


class Ball():
    def __init__(self):
        self.size = BALL_SIZE
        self.color = WHITE
        self.x = BALL_STARTING_POS[0]
        self.y = BALL_STARTING_POS[1]

    def draw(self, window):
        pygame.draw.rect(window, self.color, (self.x, self.y, self.size, self.size))

    def set_loc(self, x, y):
        self.x = x
        self.y = y