#ifndef GAME_H
#define GAME_H

#include <string>
#include "game_constants.h"

/*
 * Class to hold the raw data of each component
 */
class game_component
{
private:
    //Variables to hold the raw data
    std::string _component_id;
    int _component_height, _component_width;

    int _location_x, _location_y, _x_velocity, _y_velocity;
    int _edge_top, _edge_bottom, _edge_right, _edge_left;

public:
    //Default constructor
    game_component() { }

    //Constructor that takes in the basic information to setup the data stored within the class
    game_component(std::string input_id, int input_height, int input_width, int _input_x, int _input_y)
    {
        //ID, height, width, and location are all set through values passed to the function
        _component_id = input_id;
        _component_height = input_height;
        _component_width = input_width;
        _location_x = _input_x;
        _location_y = _input_y;

        //Velocities are initially set to zero
        _x_velocity = _y_velocity = 0;

        //Component edges calculated through the location and component size
        _edge_top = _location_y - input_height / 2;
        _edge_bottom = _location_y + input_height;
        _edge_left = _location_x - input_width / 2;
        _edge_right = _location_x + input_width / 2;

        return;
    }

    //Default destructor
    ~game_component() { }

    //Functions to retrieve the location coordinates
    int get_location_x() { return _location_x; }
    int get_location_y() { return _location_y; }

    //Function to set the location coordiates
    void set_location(double input_x, double input_y)
    {
        //Coordinates are set through input information
        _location_x = input_x;
        _location_y = input_y;

        //Edges are recalculated using the new coordinates and component size
        _edge_top = _location_y - _component_height / 2;
        _edge_bottom = _edge_top + _component_height;
        _edge_right = _location_x + _component_width / 2;
        _edge_left = _location_x - _component_width / 2;

        return;
    }

    //Functions to retrieve the edges of the component
    int get_top_edge() { return _edge_top; }
    int get_bottom_edge() { return _edge_bottom; }
    int get_left_edge() { return _edge_left; }
    int get_right_edge() { return _edge_right; }

    //Function to set the edges if they need to be overwritten from the internal calculation
    void set_edges(int input_bottom, int input_top, int input_left, int input_right) 
    {
        _edge_bottom = input_bottom;
        _edge_top = input_top;
        _edge_right = input_right;
        _edge_left = input_left;

        return;
    }

    //Functions to retrieve the dimensions of the component
    int get_height() { return _component_height; }
    int get_width() { return _component_width; }

    //Functions to retrieve the velocities of the component
    double get_velocity_x() { return _x_velocity; }
    double get_velocity_y() { return _y_velocity; }

    //Function to set the velocities of the component
    void set_velocity(double xVel, double yVel)
    {
        _x_velocity = xVel;
        _y_velocity = yVel;

        return;
    }
};

/*
 * Class to hold basic the game data
 *
 * Performs calculations with the components to determine how the game should proceed such as
 *      where components are located, where they will be placed, and what occurs when an update takes place
 */
class pong_game
{
private:
    //Variables to hold all the components in the game
    game_component *_ball, *_left_paddle, *_right_paddle, *_window, *_play_area;    //Play area is the section of the window that the ball can travel in

    //Variables that are updated while the game is running to determine what actions to be taken
    int _left_score, _right_score, _ball_angle, _angle_counter;
    bool _wall_collision;

    //Function to determine if the ball has collided with any of the walls of the play area
    //      Returns 0 if there was a wall collision, -1 if there was a goal scored on the left side, and 1 if there was a goal scored on the right side
    int _check_ball_collision()
    {
        //Declaring and initializing a variable that will be returned
        int returnValue = 0;

        //Check if the ball collided with the top or bottom walls
        //      If so -> calculate the new ball trajectory
        //      Else -> determine if a goal has been scored

        //Instead of just returning the value once the goal is checked and the trajectory calculated,
        //      all of the if statements must be checked for the case of a ball hitting a corner of a paddle and a wall
        if(_ball->get_bottom_edge() >= _play_area->get_top_edge() && !_wall_collision)
            _calc_ball_trajectory(2);
        
        if(_ball->get_left_edge() <= _play_area->get_left_edge())
            returnValue = _check_goal();

        if(_ball->get_right_edge() >= _play_area->get_right_edge())
            returnValue = _check_goal();

        if(_ball->get_top_edge() <= _play_area->get_bottom_edge() && !_wall_collision)
            _calc_ball_trajectory(0);

        //Return the indicator
        return returnValue;
    }

    //Function to determine if the ball hits a paddle or if a goal was scored
    //
    int _check_goal()
    {
        //If the ball is on the left edge -> check to see if a goal is scored on the left side
        if(_ball->get_left_edge() <= _play_area->get_left_edge())
        {
            //Determine if the ball bottom edge is over the top edge of the paddle or if the ball top edge is below the paddle bottom edge
            if(_ball->get_bottom_edge() <= _left_paddle->get_top_edge() || _ball->get_top_edge() >= _left_paddle->get_bottom_edge())
                return -1;      //If so, then return the indicator that a goal was score on that side
            
            _calc_ball_trajectory(1);        //Otherwise calculate the ball trajectory based on the ball hitting the paddle
        }
        else if(_ball->get_right_edge() >= _play_area->get_right_edge())        //Else -> check to see if a goal is scored on the right side
        {
            //Do all the things done for the right side as above if the ball is on the right edge
            if(_ball->get_bottom_edge() <= _right_paddle->get_top_edge() || _ball->get_top_edge() >= _right_paddle->get_bottom_edge())
                return 1;

            _calc_ball_trajectory(3);
        }

        //If no goal was scored, then return 0 as an indicator
        return 0;
    }

