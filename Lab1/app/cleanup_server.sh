#!/bin/bash

docker stop serv_container

docker rm serv_container
docker rmi serv_image

# Delete all other data associated with the container
docker system prune -a --volumes --force
