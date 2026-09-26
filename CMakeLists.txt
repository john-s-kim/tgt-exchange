cmake_minimum_required(VERSION 3.20)
project(tgt_exchange LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -Wpedantic)

add_library(order_book
        order_book/order_book.cpp
        order_book/order_book.h)
target_include_directories(order_book PUBLIC order_book)

add_executable(order_book_check order_book/check.cpp)
target_link_libraries(order_book_check PRIVATE order_book)

add_executable(order_book_test order_book/order_book_test.cpp)
target_link_libraries(order_book_test PRIVATE order_book)

find_package(Drogon CONFIG REQUIRED)
add_executable(exchange
        exchange/main.cc
        exchange/exchange_state.cc
        exchange/snapshot.cc
        exchange/controllers/orders.cc
        exchange/controllers/market_data.cc)
target_include_directories(exchange PRIVATE exchange exchange/controllers)
target_link_libraries(exchange PRIVATE order_book Drogon::Drogon)
