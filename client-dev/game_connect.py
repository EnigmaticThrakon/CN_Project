from client import Client
from game import Game
import threading

client = Client()
game = Game(client)

# t1 = threading.Thread(target=game.set_game_info())
# t2 = threading.Thread(target=game.player_move())
#
# t1.start()
# t2.start()

# game.set_game_info()
try:
    t1 = threading.Thread(target=game.game_loop)
    t1.start()
except:
    client.disconnect()



# TODO: read server data, thread player and reader