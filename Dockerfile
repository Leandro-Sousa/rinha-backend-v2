FROM leandro-sousa/drogon:latest

WORKDIR /app

COPY src             ./rinha-backend/src
COPY Makefile        ./rinha-backend
COPY CMakeLists.txt  ./rinha-backend

RUN cd rinha-backend && \
    make build && \
    mkdir /app/bin && \
    cp build/rinha-backend /app/bin

RUN rm -rf /app/rinha-backend

ENTRYPOINT ["./bin/rinha-backend"]
EXPOSE 8000
