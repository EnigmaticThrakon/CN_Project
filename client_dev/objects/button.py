import pygame
from client_dev.variables import *

class Button:
    def __init__(self, x, y, width, height, hover_color, default_color, text_obj):
        self.x = x
        self.y = y
        self.width = width
        self.height = height
        self.hover_color = hover_color
        self.default_color = default_color
        self.button_text = text_obj
        self.color = self.default_color

    def create_button(self, window):
        mouse = pygame.mouse.get_pos()
        # Mouse get pressed can run without an integer, but needs a 3 or 5 to indicate how many buttons
        click = pygame.mouse.get_pressed(3)
        if self.x + self.width > mouse[0] > self.x and self.y + self.height > mouse[1] > self.y:
            self.color = self.hover_color
            if click[0] == 1:
                return True
        else:
            self.color = self.default_color
        self.draw(window)

    def draw(self, window):
        # Draw button
        pygame.draw.rect(window, self.color, (self.x, self.y, self.width, self.height))
        # Draw text on button
        self.button_text.x = self.x + self.width / 16
        self.button_text.y = self.y + self.height / 8
        self.button_text.draw(window)