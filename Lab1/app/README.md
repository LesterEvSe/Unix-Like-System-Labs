## Docker Compose commands
docker compose up          - start app
docker compose up --build  - rebuild file
docker compose up -d       - in detach thread
docker compose down        - end the apps and clear all containers

docker compose logs 'id'   - show output of id


docker ps -a  - check all containers ever executed (id, command etc.)
docker container prune  - delete all stop containers
docker exec -it 'id' sh - when container with 'id' is started, we can check it's structure

docker build . -t client - build current Dockerfile with tag 'client'
docker run -i -p 2222:2222 server - run container 'client' on port 2222:2222. -i for output
