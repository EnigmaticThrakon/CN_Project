#include <iostream>
#include <hiredis/hiredis.h>
#include "shared_files/redis_functions.h"
#include "shared_files/redis_keys.h"
#include "shared_files/timer.h"
#include "shared_files/game.h"
#include "shared_files/game_constants.h"

redis_handler *redisHandler = new redis_handler();
pong_game *gameHandler = nullptr;

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

    return;
}

int main()
{
    bool program_running = true, game_running = false;
    while(program_running)
    {
        setup_game();
        await_players();

        if(redisHandler->get_key(_game_started) == "1")
        {
            game_running = true;
        }

        while(game_running)
        {
            if(!test_player_connection(true) || !test_player_connection(false))
            {
                game_running = false;
                redisHandler->set_key(_game_started, "0");
                break;
            }
        }

    }
    std::cout << "Hello World!";
    return 0;
}