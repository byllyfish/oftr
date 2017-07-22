FROM alpine:3.6

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
#   docker run -v msgs.pcap:/msgs.pcap byllyfish/oftr oftr decode /msgs.pcap
#

MAINTAINER William W. Fisher "william.w.fisher@gmail.com"

# Set up build dependencies, then build and install oftr. Clean up when we
# are done. N.B. oftr needs libgcc, libstdc++, and libpcap to run.

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
    && git clone --depth=1 -b develop --single-branch --recursive https://github.com/byllyfish/oftr.git \
    && mkdir oftr/Build \
    && cd oftr/Build \
    && cmake .. \
    && CTEST_OUTPUT_ON_FAILURE=1 make all test install/strip \
    && apk add --virtual .oftr-rundeps libgcc libstdc++ libpcap \
    && apk del .build-deps \
    && rm -rf /oftr

EXPOSE 6633 6653

CMD ["oftr", "jsonrpc"]
