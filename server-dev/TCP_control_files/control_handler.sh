#!/bin/bash

#The point of this file is to allow for the seamless start of the TCP server

#Be sure to kill any rogue instances of previous starts to prevent issues
pkill -9 -f ./pong_controller

#If the program fails, kill and continuing programs and attempt to restart it
until ${PWD}/socket_listener; do
    pkill -9 -f ./pong_controller
    echo 'Game Resetting'
    sleep 1
done