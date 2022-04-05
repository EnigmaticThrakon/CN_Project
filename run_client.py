import pygame
from resource.game import Game
from resource.variables import *


# Main function
def main():
    clock = pygame.time.Clock()
    game = Game()

    while True:
        try:
            clock.tick(FPS)
            game.draw()
            # If the window is closed, end the program
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    run = False
                    break
            keys = pygame.key.get_pressed()
            # Breaks loop if player presses escape
            if game.playerMovementHandler(keys) is False:
                return
            game.client.send_msg(str(int(game.player.y))[0:3])
            game.set_game_info()
        except Exception as e:
            pass

# Begin the program
if __name__ == '__main__':
    main()