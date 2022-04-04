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
#include "redis_shared_files/redis_functions.h"
#include "redis_shared_files/redis_keys.h"
#include <mutex>

#define TRUE 1
#define FALSE 0
#define PORT 8888

int client_socket[30] = {0};
std::string reply = "<0:0,0:0,0>";
std::mutex redis_mtx;
redis_handler *redisHandler;

struct sockaddr_in address;
fd_set readfds;

void *status_update(int socket_id)
{
    while (true)
    {
        redis_mtx.lock();
        redisHandler->get_key(std::to_string(i) + ':' + _player_connected);
        //redisHandler->get_key(std::to_string(i) + ':' + _player_connected);
        redis_mtx.unlock();

        //if(player is connected)
        //  send the data from redis
        send(socket_id, reply.c_str(), strlen(reply.c_str()), 0);
    }
    return NULL;
}

void *client_listener(int max_sd, fd_set readfds, int socket_id)
{
    redis_mtx.lock();
    redisHandler->set_key(std::to_string(i) + ':' + _player_connected, std::to_string(1));
    redis_mtx.unlock();

    bool connected = true;
    char buffer[1024];

    while (connected)
    {
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR))
        {
            printf("select error");
        }

        if (FD_ISSET(socket_id, &readfds))
        {
            int temp = read(socket_id, buffer, 1025);
            buffer[temp] = '\0';

            if (std::string(buffer).substr(0, 3) = "~~~")
            {
                connected = false;
            }
            else
            {
                y_pos = atoi(buffer);
                reply = '<' + std::to_string(y_pos) + ':' + std::to_string(ball_x) + ',' + std::to_string(ball_y) + ':' + std::to_string(score_a) + ',' + std::to_string(score_b) + '>';
                // std::string(y_pos + ':' + ball_x + ',' + ball_y + ':' + score_a + ',' + score_b);
                send(sd, reply.c_str(), strlen(reply.c_str()), 0);
                // send(sd , buffer , strlen(buffer) , 0 );
                std::cout << std::string(buffer) << std::endl;
                redis_mtx.lock();
                redisHandler->set_key(std::to_string(i) + ':' + _player_connected, std::to_string(1));
                redis_mtx.unlock();
            }
        }
    }

    return NULL;
}

