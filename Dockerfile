FROM debian:trixie-slim

ENV TZ=UTC
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN apt-get update -yqq \
    && apt-get install -yqq --no-install-recommends software-properties-common \
    sudo curl wget cmake make pkg-config locales git gcc-13 g++-13 \
    openssl libssl-dev libjsoncpp-dev uuid-dev zlib1g-dev libc-ares-dev\
    postgresql-server-dev-all libmariadb-dev libsqlite3-dev libhiredis-dev\
    && rm -rf /var/lib/apt/lists/* \
    && locale-gen en_US.UTF-8

ENV LANG=en_US.UTF-8 \
    LANGUAGE=en_US:en \
    LC_ALL=en_US.UTF-8 \
    CC=gcc-13 \
    CXX=g++-13 \
    AR=gcc-ar-13 \
    RANLIB=gcc-ranlib-13

RUN git clone https://github.com/drogonframework/drogon && \ 
    cd drogon && \
    git submodule update --init && \
    ./build.sh

RUN git clone -b poco-1.11.8-release --single-branch https://github.com/pocoproject/poco.git && \ 
    cd poco && \ 
    mkdir cmake-build && \ 
    cd cmake-build && \ 
    cmake .. && \ 
    cmake --build . --config Release --target install

WORKDIR /app

COPY src             ./rinha-backend/src
COPY Makefile        ./rinha-backend
COPY CMakeLists.txt  ./rinha-backend
COPY init.sql        ./

RUN cd rinha-backend && \
    make build && \
    mkdir /app/bin && \
    cp build/rinha-backend /app/bin

RUN rm -rf /app/rinha-backend

ENTRYPOINT ["./bin/rinha-backend"]
EXPOSE 8000
