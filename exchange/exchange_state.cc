#include "exchange_state.h"

namespace {

OrderBook g_book;
std::mutex g_book_mutex;

}  // namespace

OrderBook& book()
{
    return g_book;
}

std::mutex& book_mutex()
{
    return g_book_mutex;
}
