#include <stdio.h>
#include <stdexcept>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>

// Other standard library headers
#include <iostream>
#include <string>
#include <thread>

// Header for the Redis file
#include <hiredis/hiredis.h>

// Custom header files
#include "../shared_files/redis_functions.h"
#include "../shared_files/redis_keys.h"
#include "../shared_files/timer.h"

#define PORT 8888

redis_handler *redisHandler = new redis_handler();
int left_player = -1, right_player = -1;

// Function to wait until the game is setup
void await_game_setup()
{
    // Lock in loop until the game setup flag is set then return
    while (redisHandler->get_key(_game_setup) != "1")
    {
    }
    return;
}

// Function to continually send out the game starting notification
void notify_game_starting()
{
    std::string data = "";

    // Loop until the game started flag is set and return
    while (redisHandler->get_key(_game_started) != "1")
    {
        // Get the right player status and send it to the right player client if they're connected
        data = get_response_string(redisHandler->get_key(_right_player_update));
        if (redisHandler->get_key(_right_player_connected) == "1")
            send(right_player, data.c_str(), strlen(data.c_str()), 0);

        // Get the left player status and send it to the left player client if they're connected
        data = get_response_string(redisHandler->get_key(_left_player_update));
        if (redisHandler->get_key(_left_player_connected) == "1")
            send(left_player, data.c_str(), strlen(data.c_str()), 0);
    }

    return;
}

// Function to send the status to the clients and return if they're both connected
bool send_status()
{
    std::string data = "";

    // Get the right player status and send it to the right player client if they're connected
    data = get_response_string(redisHandler->get_key(_right_player_update));
    if (redisHandler->get_key(_right_player_connected) == "1")
        send(right_player, data.c_str(), strlen(data.c_str()), 0);

    // Get the left player status and send it to the left player client if they're connected
    data = get_response_string(redisHandler->get_key(_left_player_update));
    if (redisHandler->get_key(_left_player_connected) == "1")
        send(left_player, data.c_str(), strlen(data.c_str()), 0);

    // Set the update sent flag
    redisHandler->set_key(_status_update_sent, "1");

    // Return if both players are connected
    return redisHandler->get_key(_right_player_connected) == "0" || redisHandler->get_key(_left_player_connected) == "0";
}

// Function to constantly send the status update when both players are connected and
//       the game has started
void status_update()
{
    // Send a command to the system to start the pong controller asynchronously
    system("./pong_controller &");

    // Declare and initialize a new timer
    update_timer *timer = new update_timer();
    bool connected = true, transmitting = false;
    char buff[38];

    // Wait for both players are connected and the game has started
    await_game_setup();
    notify_game_starting();

    // Reset the timer
    timer->reset();

    // While the players are connected then continue to send the status update
    while (connected)
    {
        // Send a status update after 880 microseconds
        if (timer->elapsed_time() > 8.8e5)
        {
            // Send the status update and update the indicator if either player has disconnected
            if (send_status())
                connected = false;

            // Reset the timer
            timer->reset();
        }

        // Stop the loop if the game controller exits
        if (redisHandler->get_key(_game_program_running) == "0")
            return;
    }

    return;
}

// Function to initialize the socket listener
bool initialize(int &master_socket, fd_set &readfds, struct sockaddr_in &address)
{
    // Set the master socket variable and exit with error if it equals 0
    if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
        return false;
    }

    // Set the socket options for the master socket and return with error if it returns with a negative
    int opt = 1;
    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
                   sizeof(opt)) < 0)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
        return false;
    }

    // Set the structure variables for the address struct
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the master socket to the address and return with error if the function returns with a negative
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind Failed");
        exit(EXIT_FAILURE);
        return false;
    }
    printf("Listener on port %d \n", PORT);

    // Start listening on the socket and return with error if the function returns with a negative
    if (listen(master_socket, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
        return false;
    }

    // Reset the Redis database
    redisHandler->reset_database();

    // Return true if no errors occurred
    return true;
}

