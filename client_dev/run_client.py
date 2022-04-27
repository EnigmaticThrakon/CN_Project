from dependencies.game import Game


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