#!/bin/bash

if [[ ! $(docker volume ls -q -f name=database) ]]; then
    echo "Creating volume 'database'..."
    docker volume create database
fi

cd server
if ! docker images -q server >/dev/null; then
    echo "Build 'server' application..."
    docker build . -t server
fi

docker run -p 2222:2222 -v database:/data server
cd ..
