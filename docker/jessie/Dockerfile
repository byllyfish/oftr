FROM debian:jessie

MAINTAINER William W. Fisher "william.w.fisher@gmail.com"

RUN apt-key adv --keyserver keyserver.ubuntu.com --recv-keys FFBE06FBB4EDC679174DD2CCD5AA2ED445128570 \
    && echo "deb http://ppa.launchpad.net/byllyfish/oftr/ubuntu trusty main" >> /etc/apt/sources.list \
    && apt-get update \
    && apt-get install -y oftr \
    && rm -rf /var/lib/apt/lists/*

EXPOSE 6633 6653

CMD ["oftr", "jsonrpc"]
