import pygame
from resource.variables import *

class Text:
    def __init__(self, size, color, message, x=None, y=None):
        self.x = x
        self.y = y
        self.size = size
        self.color = color
        self.message = message
        self.font = pygame.font.Font(SLKSCREB, self.size)


    def draw(self, window):
        render = self.font.render(self.message, True, self.color)
        window.blit(render, (self.x, self.y))