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
struct sockaddr_in cliaddr, players[2];

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
void notify_game_starting(int master_socket)
{
    std::string data = "";

    // Loop until the game started flag is set and return
    while (redisHandler->get_key(_game_started) != "1")
    {
        // Get the right player status and send it to the right player client if they're connected
        data = get_response_string(redisHandler->get_key(_right_player_update));
        if (redisHandler->get_key(_right_player_connected) == "1")
            sendto(master_socket, data.c_str(), 1024, 0, (struct sockaddr*)&players[1], sizeof(players[1]));

        // Get the left player status and send it to the left player client if they're connected
        data = get_response_string(redisHandler->get_key(_left_player_update));
        if (redisHandler->get_key(_left_player_connected) == "1")
            sendto(master_socket, data.c_str(), 1024, 0, (struct sockaddr*)&players[0], sizeof(players[0]));
    }

    return;
}

// Function to send the status to the clients and return if they're both connected
bool send_status(int master_socket)
{
    std::string data = "";

    // Get the right player status and send it to the right player client if they're connected
    data = get_response_string(redisHandler->get_key(_right_player_update));
    if (redisHandler->get_key(_right_player_connected) == "1")
        sendto(master_socket, data.c_str(), 1024, 0, (struct sockaddr*)&players[1], sizeof(players[1]));

    // Get the left player status and send it to the left player client if they're connected
    data = get_response_string(redisHandler->get_key(_left_player_update));
    if (redisHandler->get_key(_left_player_connected) == "1")
        sendto(master_socket, data.c_str(), 1024, 0, (struct sockaddr*)&players[0], sizeof(players[0]));

    // Set the update sent flag
    redisHandler->set_key(_status_update_sent, "1");

    // Return if both players are connected
    return redisHandler->get_key(_right_player_connected) == "0" || redisHandler->get_key(_left_player_connected) == "0";
}

// Function to constantly send the status update when both players are connected and
//       the game has started
void status_update(int master_socket)
{
    // Send a command to the system to start the pong controller asynchronously
    system("./pong_controller &");

    // Declare and initialize a new timer
    update_timer *timer = new update_timer();
    bool connected = true, transmitting = false;
    char buff[38];

    // Wait for both players are connected and the game has started
    await_game_setup();
    notify_game_starting(master_socket);

    // Reset the timer
    timer->reset();

    // While the players are connected then continue to send the status update
    while (connected)
    {
        // Send a status update after 880 microseconds
        if (timer->elapsed_time() > 8.8e6)
        {
            // Send the status update and update the indicator if either player has disconnected
            if (send_status(master_socket))
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
    if ((master_socket = socket(AF_INET, SOCK_DGRAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
        return false;
    }

    // Set the socket options for the master socket and return with error if it returns with a negative
    // int opt = 1;
    struct timeval read_timeout;
    read_timeout.tv_sec = 0;
    read_timeout.tv_usec = 10;
    if (setsockopt(master_socket, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof(read_timeout)) < 0)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
        return false;
    }

    // Set the structure variables for the address struct
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(PORT);

    // Bind the master socket to the address and return with error if the function returns with a negative
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind Failed");
        exit(EXIT_FAILURE);
        return false;
    }
    printf("Listener on port %d \n", PORT);

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
    std::thread(status_update, master_socket).detach();

    // Wait for 5 milliseconds
    while (tempTimer->elapsed_time() < 5e6)
    {
    }

    std::string input_data = "";
    int new_socket = -1;
    int len = sizeof(cliaddr);
    int delimiter_location = 0;
    bool left_contacted = false, right_contacted = false;

    //Loop until both players have made contact and the response has been sent
    while(!left_contacted || !right_contacted)
    {
        //Wait until there is viable data received
        while(delimiter_location = recvfrom(master_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&cliaddr, (socklen_t *)&len) < 0) { }
        input_data = std::string(buffer);

        //If the data length is zero, continue
        if(input_data.length() == 0)
            continue;

        //If the data is long enough then pass into the parsing portion
        if((input_data.length() >= 7 && input_data.substr(0, 7) == "connect") || (input_data.length() > 4 && input_data.substr(2, 3) == "deb"))
        {
            //Check if the right play is connected, if not, set as connected and respond with the player side
        if(!right_contacted || input_data.substr(0, 1) == "1")
        {
            if (redisHandler->get_key(_right_player_connected) != "1")
            {
                if(input_data.substr(2, 3) == "deb")
                {
                    //If "deb" is received then set the right player connected flag to 1
                    redisHandler->set_key(_right_player_connected, "1");
                }
                else
                {
                    //Otherwise add the player data to the client array and respond with the player side
                    right_contacted = true;
                    players[1] = cliaddr;
                    snprintf(buffer, sizeof(buffer), responseFormat.c_str(), "111", "111", "111", "111", "111", "111");
                    input_data = get_response_string(std::string(buffer));
                    sendto(master_socket, input_data.c_str(), 1024, 0, (struct sockaddr*)&players[1], sizeof(players[1]));
                }
            }
        }
        else if(!left_contacted || input_data.substr(0, 1) == "0")  //Otherwise check if left is connected and do the same
        {                                                           //      as above if it isn't
            if (redisHandler->get_key(_left_player_connected) != "1")
            {
                if(input_data.substr(2, 3) == "deb")
                {
                    redisHandler->set_key(_left_player_connected, "1");
                }
                else
                {
                    left_contacted = true;
                    players[0] = cliaddr;
                    snprintf(buffer, sizeof(buffer), responseFormat.c_str(), "000", "000", "000", "000", "000", "000");
                    input_data = get_response_string(std::string(buffer));
                    sendto(master_socket, input_data.c_str(), 1024, 0, (struct sockaddr *)&players[0], sizeof(players[0]));
                }
            }
        }
        }
    }

    //Enter never ending loop
    while(1)
    {
        //Receive the data
        delimiter_location = recvfrom(master_socket, buffer, sizeof(buffer), 0, (struct sockaddr *)&cliaddr, (socklen_t *)&len);
        buffer[delimiter_location] = '\0';
        input_data = std::string(buffer);

        //If the data starts with 0 then it's the left player
        if(input_data.substr(0, 1) == "0")
        {
            //Set the player flag to connected if it isn't set already
            if (redisHandler->get_key(_left_player_connected) != "1")
            {
                redisHandler->set_key(_left_player_connected, "1");
            }
            //Determine if the data is a control string
            else if(input_data.substr(2, 3) == "999")
            {
                redisHandler->set_key(_left_started_received, "1");
            }
            //Otherwise just send the data into Redis
            else
            {
                redisHandler->set_key(_left_player_response, input_data.substr(2, 3));
            }
        }
        else if(input_data.substr(0, 1) == "1")     //If the data starts with 1, then it's the right player
        {                                           //      and everything above needs to be done for the right player
            if (redisHandler->get_key(_right_player_connected) != "1")
            {
                redisHandler->set_key(_right_player_connected, "1");
            }

            else if(input_data.substr(2, 3) == "999")
            {
                redisHandler->set_key(_right_started_received, "1");
            }

            else
            {
                redisHandler->set_key(_right_player_response, input_data.substr(2, 3));
            }
        }
    }
    
    return 0;
}
