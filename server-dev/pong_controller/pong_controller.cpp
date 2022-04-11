#include <iostream>
#include <hiredis/hiredis.h>
#include "../shared_files/redis_functions.h"
#include "../shared_files/redis_keys.h"
#include "../shared_files/timer.h"
#include "../shared_files/game.h"
#include "../shared_files/game_constants.h"

redis_handler *redisHandler = new redis_handler();
pong_game *gameHandler = nullptr;
update_timer* globalTimer = new update_timer();
char _status_buffer[38];

void setup_game()
{
    if(gameHandler != nullptr)
    {
        free(gameHandler);
        gameHandler = nullptr;
    }

    gameHandler = new pong_game();
    redisHandler->set_key(_game_setup, "1");
}

bool test_player_connection(bool right)
{
    if(right)
    {
        return (redisHandler->get_key(_right_player_connected) == "1");
    }

    return (redisHandler->get_key(_left_player_connected) == "1");
}

void await_players()
{
    bool playersConnected = false;
    while(!playersConnected)
    {
        playersConnected = test_player_connection(true);
        playersConnected = playersConnected && test_player_connection(false);
    }

    snprintf(_status_buffer, sizeof(_status_buffer), responseFormat.c_str(), "999", "999", "999", "999", "999", "999");

    return;
}

std::string get_response_string(std::string formattedString)
{
    for(int i = 0; i < 5; i++)
        formattedString = formattedString + formattedString;
    
    return formattedString;
}

void update_game_status()
{
    snprintf(_status_buffer, sizeof(_status_buffer), responseFormat.c_str(), gameHandler->get_lpaddle_location().c_str(), gameHandler->get_rpaddle_location().c_str(),
            gameHandler->get_ball_location_x().c_str(), gameHandler->get_ball_location_y().c_str(), gameHandler->get_left_score().c_str(), gameHandler->get_right_score().c_str());

    std::cout << _status_buffer << std::endl;

    redisHandler->set_key(_game_status, get_response_string(_status_buffer));
}

int main()
{
    bool program_running = true, game_running = false;
    while(program_running)
    {
        setup_game();
        await_players();

        if(redisHandler->get_key(_game_setup) == "1")
        {
            game_running = true;
            redisHandler->set_key(_left_player_response, std::to_string(_initial_lpaddle_y));
            redisHandler->set_key(_right_player_response, std::to_string(_initial_rpaddle_y));
            redisHandler->set_key(_game_started, "1");
        }

        globalTimer->reset();
        while(game_running)
        {
            gameHandler->update_paddle_location(stoi(redisHandler->get_key(_left_player_response)), true);
            gameHandler->update_paddle_location(stoi(redisHandler->get_key(_right_player_response)), false);

            if(globalTimer->elapsed_time() > 100)
            {
                globalTimer->reset();
                if(gameHandler->update_ball_location())
                {
                    update_game_status();
                    while(globalTimer->elapsed_time() < 1000) { }
                }
            }

            update_game_status();

            if(gameHandler->has_won() != 0)
            {
                game_running = false;
                redisHandler->set_key(_game_started, "0");
            }

            //if(!test_player_connection(true) || !test_player_connection(false))
            //{
            //    game_running = false;
            //    redisHandler->set_key(_game_started, "0");
            //    break;
            //}
        }
        break;

    }
    return 0;
}