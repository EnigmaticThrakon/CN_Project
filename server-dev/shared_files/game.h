#ifndef GAME_H
#define GAME_H

#include <string>
#include "game_constants.h"

class game_component
{
private:
    std::string _component_id;
    int _location_x;
    int _location_y;
    int _component_height;
    int _component_width;
public:
    game_component()
    {
        _component_id = "";
        _location_x = _location_y = _component_height = _component_width = 0;
    }

    game_component(std::string input_id)
    {
        game_component();
        _component_id = input_id;
    }

    game_component(std::string input_id, int input_height, int input_width)
    {
        game_component(input_id);
        _component_height = input_height;
        _component_width = input_width;
        _location_x = _location_y = 0;
    }

    ~game_component() { }

    int get_location_x() { return _location_x; }
    int get_location_y() { return _location_y; }
    void set_location_x(int input_x) { _location_x = input_x; }
    void set_location_y(int input_y) { _location_y = input_y; }
};

class pong_game
{
private:
    game_component _ball, _left_paddle, _right_paddle;
    int _left_score, _right_score, _window_width, _window_height;
public:
    pong_game()
    {
        _left_score = _right_score = 0;
        _ball = game_component("ball", _ball_y, _ball_x);
        _left_paddle = game_component("left_paddle", _paddle_y, _paddle_x);
        _right_paddle = game_component("right_paddle", _paddle_y, _paddle_x);
    }

    ~pong_game() { }
};

#endif