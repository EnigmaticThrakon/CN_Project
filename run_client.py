import pygame
from resource.game import Game
from resource.variables import *


# Main function
def main():
    clock = pygame.time.Clock()
    game = Game()
    index = 0
    # while True:
    #     try:
    #         clock.tick(FPS)
    #         game.draw()
    #         keys = pygame.key.get_pressed()
    #         # Breaks loop if player presses escape
    #         if game.playerMovementHandler(keys) is False:
    #             return
    #         game.client.send_msg("{:03d}".format(int(game.player.y)))
    #         game.set_game_info()
    #         # If the window is closed, end the program
    #         for event in pygame.event.get():
    #             if event.type == pygame.QUIT:
    #                 game.client.disconnect()
    #                 return
    #     except Exception as e:
    #         index = index + 1
    #         if(index > 10):
    #             return

# Begin the program
if __name__ == '__main__':
    main()