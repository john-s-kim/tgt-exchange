#include "order_book.h"

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

// tests written with ai
namespace {

int g_failures = 0;

void check(bool condition, const std::string& message)
{
    if (!condition)
    {
        std::cerr << "FAIL: " << message << '\n';
        ++g_failures;
    }
}

void test_price_priority()
{
    OrderBook book;
    const auto worse = book.submit(Side::Sell, 105, 1);
    const auto better = book.submit(Side::Sell, 100, 1);
    const auto buy = book.submit(Side::Buy, 110, 1);

    check(buy != 0, "buy is accepted");
    check(!book.cancel(better), "better ask was filled");
    check(book.cancel(worse), "worse ask is still resting");
    std::vector<Level> bid_levels;
    std::vector<Level> ask_levels;
    book.top(bid_levels, ask_levels);
    check(bid_levels.empty(), "filled buy does not rest");
}

void test_fifo_within_a_price()
{
    OrderBook book;
    const auto older = book.submit(Side::Sell, 100, 1);
    const auto newer = book.submit(Side::Sell, 100, 1);
    book.submit(Side::Buy, 100, 1);

    check(!book.cancel(older), "oldest order was filled");
    check(book.cancel(newer), "newer order was still resting");
    std::vector<Level> bid_levels;
    std::vector<Level> ask_levels;
    book.top(bid_levels, ask_levels);
    check(ask_levels.empty(), "level is gone");
}

void test_partial_fill()
{
    OrderBook book;
    book.submit(Side::Sell, 100, 5);
    book.submit(Side::Buy, 100, 2);

    std::vector<Level> bid_levels;
    std::vector<Level> ask_levels;
    book.top(bid_levels, ask_levels);
    check(bid_levels.empty(), "smaller taker does not rest");
    check(ask_levels.size() == 1 && ask_levels[0].price == 100 && ask_levels[0].quantity == 3,
          "resting quantity is reduced");
}

void test_sweep_multiple_levels()
{
    OrderBook book;
    book.submit(Side::Sell, 100, 2);
    book.submit(Side::Sell, 101, 2);
    book.submit(Side::Buy, 101, 5);

    std::vector<Level> bid_levels;
    std::vector<Level> ask_levels;
    book.top(bid_levels, ask_levels);
    check(ask_levels.empty(), "both ask levels cleared");
    check(bid_levels.size() == 1 && bid_levels[0].price == 101 && bid_levels[0].quantity == 1,
          "unfilled tail rests");
}

void test_cancel()
{
    OrderBook book;
    const auto bid = book.submit(Side::Buy, 50, 4);
    check(book.cancel(bid), "resting order cancels");
    std::vector<Level> bid_levels;
    std::vector<Level> ask_levels;
    book.top(bid_levels, ask_levels);
    check(bid_levels.empty(), "cancelled order leaves the book");
    check(!book.cancel(bid), "second cancel is invalid");
    check(!book.cancel(999), "unknown id is invalid");
}

}  // namespace

int main()
{
    test_price_priority();
    test_fifo_within_a_price();
    test_partial_fill();
    test_sweep_multiple_levels();
    test_cancel();

    if (g_failures != 0)
    {
        std::cerr << g_failures << " failure(s)\n";
        return EXIT_FAILURE;
    }
    std::cout << "order book tests passed\n";
    return EXIT_SUCCESS;
}
