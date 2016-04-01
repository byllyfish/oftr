FROM alpine:3.3

# Usage:
#
# Without a command, this image runs `libofp jsonrpc`.
#
#   docker run -it byllyfish/libofp
#
# You can run other subcommands:
#
#   docker run -it byllyfish/libofp libofp version
#
#   docker run -it byllyfish/libofp libofp decode < some_packet
#
# You can view man pages for libofp:
#
#   docker run -it byllyfish/libofp man libofp
#

MAINTAINER William W. Fisher "william.w.fisher@gmail.com"

# Libofp needs libgcc and libstdc++. Include man as well.

RUN apk add --no-cache libgcc libstdc++ man

# Set up build dependencies, then build and install libofp. Clean up when we
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
    && git clone -b develop --single-branch --recursive https://github.com/byllyfish/libofp.git \
    && mkdir libofp/Build \
    && cd libofp/Build \
    && cmake .. \
    && make \
    && make test \
    && make install/strip \
    && apk del .build-deps \
    && rm -rf /libofp

EXPOSE 6633 6653

CMD ["libofp", "jsonrpc"]
