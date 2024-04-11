#!/bin/bash

if [ "$1" = "s" ]; then
    cd server
    if g++ -std=c++20 -o server server.cpp database.cpp -lpthread -lsqlite3; then
        ./server
    fi

else
    cd client
    if g++ -std=c++20 -o client client.cpp; then
        ./client
    fi
fi
