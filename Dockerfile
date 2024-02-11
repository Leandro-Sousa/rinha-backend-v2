FROM ubuntu:23.10

RUN apt-get update -yqq \
    && apt-get install -yqq --no-install-recommends software-properties-common \
    sudo curl wget cmake pkg-config locales git gcc g++ build-essential \
    openssl libssl-dev libjsoncpp-dev uuid-dev zlib1g-dev libc-ares-dev\
    postgresql-server-dev-all libsqlite3-dev \
    && rm -rf /var/lib/apt/lists/* \
    && locale-gen en_US.UTF-8

WORKDIR /app

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

COPY src            ./rinha-backend/src
COPY CMakeLists.txt ./rinha-backend
COPY build.sh       ./rinha-backend
COPY init.sql       ./rinha-backend

RUN cd rinha-backend && \
    ./build.sh && \
    mkdir /app/bin && \
    cp build/rinha-backend /app/bin && \
    cp init.sql /app && \
    cd ..

RUN rm -rf drogon && \
    rm -rf poco && \
    rm -rf rinha-backend

ENTRYPOINT [ "./bin/rinha-backend" ]
EXPOSE 8000
