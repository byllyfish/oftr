FROM alpine:3.3

MAINTAINER William W. Fisher "william.w.fisher@gmail.com"

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
    && make install \
    && apk del .build-deps \
    && rm -rf /libofp

EXPOSE 6633 6653

CMD ["libofp", "jsonrpc"]
