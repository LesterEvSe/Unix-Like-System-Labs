FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    build-essential \
    g++ \
    libsqlite3-dev

WORKDIR /app
COPY . .

RUN g++ -std=c++20 -o client client.cpp
CMD ["./client"]
