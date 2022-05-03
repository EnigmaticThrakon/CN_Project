from dependencies.TCP_game import Game  #This can take the UDP_game to run with the UDP protocol


# Main function
def main():
    # Initialize the game
    game = Game()
    if game.start():
        game.client.disconnect()
        return
    while True:
        if game.loop():
            break
        if game.end():
            break
    game.client.disconnect()

# Begin the program
if __name__ == '__main__':
    main()