#!/bin/bash

docker build -t serv_image .
docker run -d -p 8080:8080 --name serv_container serv_image
