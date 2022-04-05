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
#include "shared_files/redis_functions.h"
#include "shared_files/redis_keys.h"
#include "shared_files/timer.h"
#include <thread>

#define PORT 8888

redis_handler *redisHandler = new redis_handler();
std::mutex connection_mtx;

void status_update(int socket_id)
{
    update_timer *timer = new update_timer();
    std::string reply = "20";
    bool connected = true, transmitting = false;

    while(!transmitting)
    {
        if(timer->elapsed_time() > 10) {
            reply = redisHandler->get_key(std::to_string(socket_id) + ':' + _player_transmitting);
        }

        transmitting = (reply == "1");
    }

    while (connected)
    {
        if (timer->elapsed_time() > 100)
        {
            reply = redisHandler->get_key(std::to_string(socket_id) + ':' + _player_connected);

            if (reply == "1")
            {
                if(connection_mtx.try_lock())
                {
                    reply = redisHandler->get_key(std::to_string(socket_id) + ':' + _test_response);
                    try 
                    { 
                        std::cout << reply << std::endl;
                        //write(socket_id, reply.c_str(), strlen(reply.c_str()));
                        send(socket_id, reply.c_str(), strlen(reply.c_str()), 0); 
                    }
                    catch(...) 
                    { 
                        connected = false;
                        redisHandler->set_key(std::to_string(socket_id) + ':' + _player_connected, "0"); 
                    }
                    connection_mtx.unlock();
                }
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

    return true;
}

int main(int argc, char *argv[])
{
    const std::string new_connection = "New User Connected\nSocket FD is %d\nIP : %s:%d\nSocket Index: %d\n";
    const std::string closed_connection = "User Disconnected\nSocket FD is %d\nIP : %s:%d\nSocket Index: %d\n";
    const int max_clients = 2;

    struct sockaddr_in address;
    std::thread client_threads[max_clients];
    int master_socket = 0, addrlen = sizeof(address), client_socket[max_clients], activity = 0, valread = 0, max_sd = 0;
    char buffer[1025];
    fd_set readfds;

    for (int i = 0; i < max_clients; i++) { client_socket[i] = 0; }

    if(!initialize(master_socket, readfds, address)) { return -1; }

    puts("Waiting for connections ...");


    std::string input_data = "";

    int new_socket = -1;
    while (true)
    {
        FD_ZERO(&readfds);

        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        for (int i = 0; i < max_clients; i++)
        {
            if (client_socket[i] > 0)
                FD_SET(client_socket[i], &readfds);

            if (client_socket[i] > max_sd)
                max_sd = client_socket[i];
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

            for (int i = 0; i < max_clients; i++)
            {
                if (client_socket[i] == 0)
                {
                    redisHandler->set_key(std::to_string(new_socket) + ':' + _player_connected, std::to_string(1));
                    input_data = "true";
                    client_socket[i] = new_socket;

                    send(new_socket, input_data.c_str(), strlen(input_data.c_str()), 0);
                    client_threads[i] = std::thread(status_update, new_socket);

                    printf(new_connection.c_str(), new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port), i);
                    break;
                }
            }

            new_socket = -1;
        }

        for (int i = 0; i < max_clients; i++)
        {
            if (FD_ISSET(client_socket[i], &readfds))
            {
                connection_mtx.lock();
                buffer[read(client_socket[i], buffer, 1025)] = '\0';
                input_data = std::string(buffer).substr(0, 3);

                if (input_data == "~~~")
                {
                    redisHandler->set_key(std::to_string(client_socket[i]) + ':' + _player_connected, std::to_string(0));

                    getpeername(client_socket[i], (struct sockaddr *)&address, (socklen_t *)&addrlen);
                    printf(closed_connection.c_str(), client_socket[i], inet_ntoa(address.sin_addr), ntohs(address.sin_port), i);

                    client_threads[i].join();
                    close(client_socket[i]);
                    client_socket[i] = 0;
                }
                else
                {
                    input_data = '<' + input_data + ":000,000:000,000>";

                    for(int j = 0; j < 3; j++)
                        input_data = input_data + input_data;

                    redisHandler->set_key(std::to_string(client_socket[i]) + ':' + _test_response, input_data);
                    redisHandler->set_key(std::to_string(client_socket[i]) + ':' + _player_transmitting, "1");
                    //std::cout << input_data << std::endl;
                }
                connection_mtx.unlock();
            }
        }
    }

    return 0;
}
