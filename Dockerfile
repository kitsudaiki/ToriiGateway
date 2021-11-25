FROM ubuntu:20.04

RUN apt-get update; \
    apt-get install -y openssl libuuid1 libcrypto++6; \
    mkdir /etc/torii
COPY upload/ToriiGateway /usr/bin/ToriiGateway

