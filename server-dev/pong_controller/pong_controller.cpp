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
        std::cout << "Clearing Previous Game" << std::endl;
        free(gameHandler);
        gameHandler = nullptr;
    }

    std::cout << "Setting Up New Game Handler" << std::endl;
    gameHandler = new pong_game();
}

bool test_player_connection(bool right)
{
    if(right)
    {
        return (redisHandler->get_key(_right_player_connected) == "1");
    }

    return (redisHandler->get_key(_left_player_connected) == "1");
}

std::string get_response_string(std::string formattedString)
{
    for(int i = 0; i < 5; i++)
        formattedString = formattedString + formattedString;
    
    return formattedString;
}

void await_players()
{
    bool playersConnected = false;
    std::cout << "Awaiting Players to Connect" << std::endl;

    while(!playersConnected)
    {
        playersConnected = test_player_connection(true);
        //playersConnected = playersConnected && test_player_connection(false);
    }

    redisHandler->set_key(_game_setup, "1");

    std::cout << "Both Players Connected" << std::endl;
    snprintf(_status_buffer, sizeof(_status_buffer), responseFormat.c_str(), "999", "999", "999", "999", "999", "999");
    redisHandler->set_key(_right_player_update, get_response_string(_status_buffer));
    redisHandler->set_key(_left_player_update, get_response_string(_status_buffer));

    //while(redisHandler->get_key(_left_started_received) != "1" && redisHandler->get_key(_right_started_received) != "1") { }
    while(redisHandler->get_key(_right_started_received) != "1") { }
    std::cout << "Players Acknowledged Game Starting" << std::endl;

    return;
}

void update_game_status()
{
    snprintf(_status_buffer, sizeof(_status_buffer), responseFormat.c_str(), gameHandler->get_lpaddle_location().c_str(), gameHandler->get_rpaddle_location().c_str(),
            gameHandler->get_ball_location_x().c_str(), gameHandler->get_ball_location_y().c_str(), gameHandler->get_left_score().c_str(), gameHandler->get_right_score().c_str());

    std::cout << _status_buffer << std::endl;

    redisHandler->set_key(_right_player_update, get_response_string(_status_buffer));
    redisHandler->set_key(_left_player_update, get_response_string(_status_buffer));
}

int main()
{
    bool program_running = true, game_running = false;
    update_timer *timer = new update_timer();
    while(program_running)
    {
        setup_game();
        await_players();

        std::cout << "Game Setup Complete" << std::endl;
        game_running = true;
        redisHandler->set_key(_left_player_response, std::to_string(_initial_lpaddle_y));
        redisHandler->set_key(_right_player_response, std::to_string(_initial_rpaddle_y));
        redisHandler->set_key(_game_started, "1");

        globalTimer->reset();
        timer->reset();
        std::cout << "Game Started" << std::endl;

        std::string redisData = "";
        while(game_running)
        {
            while(redisHandler->get_key(_status_update_sent) != "1") { }

            redisHandler->set_key(_status_update_sent, "0");
            // gameHandler->update_paddle_location(stoi(redisHandler->get_key(_left_player_response)), true);

            for(int i = 0; i < 4; i++)
            {
                redisData = redisHandler->get_key(_right_player_response);
                if(redisData != "")
                    gameHandler->update_paddle_location(stoi(redisHandler->get_key(_right_player_response)), false);

                if(gameHandler->update_ball_location())
                {
                    globalTimer->reset();
                    update_game_status();
                    while(globalTimer->elapsed_time() < 5e9) { }
                    break;
                }
                else
                {
                    update_game_status();
                }
            }

            if (gameHandler->has_won() != 0)
            {
                game_running = false;
                redisHandler->set_key(_game_started, "0");
            }
        }
        break;

    }
    return 0;
}