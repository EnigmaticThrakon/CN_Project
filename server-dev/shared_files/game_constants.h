#ifndef GAME_CONSTANTS_H
#define GAME_CONSTANTS_H

//Header file that stores basic constants of the game
//      to allow for easier access and manipulation of global
//      values that are the basis for the game and communication

//Pair of variables that define the window dimensions
int _screen_height = 780;
int _screen_width = 1000;

//Variables that determine the dimensions of the paddle components
//      and their initial locations
int _paddle_height = 100;
int _paddle_width = 20;
int _initial_paddle_y = _screen_height / 2;
int _initial_lpaddle_x = 20;
int _initial_rpaddle_x = 970;

//Variables that determine the dimensions of the ball component
//      and the ball's initial location along with velocities
int _ball_height = 10;
int _ball_width = 10;
int _initial_ball_velocity_x = 1;
int _initial_ball_velocity_y = 1;
int _initial_ball_x = _screen_width / 2;
int _initial_ball_y = _screen_height / 2;

//Standardized response format for all the information that is 
//      sent to the clients from the server
std::string responseFormat = "<%s,%s:%s,%s:%s,%s>";

//Standardized console output for when users connect or disconnect
std::string newConnectionFormat = "New User Connected\n%s Player\nSocket FD is %d\nIP : %s:%d\n";
std::string disconnectionFormat = "User Disconnected\n%s Player\nSocket FD is %d\nIP : %s:%d\n";

//Variable that determines the score before the game completes
int _win = 5;

#endif