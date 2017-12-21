## Preliminary docker file for afl fuzzing.

FROM ubuntu:latest

ENV AG="apt-get -qy --no-install-recommends"
ENV DEBIAN_FRONTEND=noninteractive

RUN $AG update \
    && $AG install afl build-essential cmake git

RUN pwd \
    && git clone --depth=1 --single-branch --recursive https://github.com/byllyfish/oftr.git \
    && mkdir oftr/FuzzBuild \
    && cd oftr/FuzzBuild \
    && cmake -DLIBOFP_ENABLE_OPENSSL=false -DCMAKE_C_COMPILER=afl-gcc -DCMAKE_CXX_COMPILER=afl-g++ .. \
    && make -j4 \
    && cd \
    && ln -s oftr/FuzzBuild/tools/oftr/oftr oftr