    //Function to calculate the ball trajectory
    //      Takes in the side that the ball has collided with to determine if it's a wall or a paddle collision
    void _calc_ball_trajectory(int sideCollided)
    {
        //If the side collided is an odd number, that means it's a paddle side
        if(sideCollided % 2 == 1)
        {
            //Determine the paddle being collided with and declare a new pointer that points to the desired paddle's data
            game_component *paddle =  sideCollided == 1 ? _left_paddle : _right_paddle;

            //Determine the distance the center of the ball is from the center of the paddle with an addition 10 pixels to account for the ball's edge
            int centerOffset = paddle->get_location_y() - _ball->get_location_y() - 10;

            //Calculate the ball angle counter that will be used to determine when the y location will be updated -> make sure the value is a positive integer
            _ball_angle = (paddle->get_height() / 2) - abs(centerOffset) - 10;
            _ball_angle < 0 ? _ball_angle = abs(_ball_angle) : _ball_angle = _ball_angle;

            //Determine what the new y velocity should be based on the current y velocity and where on the paddle the ball hit
            if(_ball->get_velocity_y() > 0)
                centerOffset < 0 ? _ball->set_velocity(-1 * _ball->get_velocity_x(), _ball->get_velocity_y()) : _ball->set_velocity(-1 * _ball->get_velocity_x(), -1 * _ball->get_velocity_y());
            else
                centerOffset < 0 ? _ball->set_velocity(-1 * _ball->get_velocity_x(), -1 * _ball->get_velocity_y()) : _ball->set_velocity(-1 * _ball->get_velocity_x(), _ball->get_velocity_y());

            //Reset the angle counter and return so the wall collision code doesn't get executed
            _angle_counter = 0;
            return;
        }

        //Otherwise it's a wall collision
        //  -> Set the wall collision flag, modify the velocity
        _wall_collision = true;
        _ball->set_velocity(_ball->get_velocity_x(), -1 * _ball->get_velocity_y());     //Ball x velocity stays the same but the y velocity is inverted

        //Update the ball location to avoid an error where the ball was caught in an endless loop of
        //      indicating it hit the wall and swapping the y velocity while the ball travels outside of the play area
        _ball->set_location(_ball->get_location_x() + _ball->get_velocity_x(), _ball->get_location_y() + _ball->get_velocity_y());

        return;
    }

    //Function to determine if the paddle collided with the top or bottom wall
    //      Return true if a collision occurred, otherwise false
    bool _paddle_wall_collision(int input_y)
    {
        if(input_y > _window->get_top_edge())
            return true;
        
        if(input_y < _window->get_bottom_edge())
            return true;
        
        return false;
    }

    //Function to update the score based on an input variable then reset the game
    void _update_score(bool left)
    {
        if(left)
            _left_score++;
        else   
            _right_score++;

        _reset_game(1);
    }

    //Function to reset the game components
    void _reset_game(int indicator)
    {
        //If the indicator is 0, then there are no components that need pointers to be freed
        if(indicator != 0)
        {
            //Free all the pointers that are used for updating components
            free(_ball);
            free(_left_paddle);
            free(_right_paddle);
        }

        //Seed the random number generator to determine the initial ball angle and velocities
        std::srand(std::time(NULL));
        _ball_angle = std::rand() % 50;
        _angle_counter = 0;
        _wall_collision = false;

        //Point to a new initialization of a component for the ball
        _ball = new game_component("ball", _ball_height, _ball_width, _initial_ball_x, _initial_ball_y);
        _ball->set_velocity(std::rand() % 2 > 0 ? _initial_ball_velocity_x : _initial_ball_velocity_x * -1, std::rand() % 2 > 0 ? _initial_ball_velocity_y : _initial_ball_velocity_y * -1);

        //Point new initializations of components for the right and left paddles
        _left_paddle = new game_component("left_paddle", _paddle_height, _paddle_width, _initial_lpaddle_x, _initial_paddle_y);
        _right_paddle = new game_component("right_paddle", _paddle_height, _paddle_width, _initial_rpaddle_x, _initial_paddle_y);

        //If the indicator is even then it's a new game and score has to be reset
        if(indicator % 2 == 0)
            _right_score = _left_score = 0;

        return;
    }

