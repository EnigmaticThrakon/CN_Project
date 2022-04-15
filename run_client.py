import pygame
import time
from client_dev.objects.text import Text
from client_dev.objects.button import Button
from client_dev.game import Game
from client_dev.variables import *

def start_game(game):
    # Initialize start button and title
    volume = 0
    pygame.mixer.music.load("intro.mp3")
    pygame.mixer.music.set_volume(volume)
    pygame.mixer.music.play(-1)
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
            pygame.mixer.music.fadeout(1275)
            for rgb in range(255, -1, -1):
                title_text.color = (rgb, rgb, rgb)
                title_text.draw(game.window)
                start_button.draw(game.window, (rgb, rgb, rgb))
                pygame.display.update()
                time.sleep(.005)
            game.initialize_client()
            response = ""
            while response != 999:
                response = game.parse_response(game.client.recv_msg())[0]
                game.window.fill(BLACK)
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
            game.client.send_msg("999")
            return True
        # If the window is closed, end the program
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                game.client.disconnect()
                return False
        pygame.display.update()
        while volume < 1.0:
            pygame.mixer.music.set_volume(volume)
            volume += 0.01
            time.sleep(.005)


def game_loop(game):
    while True:
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
        if not start_game(game):
            return
        game_loop(game)
    except Exception as ex:
        print(ex)
        index = index + 1
        if (index > 10):
            return


# Begin the program
if __name__ == '__main__':
    main()