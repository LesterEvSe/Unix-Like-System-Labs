FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    build-essential \
    g++ \
    libsqlite3-dev

# Port here only for information
WORKDIR /app
EXPOSE 2222
COPY . .

RUN g++ -std=c++20 -o server server.cpp database.cpp -lpthread -lsqlite3
CMD ["./server"]