    //Function to format a string so it's ready to be sent into the Redis database
    //      String variable is passed by reference to avoid declaring more variables that aren't needed
    void _format_string(std::string &input_string)
    {
        //If the string is less than three then a loop is used to add zeros to the beginning
        //      of the string so it's the correct length
        if(input_string.length() < 3)
        {
            for(int i = 0; i <= 3 - input_string.length(); i++)
                input_string = "0" + input_string;

            return;
        }

        //If the string is greater than three then only the first three characters are taken
        if(input_string.length() > 3)
            input_string = input_string.substr(0, 3);

        return;
    }

public:
    //Default constructor
    pong_game()
    {
        //Reset the game without freeing any pointers
        _reset_game(0);

        //Initialize new components to hold data on the window and the play area
        _window = new game_component("window", _screen_height, _screen_width, 0, 0);        //Window is just the height and width of the screen with a (0, 0) coordinate
        _play_area = new game_component("play_area", _screen_height, _right_paddle->get_left_edge() - _left_paddle->get_right_edge(), 0, 0);    
        //Play area is the height of the window with the space between the edges of the paddles at the (0, 0) coordinate

        //Force set the edges to make sure they are at the height and width of the window
        _window->set_edges(0, _screen_height, 0, _screen_width);

        //Force set the edges to make sure they are at the height of the window and at the edges of the paddles
        _play_area->set_edges(0, _screen_height, _left_paddle->get_right_edge(), _right_paddle->get_left_edge());
    }

    //Default deconstructor
    ~pong_game() 
    { 
        //Free all the pointers
        free(_ball);
        free(_left_paddle);
        free(_right_paddle);
        free(_window);
        free(_play_area);
    }

    //Function to update the ball location and return if a score has been made or not
    bool update_ball_location()
    {
        //Declare a variable that determines how much the ball's y value should be multiplied by
        int temp = 0;
        
        //Increase the angle counter
        _angle_counter++;
        if(_angle_counter >= _ball_angle)
        {
            //If the angle counter is greater than or equal to the ball angle then set the y velocity multiplier to 1,
            //      reset the angle counter, and remove the wall collision flag so a different wall collision can be detected

            /*
             * This is done because the y location can only be update by 1 or zero, so to have a multitude of possible angles that range from
             *      y always being updated by 1 (which equals 45 degrees) to y never being updated (which equals 0 or 180 degrees)
             * 
             * Essentially, this is creating a triangle of different size horizontal legs so the angle can be changed without having to change the 
             *      the max/min the y value can be updated by
             */

            temp = 1;
            _angle_counter = 0;
            _wall_collision = false;
        }

        //Ball location is updated based on the current ball location along with the current ball velocity and the y velocity multiplier
        _ball->set_location(_ball->get_location_x() + _ball->get_velocity_x(), _ball->get_location_y() + _ball->get_velocity_y() * temp);

        //Variable is updated to hold the value determining if a goal was scored
        temp = _check_ball_collision();

        //If the variable does not hold 0, then there was a goal
        if(temp != 0)
        {
            //Update the score based on what value the variable is holding
            _update_score(temp == 1 ? true : false);

            //Return a boolean indicating a goal was made
            return true;
        }

        return false;
    }

    //Function to update paddle location based on input value and side indicator
    void update_paddle_location(int input_y, bool left)
    {
        //Depending on the side ->
        //      Determine if the new location will collide with a wall then update the paddle to the location as close to the wall as possible
        //      Otherwise, update the paddle location normally
        if(left)
        {
            if(!_paddle_wall_collision(input_y))
                _left_paddle->set_location(_left_paddle->get_location_x(), input_y + _left_paddle->get_height() / 2);
            else
                _left_paddle->set_location(_left_paddle->get_location_x(), _play_area->get_top_edge() - _left_paddle->get_height());

            return;
        }
        
        if(!_paddle_wall_collision(input_y))
            _right_paddle->set_location(_right_paddle->get_location_x(), input_y + _right_paddle->get_height() / 2);
        else
            _right_paddle->set_location(_right_paddle->get_location_x(), _play_area->get_top_edge() - _right_paddle->get_height());

        return;
    }

    //Function to return if a game has been won
    int has_won()
    {
        //Return -1 if left has won, or 1 if right has won
        if(_left_score >= _win)
            return -1;
        
        if(_right_score >= _win)
            return 1;

        return 0;
    }

    //Region of functions to get the values that will be passed into the Redis database
    std::string get_lpaddle_location()
    {
        std::string returnValue = std::to_string(_left_paddle->get_top_edge());
        _format_string(returnValue);

        return returnValue;
    }

    std::string get_rpaddle_location()
    {
        std::string returnValue = std::to_string(_right_paddle->get_top_edge());
        _format_string(returnValue);

        return returnValue;
    }

    std::string get_ball_location_x()
    {
        std::string returnValue = std::to_string(_ball->get_left_edge());
        _format_string(returnValue);

        return returnValue;
    }

    std::string get_ball_location_y()
    {
        std::string returnValue = std::to_string(_ball->get_top_edge());
        _format_string(returnValue);

        return returnValue;
    }

    std::string get_left_score()
    {
        std::string returnValue = std::to_string(_left_score);
        _format_string(returnValue);

        return returnValue;
    }

    std::string get_right_score()
    {
        std::string returnValue = std::to_string(_right_score);
        _format_string(returnValue);

        return returnValue;
    }
    //End Region
};

#endif