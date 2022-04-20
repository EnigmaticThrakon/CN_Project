#ifndef REDIS_KEYS_H
#define REDIS_KEYS_H

//Header file used to hold key constants for the Redis database to help
//      prevent errors from attempting to type out the literal string every
//      time it is needed

#include <string>

//Pair of keys used to determine if the left/right player has connected to 
//      the server and have a designated socket
std::string _left_player_connected = "left:player:connected";
std::string _right_player_connected = "right:player:connected";

//Pair of keys used to hold the data that is sent to the server from the 
//      left/right player
std::string _left_player_response = "left:player:response";
std::string _right_player_response = "right:player:response";

//Pair of keys used to hold the data that will be sent to the right/left
//      player from the server
std::string _left_player_update = "left:player:update";
std::string _right_player_update = "right:player:update";

//Pair of keys used to hold flags that determine if the right/left player
//      has returned a confirmation showing they know the game is starting
std::string _left_started_received = "left:player:received";
std::string _right_started_received = "right:player:received";

//Pair of keys used to hold flags showing that the game has been setup and started
std::string _game_started = "game:started";
std::string _game_setup = "game:setup";

//Pair of keys used to hold flags that throttle the game/server by allowing them
//      to communicate based on if the data is ready to send and if it's sent
std::string _status_update_sent = "game:update:sent";
std::string _status_update_ready = "game:update:ready";

std::string _game_program_running = "game:program:running";

#endif