from resource.variables import *


class Paddle():
    def __init__(self, is_right, client=None):
        self.client = client
        self.width = 20
        self.height = 100
        self.speed = 4
        self.color = WHITE
        self.y = SCREEN_HEIGHT / 2 - self.height / 2

        if is_right:
            self.x = SCREEN_WIDTH - self.width - 10
        else:
            self.x = 10

    def draw(self, window):
        pygame.draw.rect(window, self.color, (self.x, self.y, self.width, self.height))

    def move(self, up=True):
        if up:
            self.y -= self.speed
        else:
            self.y += self.speed

    def set_loc(self, y):
        self.y = y