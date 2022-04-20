#!/bin/bash

pkill -9 -f ./pong_controller

until ${PWD}/socket_listener; do
    pkill -9 -f ./pong_controller
    echo 'Game Resetting'
    sleep 1
done