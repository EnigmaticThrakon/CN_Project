import pygame
from client_dev.variables import *


class Score:
    def __init__(self):
        self.color = WHITE
        self.score = STARTING_SCORE
        self.size = SCORE_SIZE
        self.font = pygame.font.Font(PONG, self.size)
        self.y = SCORE_STARTING_Y
        self.x = None

    def draw(self, window):
        render = self.font.render(str(int(self.score)), True, self.color)
        window.blit(render, (self.x, self.y))

    def set_score(self, new_score):
        pygame.mixer.music.load("score.mp3")
        pygame.mixer.music.set_volume(1)
        if new_score != self.score:
            pygame.mixer.music.play()
            self.score = new_score
