#ifndef SOCKET_LISTENER_H
#define SOCKET_LISTENER_H

#include "redis_functions.h"
#include "redis_keys.h"
#include "timer.h"

#include <sys/time.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdexcept>
#include <iostream>

class socket_listener
{
private:
    const int _port = 8888;
    int _master_socket;
    fd_set _readfds;
    struct sockaddr_in _address;
    int _max_clients;
    int _addrlen;
    int _max_sd;

    void _set_address()
    {
        _address.sin_family = AF_INET;
        _address.sin_addr.s_addr = INADDR_ANY;
        _address.sin_port = htons(_port);
    }

    bool _set_master_socket()
    {
        return (_master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0;
    }

    bool _set_sockopt()
    {
        int opt = 1;
        return (setsockopt(_master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt))) < 0;
    }

    bool _bind_master_socket_to_address()
    {
        return (bind(_master_socket, (struct sockaddr *)&_address, sizeof(_address))) < 0;
    }

    bool _begin_listening()
    {
        return (listen(_master_socket, 3)) < 0;
    }
public:
    socket_listener() { }
    socket_listener(bool &errors)
    {
        _max_clients = 2;
        _master_socket = 0;
        if(!_set_master_socket())
        {
            std::cout << "Socket Failed" << std::endl;
            errors = true;
            return;
        }
        std::cout << "Master Socket Set" << std::endl;

        if(!_set_sockopt())
        {
            std::cout << "Error in setsockopt()" << std::endl;
            errors = true;
            return;
        }
        std::cout << "Sockopt Completed Successfully" << std::endl;

        _set_address();
        if(!_bind_master_socket_to_address())
        {
            std::cout << "Bind Failure" << std::endl;
            errors = true;
            return;
        }
        std::cout << "Bind Completed Successfully: Listening on Port: " << _port << std::endl;

        if(!_begin_listening())
        {
            std::cout << "Listening Failed" << std::endl;
            errors = true;
            return;
        }
        std::cout << "Successfully Initializing Listening" << std::endl;

        _addrlen = sizeof(_address);
    }

    void send_msg(int socketIdentifier, std::string msg)
    {
        send(socketIdentifier, msg.c_str(), strlen(msg.c_str()), 0);
    }

    std::string ready_for_receiving(int lPlayerIdentifier, int rPlayerIdentifier)
    {
        FD_ZERO(&_readfds);
        FD_SET(_master_socket, &_readfds);

        if(rPlayerIdentifier > -1)
        {
            FD_SET(rPlayerIdentifier, &_readfds);

            // if(rPlayerIdentifier > _max_sd)
            //     _max_sd = rPlayerIdentifier;
        }

        if(lPlayerIdentifier > -1)
        {
            FD_SET(lPlayerIdentifier, &_readfds);

            // if(lPlayerIdentifier > _max_sd)
            //     _max_sd = lPlayerIdentifier;
        }

        if(select(lPlayerIdentifier > rPlayerIdentifier ? lPlayerIdentifier + 1 : rPlayerIdentifier + 1, &_readfds, NULL, NULL, NULL) < 0)
        {
            return "Selection Error";
        }

        return "";
    }

    int test_new_connection()
    {
        if(FD_ISSET(_master_socket, &_readfds))
        {
            
        }
    }
};

#endif