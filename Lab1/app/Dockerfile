FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    g++

WORKDIR /app

COPY . .

RUN cmake . && \
    make

EXPOSE 8080

CMD ["./Server"]
