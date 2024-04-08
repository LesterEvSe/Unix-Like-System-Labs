FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    build-essential \
    g++ \
    libsqlite3-dev

WORKDIR /app

COPY . .

RUN g++ -o server server.cpp -lpthread -lsqlite3

EXPOSE 8080

CMD ["./server"]
