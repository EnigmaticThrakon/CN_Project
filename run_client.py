import pygame
import time
from client_dev.objects.text import Text
from client_dev.objects.button import Button
from client_dev.game import Game
from client_dev.variables import *


def start_game(game):
    # Initialize start button, title, and background music
    pygame.mixer.music.load("intro.mp3")
    pygame.mixer.music.set_volume(1)
    pygame.mixer.music.play(-1)
    title_text = Text(75, WHITE, "''PING'' PONG", 100, 100)
    start_text = Text(40, BLACK, "START")
    start_button = Button(400, 350, 200, 50, LIGHT_GREY, WHITE, start_text)
    game.window.fill(BLACK)
    # Initialize waiting texts
    waiting_count = 0
    waiting_text_zero = Text(75, WHITE, "WAITING", 125, 100)
    waiting_text_one = Text(75, WHITE, "WAITING .", 125, 100)
    waiting_text_two = Text(75, WHITE, "WAITING . .", 125, 100)
    waiting_text_three = Text(75, WHITE, "WAITING . . .", 125, 100)

    # While both players are not connected
    while True:
        # If the window is closed, end the program
        if game.check_exit():
            return True
        # Draw the title screen
        title_text.draw(game.window)
        pygame.display.update()
        # Fade title screen to black once button is clicked
        if start_button.create_button(game.window):
            # Fade title to black and background music upon starting the game
            pygame.mixer.music.fadeout(1275)
            for rgb in range(255, -1, -1):
                title_text.color = (rgb, rgb, rgb)
                title_text.draw(game.window)
                start_button.draw(game.window, (rgb, rgb, rgb))
                pygame.display.update()
                time.sleep(.005)
            # Clear the window
            game.window.fill(BLACK)
            pygame.display.update()
            # Initialize the client
            game.initialize_client()
            response = None
            game.client.set_blocking(0)
            while response != 999:
                # If the window is closed, end the program
                if game.check_exit():
                    return True
                # Check server for response
                response = game.parse_response(game.client.recv_msg())[0]
                # Clear the window
                game.window.fill(BLACK)
                # Draw waiting text to screen
                if waiting_count == 0:
                    waiting_text_zero.draw(game.window)
                elif waiting_count == 1:
                    waiting_text_one.draw(game.window)
                elif waiting_count == 2:
                    waiting_text_two.draw(game.window)
                elif waiting_count == 3:
                    waiting_text_three.draw(game.window)
                    waiting_count = -1
                waiting_count += 1
                pygame.display.update()
                time.sleep(.5)
            game.client.set_blocking(1)
            # Send connection acknowledgement
            game.client.send_msg("999")
            return


def game_loop(game):
    while True:
        # If the window is closed, end the program
        if game.check_exit():
            return True
        game.draw([game.player, game.opponent, game.player_score, game.opponent_score, game.ball])
        game.playerMovementHandler(pygame.key.get_pressed())
        game.client.send_msg("{:03d}".format(int(game.player.y)))
        game.set_game_info(game.parse_response(game.client.recv_msg()))
        # Check for winner
        if game.player_score.score == 5 or game.opponent_score.score == 5:
            game.draw([game.player, game.opponent, game.player_score, game.opponent_score, game.ball])
            if game.player_score.score == 5:
                game.winner = True
            return

def end_game(game):
    # Decided who the winner is and initialize text and button objects
    if game.winner:
        winner_text = Text(75, WHITE, "WINNER", 400, 300)
    else:
        winner_text = Text(75, WHITE, "LOSER", 125, 100)
    play_again_text = Text(40, BLACK, "PLAY\nAGAIN?")
    play_again_button = Button(400, 450, 200, 100, LIGHT_GREY, WHITE, play_again_text)
    seconds_left = 10
    seconds_left_text = Text(75, WHITE, f"{seconds_left}s", 500, 450)
    _start = time.perf_counter()
    while seconds_left > 0:
        # If the window is closed, end the program
        if game.check_exit():
            return True
        if play_again_button.create_button(game.window):
            return False
        _start = time.perf_counter()
        if time.perf_counter() - _start > 1:
            seconds_left -= 1
            seconds_left_text = Text(75, WHITE, f"{seconds_left}s", 500, 450)
            _start = time.perf_counter()
        # Clear the window
        game.window.fill(BLACK)
        game.draw([winner_text, seconds_left_text])
        pygame.display.update()
    return True


# Main function
def main():
    # Initialize the game
    game = Game()
    if start_game(game):
        game.client.disconnect()
        return
    while True:
        if game_loop(game):
            break
        if end_game(game):
            break
    game.client.disconnect()

# Begin the program
if __name__ == '__main__':
    main()