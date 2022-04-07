import pygame
from resource.button import Button
from resource.game import Game
from resource.variables import *

def start_game(game, clock):
    index = 0
    while True:
        try:
            game.window.fill(BLACK)
            clock.tick(FPS)
            start_button = Button()
            start_button.create_button(game.window)
            # If the window is closed, end the program
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    game.client.disconnect()
                    return
        except Exception as ex:
            index = index + 1
            if(index > 10):
                return

def game_loop(game, clock):
    index = 0
    while True:
        try:
            clock.tick(FPS)
            game.draw([game.player, game.opponent, game.player_score, game.opponent_score, game.ball])
            keys = pygame.key.get_pressed()
            # Breaks loop if player presses escape
            if game.playerMovementHandler(keys) is False:
                return
            game.client.send_msg("{:03d}".format(int(game.player.y)))
            game.set_game_info()
            # If the window is closed, end the program
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    game.client.disconnect()
                    return
        except Exception as ex:
            index = index + 1
            if(index > 10):
                return

# Main function
def main():
    game = Game()
    clock = pygame.time.Clock()
    start_game(game, clock)
    # game_loop(game, clock)


# Begin the program
if __name__ == '__main__':
    main()