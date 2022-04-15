#ifndef REDIS_KEYS_H
#define REDIS_KEYS_H

#include <string>

std::string _left_player_connected = "left:player:connected";
std::string _right_player_connected = "right:player:connected";

std::string _left_player_response = "left:player:response";
std::string _right_player_response = "right:player:response";

std::string _left_player_update = "left:player:update";
std::string _right_player_update = "right:player:update";

std::string _left_started_received = "left:player:received";
std::string _right_started_received = "right:player:received";

std::string _game_started = "game:started";
std::string _game_setup = "game:setup";

std::string _status_update_sent = "game:update:sent";
std::string _status_update_ready = "game:update:ready";

#endif