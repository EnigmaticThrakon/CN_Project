// Example code: A simple server side code, which echos back the received message.
// Handle multiple socket connections with select and fd_set on Linux
#include <stdio.h>
#include <stdexcept>
#include <string.h> //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>    //close
#include <arpa/inet.h> //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include <iostream>
#include <string>
#include <hiredis/hiredis.h>
#include "../shared_files/redis_functions.h"
#include "../shared_files/redis_keys.h"
#include "../shared_files/timer.h"
#include <thread>

#define PORT 8888

redis_handler *redisHandler = new redis_handler();
std::mutex connection_mtx;
int left_player = -1, right_player = -1;

std::string get_response_string(std::string formattedString)
{
    for(int i = 0; i < 5; i++)
        formattedString = formattedString + formattedString;
    
    return formattedString;
}

void status_update()
{
    update_timer *timer = new update_timer();
    std::string reply = "20";
    bool connected = true, transmitting = false;
    char buff[38];

    while(redisHandler->get_key(_game_started) != "1") { timer->reset(); }
    reply = get_response_string(redisHandler->get_key(_game_status));
    //send(left_player, reply.c_str(), strlen(reply.c_str()), 0);
    send(right_player, reply.c_str(), strlen(reply.c_str()), 0);
    while(reply == redisHandler->get_key(_game_status)) { timer->reset(); }

    while (connected)
    {
        if (timer->elapsed_time() > 1000)
        {
            if (redisHandler->get_key(_right_player_connected) == "1")// && redisHandler->get_key(_left_player_connected) == "1")
            {
                //send(left_player, reply.c_str(), strlen(reply.c_str()), 0);
                send(right_player, reply.c_str(), strlen(reply.c_str()), 0);

                reply = get_response_string(redisHandler->get_key(_game_status));
            }
            else
            {
                connected = false;
            }
            timer->reset();
        }
    }

    return;
}

bool initialize(int &master_socket, fd_set &readfds, struct sockaddr_in &address)
{
    if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
        return false;
    }

    int opt = 1;
    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
                   sizeof(opt)) < 0)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
        return false;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind Failed");
        exit(EXIT_FAILURE);
        return false;
    }
    printf("Listener on port %d \n", PORT);

    if (listen(master_socket, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
        return false;
    }

    redisHandler->reset_database();

    return true;
}

int main(int argc, char *argv[])
{
    const std::string new_connection = "New User Connected\n%s Player\nSocket FD is %d\nIP : %s:%d\n";
    const std::string closed_connection = "User Disconnected\n%s Player\nSocket FD is %d\nIP : %s:%d\n";
    const int max_clients = 2;

    struct sockaddr_in address;
    std::thread client_threads[max_clients];
    int master_socket = 0, addrlen = sizeof(address), client_socket[max_clients], activity = 0, valread = 0, max_sd = 0;
    char buffer[1025];
    fd_set readfds;

    for (int i = 0; i < max_clients; i++) { client_socket[i] = 0; }

    if(!initialize(master_socket, readfds, address)) { return -1; }

    puts("Waiting for connections ...");

    std::thread(status_update).detach();

    std::string input_data = "";
    int new_socket = -1;
    
    while (true)
    {
        FD_ZERO(&readfds);

        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        if(right_player > -1)
        {
            FD_SET(right_player, &readfds);

            if(right_player > max_sd)
                max_sd = right_player;
        }
        if(left_player > -1)
        {
            FD_SET(left_player, &readfds);

            if(left_player > max_sd)
                max_sd = left_player;
        }

        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) { printf("select error"); }

        if (FD_ISSET(master_socket, &readfds))
        {
            new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t *)&addrlen);

            if (new_socket < 0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            if(right_player < 0)
            {
                right_player = new_socket;
                
                snprintf(buffer, sizeof(buffer), responseFormat.c_str(), "111", "111", "111", "111", "111", "111");
                input_data = get_response_string(std::string(buffer));
                send(right_player, input_data.c_str(), strlen(input_data.c_str()), 0);

                printf(new_connection.c_str(), "Right", new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));
            }
            else if(left_player < 0)
            {
                left_player = new_socket;

                snprintf(buffer, sizeof(buffer), responseFormat.c_str(), "000", "000", "000", "000", "000", "000");
                input_data = get_response_string(std::string(buffer));
                send(left_player, input_data.c_str(), strlen(input_data.c_str()), 0);

                printf(new_connection.c_str(), "Left", new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));
            }

            new_socket = -1;
        }

        if(FD_ISSET(right_player, &readfds))
        {
            buffer[read(right_player, buffer, 1025)] = '\0';
            input_data = std::string(buffer).substr(0, 3);

            if(input_data == "~~~")
            {
                redisHandler->set_key(_right_player_connected, std::to_string(0));

                    getpeername(right_player, (struct sockaddr *)&address, (socklen_t *)&addrlen);
                    printf(closed_connection.c_str(), "Right", right_player, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

                    close(right_player);
                    right_player = -1;
            }
            else 
            {
                if(redisHandler->get_key(_right_player_connected) != "1")
                {              
                    // snprintf(buffer, sizeof(buffer), responseFormat.c_str(), redisHandler->get_key(_right_player_response).c_str(), redisHandler->get_key(_rscore_location_x).c_str(), "000", "000", "000", "000");
                    // input_data = get_response_string(std::string(buffer));
                    // send(right_player, input_data.c_str(), strlen(input_data.c_str()), 0);

                    redisHandler->set_key(_right_player_connected, std::to_string(1));
                }

                redisHandler->set_key(_right_player_response, input_data);
            }
        }
        else if(FD_ISSET(left_player, &readfds))
        {
            buffer[read(left_player, buffer, 1025)] = '\0';
            input_data = std::string(buffer).substr(0, 3);

            if(input_data == "~~~")
            {
                redisHandler->set_key(_left_player_connected, std::to_string(0));

                getpeername(left_player, (struct sockaddr *)&address, (socklen_t *)&addrlen);
                printf(closed_connection.c_str(), "Left", left_player, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

                close(left_player);
                left_player = -1;
            }
            else 
            {
                if(redisHandler->get_key(_left_player_connected) != "1")
                {              
                    // snprintf(buffer, sizeof(buffer), responseFormat.c_str(), redisHandler->get_key(_left_player_response).c_str(), redisHandler->get_key(_lscore_location_x).c_str(), "000", "000", "000", "000");
                    // input_data = get_response_string(std::string(buffer));
                    // send(left_player, input_data.c_str(), strlen(input_data.c_str()), 0);

                    redisHandler->set_key(_left_player_connected, std::to_string(1));
                }

                redisHandler->set_key(_left_player_response, input_data);
            }
        }
    }

    return 0;
}
