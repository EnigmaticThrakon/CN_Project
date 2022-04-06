#ifndef REDIS_KEYS_H
#define REDIS_KEYS_H

#include <string>

std::string _window_size = "window:size";
std::string _paddle_size = "paddle:size";
std::string _ball_size = "ball:size";

std::string _ball_location = "ball:location";
std::string _paddle_location = "paddle_location";

std::string _player_connected = "player:connected";
std::string _player_transmitting = "player:transmitting";
std::string _player_response = "player:response";

std::string _test_response = "test:response";

std::string _left_player_connected = "left:player:connected";
std::string _right_player_connected = "right:player:connected";

std::string _game_started = "game:started";
std::string _game_setup = "game:setup";

#endif