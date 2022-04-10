import pygame
import time
from resource.text import Text
from resource.button import Button
from resource.game import Game
from resource.variables import *

def chroma_color():
    while True:
        for rgb in range(0, 256, 1):
            title_text.color = (255, rgb, 0)
            start_button.color = (255, rgb, 0)
            title_text.draw(game.window)
            start_button.draw(game.window)
            pygame.display.update()
            time.sleep(.005)
        for rgb in range(255, -1, -1):
            title_text.color = (rgb, 255, 0)
            start_button.color = (rgb, 255, 0)
            title_text.draw(game.window)
            start_button.draw(game.window)
            pygame.display.update()
            time.sleep(.005)
        for rgb in range(0, 256, 1):
            title_text.color = (0, 255, rgb)
            start_button.color = (0, 255, rgb)
            title_text.draw(game.window)
            start_button.draw(game.window)
            pygame.display.update()
            time.sleep(.005)
        for rgb in range(255, -1, -1):
            title_text.color = (0, rgb, 255)
            start_button.color = (0, rgb, 255)
            title_text.draw(game.window)
            start_button.draw(game.window)
            pygame.display.update()
            time.sleep(.005)
        for rgb in range(0, 256, 1):
            title_text.color = (rgb, 0, 255)
            start_button.color = (rgb, 0, 255)
            title_text.draw(game.window)
            start_button.draw(game.window)
            pygame.display.update()
            time.sleep(.005)
        for rgb in range(255, -1, -1):
            title_text.color = (255, 0, rgb)
            start_button.color = (255, 0, rgb)
            title_text.draw(game.window)
            start_button.draw(game.window)
            pygame.display.update()
            time.sleep(.005)

def start_game(game, clock):
    # TODO: have gregg send back ack when both clients connected then show start button
    # TODO: fix latency
    input_sequence = []
    title_text = Text(75, WHITE, "\"PING\" PONG", 125, 100)
    start_text = Text(40, BLACK, "START")
    start_button = Button(400, 350, 200, 50, LIGHT_GREY, WHITE, start_text)
    while True:
        game.window.fill(BLACK)
        title_text.draw(game.window)
        # Fade title screen to black once button is clicked
        if start_button.create_button(game.window):
            for rgb in range(255, -1, -1):
                title_text.color = (rgb, rgb, rgb)
                start_button.color = (rgb, rgb, rgb)
                title_text.draw(game.window)
                start_button.draw(game.window)
                pygame.display.update()
                time.sleep(.005)
            return True
        # If the window is closed, end the program
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                game.client.disconnect()
                return False
            if event.type == pygame.KEYDOWN:
                for key, name in keys.items():
                    if event.key == key:
                        input_sequence.append(name)
                        break
        if len(input_sequence) == len(sequence):
            while True:
                for rgb in range(0, 256, 1):
                    title_text.color = (255, rgb, 0)
                    start_button.color = (255, rgb, 0)
                    title_text.draw(game.window)
                    start_button.draw(game.window)
                    pygame.display.update()
                    time.sleep(.005)
                for rgb in range(255, -1, -1):
                    title_text.color = (rgb, 255, 0)
                    start_button.color = (rgb, 255, 0)
                    title_text.draw(game.window)
                    start_button.draw(game.window)
                    pygame.display.update()
                    time.sleep(.005)
                for rgb in range(0, 256, 1):
                    title_text.color = (0, 255, rgb)
                    start_button.color = (0, 255, rgb)
                    title_text.draw(game.window)
                    start_button.draw(game.window)
                    pygame.display.update()
                    time.sleep(.005)
                for rgb in range(255, -1, -1):
                    title_text.color = (0, rgb, 255)
                    start_button.color = (0, rgb, 255)
                    title_text.draw(game.window)
                    start_button.draw(game.window)
                    pygame.display.update()
                    time.sleep(.005)
                for rgb in range(0, 256, 1):
                    title_text.color = (rgb, 0, 255)
                    start_button.color = (rgb, 0, 255)
                    title_text.draw(game.window)
                    start_button.draw(game.window)
                    pygame.display.update()
                    time.sleep(.005)
                for rgb in range(255, -1, -1):
                    title_text.color = (255, 0, rgb)
                    start_button.color = (255, 0, rgb)
                    title_text.draw(game.window)
                    start_button.draw(game.window)
                    pygame.display.update()
                    time.sleep(.005)
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
    try:
        game = Game()
        clock = pygame.time.Clock()
        if not start_game(game, clock):
            return
        game_loop(game, clock)
    except Exception as ex:
        print(ex)
        index = index + 1
        if (index > 10):
            return


# Begin the program
if __name__ == '__main__':
    main()