void wait_for_connections(pthread *client_threads, int master_socket, sockaddr_in address, int max_sd, fd_set readfds)
{
    std::string new_connection = "New User Connected\nSocket FD is %d\nIP : %s:%d\nSocket Index: %d\n";
    int client_count = 0;
    while (true)
    {
        FD_ZERO(&readfds);

        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        activity = select(max_sd + 1, &readfds, NULL, NULL, timeout);

        if ((activity < 0) && (errno != EINTR))
        {
            printf("select error");
        }

        if (FD_ISSET(master_socket, &readfds))
        {
            if ((new_socket = accept(master_socket,
                                     (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }
        }

        for (int i = 0; i < max_clients; i++)
        {
            // if position is empty
            if (client_socket[i] == 0)
            {
                client_count++;
                client_socket[i] = new_socket;
                FD_SET(new_socket, &readfds);

                // pthread_create(*(client_threads + client_count)

                if (new_socket > max_sd)
                    max_sd = new_socket;

                printf(new_connection.c_str(), new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port), i);

                // pthread_create(*(client_threads + client_count)
                //  printf("Adding to list of sockets as %d\n" , i);

                break;
            }
        }

        if (client_count == max_clients)
            break;
    }

    return;
}

void initialization(int master_socket)
{
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

}

int main(int argc, char *argv[])
{
    redisHandler = new redis_handler();

    int opt = TRUE;
    int master_socket, addrlen, new_socket,
        max_clients = 2, activity, i, valread, sd,
        client_socket[max_clients];
    int max_sd;

    char buffer[1025]; // data buffer of 1K

    // set of socket descriptors

    // a message
    // const char *message = "ECHO Daemon v1.0 \r\n";

    // initialise all client_socket[] to 0 so not checked
    for (int i = 0; i < max_clients; i++)
    {
        client_socket[i] = 0;
    }

    // create a master socket
    // accept the incoming connection
    addrlen = sizeof(address);
    puts("Waiting for connections ...");

    int y_pos = 0;
    const int ball_x = 0, ball_y = 0, score_a = 0, score_b = 0;

    //pthread_create(&update_thread, NULL, status_update, NULL);

    pthread_t client_threads[max_clients];
    int client_count = 0;
    while (true)
    {
        FD_ZERO(&readfds);

        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        activity = select(max_sd + 1, &readfds, NULL, NULL, timeout);

        if ((activity < 0) && (errno != EINTR))
        {
            printf("select error");
        }

        if (FD_ISSET(master_socket, &readfds))
        {
            if ((new_socket = accept(master_socket,
                                     (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }
        }

        for (int i = 0; i < max_clients; i++)
        {
            // if position is empty
            if (client_socket[i] == 0)
            {
                client_count++;
                client_socket[i] = new_socket;
                FD_SET(new_socket, &readfds);

                if (new_socket > max_sd)
                    max_sd = new_socket;

                printf(new_connection.c_str(), new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port), i);

                redisHandler->set_key(std::to_string(i) + ':' + _player_connected, std::to_string(1));
                // printf("Adding to list of sockets as %d\n" , i);

                break;
            }
        }

        if (client_count == max_clients)
            break;
    }

    while (true)
    {
    }

    while (TRUE)
    {
        // clear the socket set
        FD_ZERO(&readfds);

        // add master socket to set
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        // add child sockets to set
        client_socket_mtx.lock();
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
        client_socket_mtx.unlock();

        // wait for an activity on one of the sockets , timeout is NULL ,
        // so wait indefinitely
        struct timeval *timeout;
        timeout->tv_sec = 0;
        timeout->tv_usec = 50;
        activity = select(max_sd + 1, &readfds, NULL, NULL, timeout);

        if ((activity < 0) && (errno != EINTR))
        {
            printf("select error");
        }

        // for (int i = 0; i < max_clients; i++)
        // {
        //     send(client_socket[i], reply.c_str(), strlen(reply.c_str()), 0);
        // }

        // If something happened on the master socket ,
        // then its an incoming connection
        if (FD_ISSET(master_socket, &readfds))
        {
            if ((new_socket = accept(master_socket,
                                     (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            // inform user of socket number - used in send and receive commands
            // printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs
            //       (address.sin_port));

            // send new connection greeting message
            // reply = "Connected to marvin.webredirect.org";
            //  if( send(new_socket, reply.c_str(), strlen(reply.c_str()), 0) != strlen(reply.c_str()) )
            //  {
            //      perror("send");
            //  }

            // puts("Welcome message sent successfully");

            // add new socket to array of sockets
            client_socket_mtx.lock();

            client_socket_mtx.unlock();
        }

        // else its some IO operation on some other socket
        for (int i = 0; i < max_clients; i++)
        {
            sd = client_socket[i];

            if (FD_ISSET(sd, &readfds))
            {
                // Check if it was for closing , and also read the
                // incoming message
                valread = read(sd, buffer, 1025);
                if (std::string(buffer).substr(0, 3) == "~~~")
                {
                    // Somebody disconnected , get his details and print
                    getpeername(sd, (struct sockaddr *)&address,
                                (socklen_t *)&addrlen);
                    printf("Host disconnected , ip %s , port %d \n",
                           inet_ntoa(address.sin_addr), ntohs(address.sin_port));

                    // Close the socket and mark as 0 in list for reuse
                    close(sd);
                    client_socket_mtx.lock();
                    client_socket[i] = 0;
                    client_socket_mtx.unlock();
                }

                // Echo back the message that came in
                else
                {
                    // set the string terminating NULL byte on the end
                    // of the data read
                    buffer[valread] = '\0';

                    y_pos = atoi(buffer);
                    reply_mtx.lock();
                    reply = '<' + std::to_string(y_pos) + ':' + std::to_string(ball_x) + ',' + std::to_string(ball_y) + ':' + std::to_string(score_a) + ',' + std::to_string(score_b) + '>';
                    // std::string(y_pos + ':' + ball_x + ',' + ball_y + ':' + score_a + ',' + score_b);
                    reply_mtx.unlock();
                    send(sd, reply.c_str(), strlen(reply.c_str()), 0);
                    // send(sd , buffer , strlen(buffer) , 0 );
                    std::cout << std::string(buffer) << std::endl;
                }
            }
        }
    }

    return 0;
}
