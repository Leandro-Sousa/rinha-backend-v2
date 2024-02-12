FROM ubuntu:23.10

RUN apt-get update -yqq \
    && apt-get install -yqq --no-install-recommends software-properties-common \
    sudo curl wget make cmake pkg-config locales git gcc g++ build-essential \
    openssl libssl-dev libjsoncpp-dev uuid-dev zlib1g-dev libc-ares-dev\
    && rm -rf /var/lib/apt/lists/* \
    && locale-gen en_US.UTF-8

WORKDIR /app

RUN mkdir build

RUN git clone --branch v1.9.2 https://github.com/drogonframework/drogon && \ 
    cd drogon && \
    git submodule update --init && \
    ./build.sh

RUN git clone --branch poco-1.11.8-release https://github.com/pocoproject/poco.git && \ 
    cd poco && \ 
    mkdir cmake-build && \ 
    cd cmake-build && \ 
    cmake .. && \ 
    cmake --build . --config Release --target install

COPY src            ./rinha-backend/src
COPY Makefile       ./rinha-backend
COPY CMakeLists.txt ./rinha-backend
COPY init.sql       ./rinha-backend

RUN cd rinha-backend && \
    make build && \
    mkdir /app/bin && \
    cp build/rinha-backend /app/bin && \
    cp init.sql /app

RUN rm -rf /app/drogon && \
    rm -rf /app/poco && \
    rm -rf /app/rinha-backend

ENTRYPOINT [ "./bin/rinha-backend" ]
EXPOSE 8000
