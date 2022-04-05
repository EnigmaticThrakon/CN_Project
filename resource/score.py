import pygame
from resource.variables import *


class Score:
    def __init__(self, is_right):
        self.color = WHITE
        self.score = 0
        self.size = 100
        self.font = pygame.font.Font('pong_font.ttf', self.size)
        self.y = SCREEN_HEIGHT / 16

        if is_right:
            self.x = (SCREEN_WIDTH / 2) + (SCREEN_WIDTH / 6)
        else:
            self.x = (SCREEN_WIDTH / 2) - (SCREEN_WIDTH / 6) - self.size / 2

    def draw(self, window):
        render = self.font.render(str(int(self.score)), True, self.color)
        window.blit(render, (self.x, self.y))