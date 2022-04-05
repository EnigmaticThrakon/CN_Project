// Example code: A simple server side code, which echos back the received message.
// Handle multiple socket connections with select and fd_set on Linux
#include <stdio.h>
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

void *status_update(void *arg)
{
    update_timer *timer = new update_timer();
    int socket_id = *reinterpret_cast<int *>(arg);
    std::string reply = "20";
    bool connected = true;
    while (connected)
    {
        if (timer->elapsed_time() > 1000)
        {
            reply = redisHandler->get_key(std::to_string(socket_id) + ':' + _player_connected);

            if (reply == "1")
            {
                reply = redisHandler->get_key(std::to_string(socket_id) + ':' + _player_response);
                send(socket_id, reply.c_str(), strlen(reply.c_str()), 0);
            }
            else
            {
                connected = false;
            }
            timer->reset();
        }
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    const std::string new_connection = "New User Connected\nSocket FD is %d\nIP : %s:%d\nSocket Index: %d\n";

    int master_socket, addrlen, new_socket, client_socket[30],
        max_clients = 30, activity, i, valread, sd;
    int max_sd;
    struct sockaddr_in address;

    char buffer[1025]; // data buffer of 1K

    // set of socket descriptors
    fd_set readfds;

    // a message
    // const char *message = "ECHO Daemon v1.0 \r\n";

    // initialise all client_socket[] to 0 so not checked
    for (i = 0; i < max_clients; i++)
    {
        client_socket[i] = 0;
    }

    // create a master socket
    if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // set master socket to allow multiple connections ,
    // this is just a good habit, it will work without this
    int opt = 1;
    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
                   sizeof(opt)) < 0)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // type of socket created
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // bind the socket to localhost port 8888
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listener on port %d \n", PORT);

    // try to specify maximum of 3 pending connections for the master socket
    if (listen(master_socket, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // accept the incoming connection
    addrlen = sizeof(address);
    puts("Waiting for connections ...");

    pthread_t client_threads[max_clients];

    int y_pos = 0;
    const int ball_x = 0, ball_y = 0, score_a = 0, score_b = 0;
    std::string reply = "";
    while (true)
    {
        // clear the socket set
        FD_ZERO(&readfds);

        // add master socket to set
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        // add child sockets to set
        for (i = 0; i < max_clients; i++)
        {
            // socket descriptor
            sd = client_socket[i];

            // if valid socket descriptor then add to read list
            if (sd > 0)
                FD_SET(sd, &readfds);

            // highest file descriptor number, need it for the select function
            if (sd > max_sd)
                max_sd = sd;
        }

        // wait for an activity on one of the sockets , timeout is NULL ,
        // so wait indefinitely
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR))
        {
            printf("select error");
        }

        // If something happened on the master socket ,
        // then its an incoming connection
        if (FD_ISSET(master_socket, &readfds))
        {
            int *new_socket = new int;
            *new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t *)&addrlen);

            if (*new_socket < 0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            for (i = 0; i < max_clients; i++)
            {
                if (client_socket[i] == 0)
                {
                    reply = "true";
                    client_socket[i] = *new_socket;
                    send(*new_socket, reply.c_str(), strlen(reply.c_str()), 0);
                    pthread_create(&client_threads[i], NULL, status_update, static_cast<void *>(new_socket));
                    printf(new_connection.c_str(), *new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port), i);

                    // redisHandler->set_key(std::to_string(*new_socket) + ':' + _player_connected, std::to_string(1));

                    break;
                }
            }

            free(new_socket);
        }

        // else its some IO operation on some other socket
        for (i = 0; i < max_clients; i++)
        {
            sd = client_socket[i];

            if (FD_ISSET(sd, &readfds))
            {
                // Check if it was for closing , and also read the
                // incoming message
                valread = read(sd, buffer, 1025);
                if (std::string(buffer).substr(0, 3) == "~~~")
                {
                    //redisHandler->set_key(std::to_string(client_socket[i]) + ':' + _player_connected, std::to_string(0));
                    // Somebody disconnected , get his details and print
                    getpeername(sd, (struct sockaddr *)&address,
                                (socklen_t *)&addrlen);
                    printf("Host disconnected , ip %s , port %d \n",
                           inet_ntoa(address.sin_addr), ntohs(address.sin_port));

                    // Close the socket and mark as 0 in list for reuse
                    close(sd);
                    client_socket[i] = 0;
                    //pthread_join(client_threads[i], NULL);
                }

                // Echo back the message that came in
                else
                {
                    redisHandler->set_key(std::to_string(client_socket[i]) + ':' + _player_connected, std::to_string(1));
                    // set the string terminating NULL byte on the end
                    // of the data read
                    buffer[valread] = '\0';

                    y_pos = atoi(buffer);

                    reply = '<' + std::to_string(y_pos) + ':' + std::to_string(ball_x) + ',' + std::to_string(ball_y) + ':' + std::to_string(score_a) + ',' + std::to_string(score_b) + ">";
                    redisHandler->set_key(std::to_string(client_socket[i]) + ':' + _player_response, reply);
                    // std::string(y_pos + ':' + ball_x + ',' + ball_y + ':' + score_a + ',' + score_b);

                    // send(sd, reply.c_str(), strlen(reply.c_str()), 0);
                    std::cout << std::string(buffer) << std::endl;
                }
            }
        }
    }

    return 0;
}
