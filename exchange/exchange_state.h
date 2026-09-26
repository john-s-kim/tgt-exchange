#pragma once

#include "order_book.h"

#include <mutex>

OrderBook& book();
std::mutex& book_mutex();
