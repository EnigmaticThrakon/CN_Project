#!/bin/bash

until ${PWD}/build/port_listener; do
    echo 'Server Crashed'
    sleep 1
done