#ifndef GAME_CONSTANTS_H
#define GAME_CONSTANTS_H

int _screen_height = 800;
int _screen_width = 1000;

int _rscore_x = 520;
int _lscore_x = 470;

int _paddle_height = 10;
int _paddle_width = 10;
int _initial_lpaddle_x = 10;
int _initial_lpaddle_y = _screen_height / 2;
int _initial_rpaddle_x = 970;
int _initial_rpaddle_y = _screen_height / 2;

int _ball_height = 10;
int _ball_width = 10;
int _initial_ball_velocity_x = 1;
int _initial_ball_velocity_y = 1;
int _initial_ball_x = _screen_width / 2;
int _initial_ball_y = _screen_height / 2;

std::string responseFormat = "<%s,%s:%s,%s:%s,%s>";

int _win = 5;

#endif