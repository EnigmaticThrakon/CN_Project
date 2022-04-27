import pygame
from ..variables import *


class Score:
    def __init__(self):
        self.color = WHITE
        self.score = STARTING_SCORE
        self.size = SCORE_SIZE
        self.font = pygame.font.Font(PONG, self.size)
        self.y = SCORE_STARTING_Y
        self.x = None
        self.score_sound = pygame.mixer.Sound(SCORE_SOUND)

    # Draws score on screen
    def draw(self, window):
        render = self.font.render(str(int(self.score)), True, self.color)
        window.blit(render, (self.x, self.y))

    # Sets score and plays score sound
    def set_score(self, new_score):
        if new_score != self.score:
            self.score_sound.play()
            self.score = new_score
