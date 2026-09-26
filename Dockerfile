FROM ubuntu:24.04

RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential \
        cmake \
        libdrogon-dev \
        libjsoncpp-dev \
        libssl-dev \
        uuid-dev \
        zlib1g-dev \
        libpq-dev \
        libmariadb-dev \
        libsqlite3-dev \
        libhiredis-dev \
        libbrotli-dev \
        libyaml-cpp-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build \
    && ./build/order_book_test

EXPOSE 8080
CMD ["./build/exchange"]
