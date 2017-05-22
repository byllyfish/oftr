FROM ubuntu:zesty

MAINTAINER William W. Fisher "william.w.fisher@gmail.com"

RUN buildDeps=' \
        build-essential \
        ca-certificates \
        cmake \
        git \
        golang \
        libpcap-dev \
        python \
    ' \
    && apt-get update && apt-get install -y $buildDeps libpcap0.8 --no-install-recommends && rm -rf /var/lib/apt/lists/* \
    && cd /root \
    && git clone --depth=1 -b develop --single-branch --recursive https://github.com/byllyfish/oftr.git \
    && mkdir oftr/build && cd oftr/build \
    && cmake .. \
    && CTEST_OUTPUT_ON_FAILURE=1 make all test install/strip \
    && apt-get purge -y --auto-remove $buildDeps

EXPOSE 6653

CMD ["oftr", "jsonrpc"]
