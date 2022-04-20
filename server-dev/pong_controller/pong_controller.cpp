#include <iostream>
#include <hiredis/hiredis.h>
#include "../shared_files/redis_functions.h"
#include "../shared_files/redis_keys.h"
#include "../shared_files/timer.h"
#include "../shared_files/game.h"
#include "../shared_files/game_constants.h"

redis_handler *redisHandler = new redis_handler();
pong_game *gameHandler = new pong_game();
update_timer *globalTimer = new update_timer();
char _status_buffer[38];

// Function to return an indicator if a player is connected
//       Returns true if the player is connected or false is not
//       If the indicator is true then the right player is tested, otherwise test the left player
bool test_player_connection(bool right)
{
    if (right)
        return (redisHandler->get_key(_right_player_connected) == "1");

    return (redisHandler->get_key(_left_player_connected) == "1");
}

// Function create the string that will be sent to the client
std::string get_response_string(std::string formattedString)
{
    // Repeats the formatted strin five times, then returns it
    for (int i = 0; i < 5; i++)
        formattedString = formattedString + formattedString;

    return formattedString;
}

// Function to block and wait for both players to connect
void await_players()
{
    bool playersConnected = false;
    std::cout << "Awaiting Players to Connect" << std::endl;

    // Remain in loop until both players have been connected and the indicators have been set
    while (!playersConnected)
    {
        playersConnected = test_player_connection(true);
        playersConnected = playersConnected && test_player_connection(false);
    }

    // When both players are connected, the game setup flag is set
    redisHandler->set_key(_game_setup, "1");

    std::cout << "Both Players Connected" << std::endl;

    // Create a response string that contains all 9's to let the clients know the game is starting
    snprintf(_status_buffer, sizeof(_status_buffer), responseFormat.c_str(), "999", "999", "999", "999", "999", "999");

    // Update the status key for the left and right players with the game starting response so it will be sent to the clients
    redisHandler->set_key(_right_player_update, get_response_string(_status_buffer));
    redisHandler->set_key(_left_player_update, get_response_string(_status_buffer));

    // Wait until the flags indicating both players know the game is start have been set
    while (redisHandler->get_key(_left_started_received) != "1" && redisHandler->get_key(_right_started_received) != "1")
    {
    }
    std::cout << "Players Acknowledged Game Starting" << std::endl;

    // Return to let the game start
    return;
}

// Function to compile the string that will be used to send to the clients
void update_game_status()
{
    // Creat  the string based on locations of the ball and paddles, along with the current score
    snprintf(_status_buffer, sizeof(_status_buffer), responseFormat.c_str(), gameHandler->get_lpaddle_location().c_str(), gameHandler->get_rpaddle_location().c_str(),
             gameHandler->get_ball_location_x().c_str(), gameHandler->get_ball_location_y().c_str(), gameHandler->get_left_score().c_str(), gameHandler->get_right_score().c_str());

    // Update the status key for the left and right players with the current status so it will be sent to the clients
    redisHandler->set_key(_right_player_update, get_response_string(_status_buffer));
    redisHandler->set_key(_left_player_update, get_response_string(_status_buffer));
}

int main()
{
    // Set the flag indicating the program is running
    redisHandler->set_key(_game_program_running, "1");
    bool game_running = false;
    std::cout << "Game Setup Complete" << std::endl;

    // Once both players are connected, update the game running flag and set the boolean to allow the while loop to start
    await_players();
    game_running = true;
    redisHandler->set_key(_game_started, "1");

    // Set the initial response keys in Redis to the initial paddle positions to prevent errors with reading null values from Redis
    redisHandler->set_key(_left_player_response, std::to_string(_initial_paddle_y));
    redisHandler->set_key(_right_player_response, std::to_string(_initial_paddle_y));

    // Reset the timer
    globalTimer->reset();
    std::cout << "Game Started" << std::endl;

    // Declaring and initializing the values that will hold the data pulled from Redis and the initial ball speed
    std::string redisData = "";
    int speed = 5;

    // Enter while loop to start the game
    while (game_running)
    {
        // Wait until the last status update was sent by the server and the flag was set, then reset the flag and update the components
        while (redisHandler->get_key(_status_update_sent) != "1")
        {
        }
        redisHandler->set_key(_status_update_sent, "0");

        // Enter the for loop that determines how many times this iteration the ball location will be updated
        for (int i = 0; i < speed; i++)
        {
            // For each iteration, the left and right paddle positions are pulled from Redis to update the component
            //       positions based on the most recent value entered by the user that the server has
            redisData = redisHandler->get_key(_right_player_response);
            if (redisData != "")
                gameHandler->update_paddle_location(stoi(redisData), false);

            redisData = redisHandler->get_key(_left_player_response);
            if (redisData != "")
                gameHandler->update_paddle_location(stoi(redisData), true);

            // Update the ball location and test for a goal
            if (gameHandler->update_ball_location())
            {
                // If a goal was scored then the speed resets
                speed = 5;

                // Update the game status for the clients with the reset information so a new game screen is shown
                std::cout << "Score Made: Resetting Ball Position" << std::endl;
                update_game_status();

                // The timer is reset so the game will continue after a second
                globalTimer->reset();
                while (globalTimer->elapsed_time() < 1e9)
                {
                }

                // Break so the for-loop doesn't continue necessarily
                globalTimer->reset();
                break;
            }
            else
            {
                // If there was no goal, then just create the status update to send to the clients
                update_game_status();
            }
        }

        // If 3 seconds have passed, update the speed
        //       Don't update if a max speed of 29 has been hit
        if (globalTimer->elapsed_time() >= 3e9 && speed < 30)
        {
            speed++;
            globalTimer->reset();
        }

        // If a player has won, update the flags and indicators to show the game is no longer running
        if (gameHandler->has_won() != 0)
        {
            std::cout << "Game Completed: Beginning Setup for New Game" << std::endl;
            game_running = false;
            redisHandler->set_key(_game_started, "0");
        }

        // If either player disconnects during the game, update the flags and indicators to show the game is no longer running
        if (redisHandler->get_key(_right_player_connected) == "0" || redisHandler->get_key(_left_player_connected) == "0")
        {
            std::cout << "Player Disconnected: Game Terminating and Beginning Setup for New Game" << std::endl;
            game_running = false;
            redisHandler->set_key(_game_started, "0");
        }
    }

    // When the game completes, reset the necessary flags and show the program is no longer running
    redisHandler->set_key(_game_setup, "0");
    redisHandler->set_key(_game_started, "0");
    redisHandler->set_key(_game_program_running, "0");

    return 0;
}