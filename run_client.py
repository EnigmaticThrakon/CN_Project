import pygame
import time
from resource.text import Text
from resource.button import Button
from resource.game import Game
from resource.variables import *

def start_game(game, clock):
    # TODO: have gregg send back ack when both clients connected then show start button
    # TODO: fix latency
    # Initialize start button and title
    title_text = Text(75, WHITE, "\"PING\" PONG", 125, 100)
    start_text = Text(40, BLACK, "START")
    start_button = Button(400, 350, 200, 50, LIGHT_GREY, WHITE, start_text)
    # Initialize waiting texts
    waiting_count = 0
    waiting_text_zero = Text(75, WHITE, "WAITING", 125, 100)
    waiting_text_one = Text(75, WHITE, "WAITING .", 125, 100)
    waiting_text_two = Text(75, WHITE, "WAITING . .", 125, 100)
    waiting_text_three = Text(75, WHITE, "WAITING . . .", 125, 100)
    while True:
        game.window.fill(BLACK)
        title_text.draw(game.window)
        # Fade title screen to black once button is clicked
        if start_button.create_button(game.window):
            for rgb in range(255, -1, -1):
                title_text.color = (rgb, rgb, rgb)
                title_text.draw(game.window)
                start_button.draw(game.window, (rgb, rgb, rgb))
                clock.tick(FPS)
                pygame.display.update()
                time.sleep(.005)
                # TODO: uncomment these
            game.initialize_client()
            while game.parse_response(game.client.player_screen_side)[0] != 999:
            # while True:
                game.window.fill(BLACK)
                clock.tick(FPS)
                pygame.display.update()
                if waiting_count == 0:
                    waiting_text_zero.draw(game.window)
                elif waiting_count == 1:
                    waiting_text_one.draw(game.window)
                elif waiting_count == 2:
                    waiting_text_two.draw(game.window)
                elif waiting_count == 3:
                    waiting_text_three.draw(game.window)
                    waiting_count = -1
                pygame.display.update()
                waiting_count += 1
                time.sleep(.5)
            return True
        # If the window is closed, end the program
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                game.client.disconnect()
                return False
        pygame.display.update()


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