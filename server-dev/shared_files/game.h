#ifndef GAME_H
#define GAME_H

#include <string>
#include "game_constants.h"

struct rectangle
{
    int _center_x;
    int _center_y;
    int _height;
    int _width;

    rectangle() { }

    rectangle(int input_height, int input_width)
    {
        _height = input_height;
        _width = input_width;

        _center_x = _width / 2;
        _center_y = _height / 2;
    }
};

class game_component
{
private:
    std::string _component_id;
    rectangle _component;
    int _location_x, _previous_location_x;
    int _location_y, _previous_location_y;
    int _edge_top, _edge_bottom, _edge_right, _edge_left;
    double _component_trajectory;
    int _component_intercept;
    bool _component_direction;    //True = moving right or up, False = moving left or down

    void compute_trajectory()
    {
        int numerator = _location_y - _previous_location_y;
        int denominator = _location_x - _previous_location_x;

        if(denominator != 0)
        {
            _component_direction = (denominator >= 0);

            double slope = double(numerator) / double(denominator);
            _component_intercept = _location_y + _component_trajectory * _location_x;

            return;
        }

        _component_trajectory = double(numerator);
        _component_direction = (_component_trajectory >= 0);
        _component_intercept = 0;
        return;
    }
public:
    game_component() { }

    game_component(std::string input_id, int input_height, int input_width, int _input_x, int _input_y)
    {
        _component_id = input_id;
        _component = rectangle(input_height, input_width);
        _location_x = _previous_location_x = _input_x;
        _location_y = _previous_location_y = _input_y;
        _component_trajectory = 0;
        _component_intercept = 0;

        _edge_top = _location_y + input_height / 2;
        _edge_bottom = _location_y - input_height / 2;
        _edge_left = _location_x - input_width / 2;
        _edge_right = _location_x + input_width / 2;
    }

    ~game_component() { }

    int get_location_x() { return _location_x; }
    int get_location_y() { return _location_y; }

    int get_top_edge() { return _edge_top; }
    int get_bottom_edge() { return _edge_bottom; }
    int get_left_edge() { return _edge_left; }
    int get_right_edge() { return _edge_right; }

    bool get_direction() { return _component_direction; }
    double get_trajectory() { return _component_trajectory; }
    int get_intercept() { return _component_intercept; }

    void set_location(int input_x, int input_y, bool bounce)
    {
        _previous_location_x = _location_x;
        _previous_location_y = _location_y;

        _location_x = input_x;
        _location_y = input_x;

        if(bounce)
            compute_trajectory();
    }
};

class pong_game
{
private:
    game_component *_ball, *_left_paddle, *_right_paddle, *_window, *_play_area;
    int _left_score, _right_score, _window_width, _window_height;
    bool _bounce;

    bool ball_collision(int input_x, int input_y)
    {
        if(_ball->get_top_edge() + input_y >= _play_area->get_top_edge())
            return true;
        
        if(_ball->get_left_edge() + input_x <= _play_area->get_left_edge())
            return true;

        if(_ball->get_right_edge() + input_x >= _play_area->get_right_edge())
            return true;

        if(_ball->get_bottom_edge() + input_y <= _play_area->get_bottom_edge())
            return true;

        return false;
    }

    int check_goal(int input_x)
    {
        if(_ball->get_left_edge() + input_x <= _play_area->get_left_edge())
            return -1;
        
        if(_ball->get_right_edge() + input_x >= _play_area->get_right_edge())
            return 1;

        return 0;
    }

    bool paddle_collision(int input_y)
    {
        if(input_y > _window->get_top_edge())
            return true;
        
        if(input_y < _window->get_bottom_edge())
            return true;
        
        return false;
    }

    void update_score(bool left)
    {
        if(left)
            _left_score++;
        else   
            _right_score++;

        reset_game();
    }

    void reset_game()
    {
        free(_ball);
        free(_left_paddle);
        free(_right_paddle);

        _ball = new game_component("ball", _ball_height, _ball_width, _initial_ball_x, _initial_ball_y);
        _left_paddle = new game_component("left_paddle", _paddle_height, _paddle_width, _initial_lpaddle_x, _initial_lpaddle_y);
        _right_paddle = new game_component("right_paddle", _paddle_height, _paddle_width, _initial_rpaddle_x, _initial_rpaddle_y);

        _bounce = false;
    }


public:
    pong_game()
    {
        _bounce = false;
        _left_score = _right_score = 0;
        _ball = new game_component("ball", _ball_height, _ball_width, _initial_ball_x, _initial_ball_y);
        _left_paddle = new game_component("left_paddle", _paddle_height, _paddle_width, _initial_lpaddle_x, _initial_lpaddle_y);
        _right_paddle = new game_component("right_paddle", _paddle_height, _paddle_width, _initial_rpaddle_x, _initial_rpaddle_y);

        _window = new game_component("window", _screen_height, _screen_width, 0, 0);
        
        _play_area = new game_component("play_area", _screen_height, _right_paddle->get_left_edge() - _left_paddle->get_right_edge(), 0, 0);
    }

    ~pong_game() 
    { 
        free(_ball);
        free(_left_paddle);
        free(_right_paddle);
        free(_window);
        free(_play_area);
    }

    void update_ball_location()
    {
        int component_x = 0, component_y = 0;
        
        if(_ball->get_direction())
            component_x = _ball->get_location_x() + 1;
        else
            component_x = _ball->get_location_x() - 1;

        component_y = _ball->get_location_y() + _ball->get_trajectory();
        bool collision = ball_collision(_ball->get_direction() ? 1 : -1, _ball->get_trajectory() > 0 ? 1 : -1);

        if(collision)
        {
            int goal_side = check_goal(_ball->get_direction() ? 1 : -1);
            if(goal_side != 0)
            {
                update_score(goal_side < 0);
                return;
            }
        }

        _ball->set_location(component_x, component_y, collision);
        return;
    }

    void update_paddle_location(int input_y, bool left)
    {
        if(left)
        {
           if(!paddle_collision(input_y))
           {
                _left_paddle->set_location(0, input_y, false);
           }
            return;
        }
        
        if(!paddle_collision(input_y))
        {
            _right_paddle->set_location(0, input_y, false);
        }
        return;
    }
};

#endif