int main(int argc, char *argv[])
{
    // Set the maximum number of clients able to connect
    const int max_clients = 2;

    // Declare the variables that will be used for the connection
    struct sockaddr_in address;
    int master_socket = 0, addrlen = sizeof(address), client_socket[max_clients], activity = 0, valread = 0, max_sd = 0;
    char buffer[1025];
    fd_set readfds;

    // Initialize the client_sockets to zero
    for (int i = 0; i < max_clients; i++)
    {
        client_socket[i] = 0;
    }

    // Initialize the socket variables, return -1 if there was an error
    if (!initialize(master_socket, readfds, address))
    {
        return -1;
    }
    std::cout << "Waiting for connections ..." << std::endl;

    // Initialize, declare, and reset a timer
    update_timer *tempTimer = new update_timer();
    tempTimer->reset();

    // Detach a thread so the status update is concurrently running along side the listening portion of the server
    std::thread(status_update).detach();

    // Wait for 5 milliseconds
    while (tempTimer->elapsed_time() < 5e6)
    {
    }

    std::string input_data = "";
    int new_socket = -1;

    // Loop while the pong controller is running
    while (redisHandler->get_key(_game_program_running) == "1")
    {
        // Clear readfds
        FD_ZERO(&readfds);

        // Set the master socket value and set the max_sd to the master socket
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        // If the right player is set, reset the right player and update max_sd if
        //       right player's socket identifier is greater
        if (right_player > -1)
        {
            FD_SET(right_player, &readfds);

            if (right_player > max_sd)
                max_sd = right_player;
        }

        // If the left player is set, reset the left player and update max_sd if
        //       left player's socket identifier is greater
        if (left_player > -1)
        {
            FD_SET(left_player, &readfds);

            if (left_player > max_sd)
                max_sd = left_player;
        }

        // Wait for something to happen on any of the sockets and print error if the response is a negative
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
        if ((activity < 0) && (errno != EINTR))
        {
            printf("select error");
        }

        // If something happened on the master socket add new connection
        if (FD_ISSET(master_socket, &readfds))
        {
            // Set new_socket to the identifier returned from the accept function
            new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t *)&addrlen);

            // Return with error if new_socket was negative
            if (new_socket < 0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            // If right player isn't set yet, set the right player to the incoming connection
            if (right_player < 0)
            {
                // Set the right player to the identifier
                right_player = new_socket;

                // Send a response of all 1's to let the client know they are the right player
                snprintf(buffer, sizeof(buffer), responseFormat.c_str(), "111", "111", "111", "111", "111", "111");
                input_data = get_response_string(std::string(buffer));
                send(right_player, input_data.c_str(), strlen(input_data.c_str()), 0);

                // Print log saying the right player connected
                printf(new_connection.c_str(), "Right", new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));
            }
            else if (left_player < 0) // Otherwise set the left player to the incoming connection if it's not set
            {
                // Same thing occurs for the left as is stated for the right above
                left_player = new_socket;

                snprintf(buffer, sizeof(buffer), responseFormat.c_str(), "000", "000", "000", "000", "000", "000");
                input_data = get_response_string(std::string(buffer));
                send(left_player, input_data.c_str(), strlen(input_data.c_str()), 0);

                printf(new_connection.c_str(), "Left", new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));
            }

            // Reset new socket variable
            new_socket = -1;
        }

        // Check to see if there are updated on the right player socket
        if (FD_ISSET(right_player, &readfds))
        {
            // Update the buffer to hold the response from the right player, then pull out the first three characters
            buffer[read(right_player, buffer, 1025)] = '\0';
            input_data = std::string(buffer).substr(0, 3);

            // If the response is three ~'s, disconnect the right player
            if (input_data == "~~~")
            {
                // Update the Redis flags appropriately
                redisHandler->set_key(_right_player_connected, "0");
                redisHandler->set_key(_right_started_received, "0");

                // Get the peer name and print to the console that the right player has disconnected
                getpeername(right_player, (struct sockaddr *)&address, (socklen_t *)&addrlen);
                printf(closed_connection.c_str(), "Right", right_player, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

                // Close the right player connection and reset the right player variable
                close(right_player);
                right_player = -1;

                // Exit the program
                return 0;
            }
            else
            {
                // Otherwise, set the connected flag if it isn't already and determine if the reply 999 was received
                if (redisHandler->get_key(_right_player_connected) != "1")
                    redisHandler->set_key(_right_player_connected, "1");

                else if (input_data == "999")
                    redisHandler->set_key(_right_started_received, "1"); // If so, update the player received game starting flag so the game can start

                // Finally, set the player response Redis key to what the income data was
                redisHandler->set_key(_right_player_response, input_data);
            }
        }

        if (FD_ISSET(left_player, &readfds)) // Check to see if there are any updates on the left player and perform the same tasks as above
        {
            buffer[read(left_player, buffer, 1025)] = '\0';
            input_data = std::string(buffer).substr(0, 3);

            if (input_data == "~~~")
            {
                redisHandler->set_key(_left_player_connected, "0");
                redisHandler->set_key(_left_started_received, "0");

                getpeername(left_player, (struct sockaddr *)&address, (socklen_t *)&addrlen);
                printf(closed_connection.c_str(), "Left", left_player, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

                close(left_player);
                left_player = -1;

                return 0;
            }
            else
            {
                if (redisHandler->get_key(_left_player_connected) != "1")
                    redisHandler->set_key(_left_player_connected, "1");
                else if (input_data == "999")
                    redisHandler->set_key(_left_started_received, "1");

                redisHandler->set_key(_left_player_response, input_data);
            }
        }
    }

    return 0;
}
