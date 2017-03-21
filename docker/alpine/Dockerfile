FROM alpine:3.3

# Usage:
#
# Without a command, this image runs `oftr jsonrpc`.
#
#   docker run -it byllyfish/oftr
#
# You can run other subcommands:
#
#   docker run -it byllyfish/oftr oftr version
#
#   docker run -it byllyfish/oftr oftr decode < some_packet
#
# You can view man pages for oftr:
#
#   docker run -it byllyfish/oftr man oftr
#

MAINTAINER William W. Fisher "william.w.fisher@gmail.com"

# oftr needs libgcc and libstdc++. Include man as well.

RUN apk add --no-cache libgcc libstdc++ man libpcap

# Set up build dependencies, then build and install oftr. Clean up when we
# are done.

RUN set -ex \
    && apk add --no-cache --virtual .build-deps  \
        git \
        g++ \
        make \
        cmake \
        git \
        go \
        perl \
        python \
        linux-headers \
        bash \
        libpcap-dev \
    && git clone -b develop --single-branch --recursive https://github.com/byllyfish/oftr.git \
    && mkdir oftr/Build \
    && cd oftr/Build \
    && cmake .. \
    && make \
    && make test \
    && make install/strip \
    && apk del .build-deps \
    && rm -rf /oftr

EXPOSE 6633 6653

CMD ["oftr", "jsonrpc"]
