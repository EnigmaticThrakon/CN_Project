from resource.variables import *


class Ball():
    def __init__(self):
        self.size = 20
        self.color = WHITE
        self.x = SCREEN_WIDTH / 2 - self.size / 2
        self.y = SCREEN_HEIGHT / 2 - self.size / 2

    def draw(self, window):
        pygame.draw.rect(window, self.color, (self.x, self.y, self.size, self.size))

    def set_loc(self, x, y):
        self.x = x
        self.y = y