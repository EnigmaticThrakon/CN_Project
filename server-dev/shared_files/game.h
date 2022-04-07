#ifndef GAME_H
#define GAME_H

#include <string>
#include "game_constants.h"

class game_component
{
private:
    std::string _component_id;
    int _component_height, _component_width;
    int _location_x, _previous_location_x;
    int _location_y, _previous_location_y;
    int _edge_top, _edge_bottom, _edge_right, _edge_left;
    int _x_velocity, _y_velocity;
public:
    game_component() { }
    game_component(std::string input_id, int input_height, int input_width, int _input_x, int _input_y)
    {
        _component_id = input_id;
        _component_height = input_height;
        _component_width = input_width;
        _location_x = _previous_location_x = _input_x;
        _location_y = _previous_location_y = _input_y;
        _x_velocity = _y_velocity = 0;

        _edge_top = _location_y + input_height / 2;
        _edge_bottom = _location_y - input_height / 2;
        _edge_left = _location_x - input_width / 2;
        _edge_right = _location_x + input_width / 2;
    }
    ~game_component() { }

    int get_location_x() { return _location_x; }
    int get_location_y() { return _location_y; }
    void set_location(int input_x, int input_y, bool ball)
    {
        _previous_location_x = _location_x;
        _previous_location_y = _location_y;

        _location_x = input_x;
        _location_y = input_x;

        if(!ball)
        {
            _x_velocity = _location_x - _previous_location_x;
            _y_velocity = _location_y - _previous_location_y;
        }
    }

    int get_top_edge() { return _edge_top; }
    int get_bottom_edge() { return _edge_bottom; }
    int get_left_edge() { return _edge_left; }
    int get_right_edge() { return _edge_right; }

    int get_height() { return _component_height; }
    int get_width() { return _component_width; }

    int get_velocity_x() { return _x_velocity; }
    int get_velocity_y() { return _y_velocity; }
    void set_velocity(int xVel, int yVel)
    {
        _x_velocity = xVel;
        _y_velocity = yVel;
    }
};

class pong_game
{
private:
    game_component *_ball, *_left_paddle, *_right_paddle, *_window, *_play_area;
    int _left_score, _right_score, _win_point;

    void check_ball_collision()
    {
        if(_ball->get_top_edge() >= _play_area->get_top_edge())
            calc_ball_trajectory(2);
        
        if(_ball->get_left_edge() <= _play_area->get_left_edge())
            check_goal();

        if(_ball->get_right_edge() >= _play_area->get_right_edge())
            check_goal();


        if(_ball->get_bottom_edge()<= _play_area->get_bottom_edge())
            calc_ball_trajectory(0);

        return;
    }

    int check_goal()
    {
        if(_ball->get_left_edge() <= _play_area->get_left_edge())
        {
            if(_ball->get_bottom_edge() >= _left_paddle->get_top_edge() || _ball->get_top_edge() <= _left_paddle->get_bottom_edge())
            {
                return -1;
            }
            
            calc_ball_trajectory(1);
        }
        else if(_ball->get_right_edge() >= _play_area->get_right_edge())
        {
            if(_ball->get_bottom_edge() >= _right_paddle->get_top_edge() || _ball->get_top_edge() <= _right_paddle->get_bottom_edge())
            {
                return 1;
            }

            calc_ball_trajectory(3);
        }

        return 0;
    }

    void calc_ball_trajectory(int sideCollided)
    {
        if(sideCollided % 2 == 1)
        {
            game_component *paddle =  sideCollided == 1 ? _left_paddle : _right_paddle;
            double centerOffset = paddle->get_location_y() - _ball->get_location_y();

            _ball->set_velocity(-1 * _ball->get_velocity_x(), (int)centerOffset);
            return;
        }

        _ball->set_velocity(_ball->get_velocity_x(), -1 * _ball->get_velocity_y());
        return;
    }

    bool paddle_wall_collision(int input_y)
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

    void format_string(std::string &input_string)
    {
        if(input_string.length() < 3)
        {
            for(int i = 0; i < 3 - input_string.length(); i++)
                input_string = "0" + input_string;

            return;
        }

        if(input_string.length() > 3)
            input_string = input_string.substr(0, 3);

        return;
    }
public:
    pong_game()
    {
        _win_point = _win;
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
        
        _ball->set_location(_ball->get_location_x() + _ball->get_velocity_x(), _ball->get_location_y() + _ball->get_velocity_y(), true);
        check_ball_collision();
        return;
    }

    void update_paddle_location(int input_y, bool left)
    {
        if(left)
        {
           if(!paddle_wall_collision(input_y))
                _left_paddle->set_location(_left_paddle->get_location_x(), input_y, false);
           else
                _left_paddle->set_location(_left_paddle->get_location_x(), _play_area->get_top_edge() - _left_paddle->get_height(), false);
            return;
        }
        
        if(!paddle_wall_collision(input_y))
            _right_paddle->set_location(_right_paddle->get_location_x(), input_y, false);
        else
            _right_paddle->set_location(_right_paddle->get_location_x(), _play_area->get_top_edge() - _right_paddle->get_height(), false);
        return;
    }

    std::string get_lpaddle_location()
    {
        std::string returnValue = std::to_string(_left_paddle->get_location_y());
        format_string(returnValue);

        return returnValue;
    }

    std::string get_rpaddle_location()
    {
        std::string returnValue = std::to_string(_right_paddle->get_location_y());
        format_string(returnValue);

        return returnValue;
    }

    std::string get_ball_location_x()
    {
        std::string returnValue = std::to_string(_ball->get_location_x());
        format_string(returnValue);

        return returnValue;
    }

    std::string get_ball_location_y()
    {
        std::string returnValue = std::to_string(_ball->get_location_y());
        format_string(returnValue);

        return returnValue;
    }

    std::string get_left_score()
    {
        std::string returnValue = std::to_string(_left_score);
        format_string(returnValue);

        return returnValue;
    }

    std::string get_right_score()
    {
        std::string returnValue = std::to_string(_right_score);
        format_string(returnValue);

        return returnValue;
    }

    int has_won()
    {
        if(_left_score >= _win_point)
            return -1;
        
        if(_right_score >= _win_point)
            return 1;

        return 0;
    }
};

#endif