## Preliminary docker file for afl fuzzing.

FROM ubuntu:latest

ENV AG="apt-get -qy --no-install-recommends"
ENV DEBIAN_FRONTEND=noninteractive

COPY ./fuzz_encode.sh /root

RUN $AG update \
    && $AG install afl build-essential cmake git ca-certificates libpcap-dev python

RUN pwd \
    && git clone --depth=1 --single-branch --recursive --branch=develop https://github.com/byllyfish/oftr.git \
    && mkdir oftr/FuzzBuild \
    && cd oftr/FuzzBuild \
    && cmake -DLIBOFP_ENABLE_OPENSSL=false -DCMAKE_C_COMPILER=afl-gcc -DCMAKE_CXX_COMPILER=afl-g++ .. \
    && make -j4 \
    && cd \
    && ln -s $PWD/oftr/FuzzBuild/tools/oftr/oftr afl-oftr \
    && mkdir encode_in encode_out \
    && cp oftr/tools/oftr/test/*.yml encode_in
