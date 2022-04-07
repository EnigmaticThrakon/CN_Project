import pygame
from resource.text import Text
from resource.button import Button
from resource.game import Game
from resource.variables import *

def start_game(game, clock):
    title_text = Text(200, WHITE, "PONG", 125, 100)
    start_text = Text(40, BLACK, "START")
    start_button = Button(400, 350, 200, 50, LIGHT_GREY, WHITE, start_text)
    while True:
        game.window.fill(BLACK)
        title_text.draw(game.window)
        if start_button.create_button(game.window):
            return True
        # If the window is closed, end the program
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                game.client.disconnect()
                return False
        pygame.display.update()
        clock.tick(FPS)


def game_loop(game, clock):
    while True:
        clock.tick(FPS)
        game.draw([game.player, game.opponent, game.player_score, game.opponent_score, game.ball])
        keys = pygame.key.get_pressed()
        # Breaks loop if player presses escape
        if game.playerMovementHandler(keys) is False:
            return
        game.client.send_msg("{:03d}".format(int(game.player.y)))
        game.set_game_info(game.parse_response(game.client.recv_msg()))
        # If the window is closed, end the program
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                game.client.disconnect()
                return


# Main function
def main():
    index = 0
    # try:
    game = Game()
    clock = pygame.time.Clock()
    if not start_game(game, clock):
        return
    game_loop(game, clock)
    # except Exception as ex:
    #     print(ex)
    #     index = index + 1
    #     if (index > 10):
    #         return


# Begin the program
if __name__ == '__main__':
    main()