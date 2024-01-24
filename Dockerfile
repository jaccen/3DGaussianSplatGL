FROM ubuntu:22.04

RUN apt-get update && \ 
    apt-get install -y build-essential cmake git libgl-dev xorg-dev

ADD . /tinyEngine
WORKDIR /tinyEngine

CMD [ "bash" ]