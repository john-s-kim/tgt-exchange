//
// Created by John Kim on 9/22/26.
#ifndef TGT_EXCHANGE_ORDER_BOOK_H
#define TGT_EXCHANGE_ORDER_BOOK_H

#include <cstddef>
#include <cstdint>
#include <functional>
#include <list>
#include <map>
#include <unordered_map>
#include <vector>

enum class Side : std::uint8_t
{
    Buy,
    Sell
};

struct Order
{
    std::uint64_t id{0};
    Side side{Side::Buy};
    std::uint64_t price{0};
    std::uint64_t quantity{0};
};

struct Level
{
    std::uint64_t price{0};
    std::uint64_t quantity{0};
};

class OrderBook
{
public:
    // Returns the new order id, or 0 if price or quantity is zero.
    std::uint64_t submit(Side side, std::uint64_t price, std::uint64_t quantity);
    bool cancel(std::uint64_t id);
    // Best prices first. Each side has at most `depth` levels.
    void top(std::vector<Level>& bid_levels, std::vector<Level>& ask_levels, std::size_t depth = 5) const;

private:
    using BidBook = std::map<std::uint64_t, std::list<Order>, std::greater<>>;
    using AskBook = std::map<std::uint64_t, std::list<Order>>;

    template <typename SideBook>
    void match_side(SideBook& opposite, Order& incoming);

    template <typename SideBook>
    void rest_on(SideBook& book_side, const Order& order);

    void erase_resting(std::list<Order>::iterator order);

    std::uint64_t next_id_{1};
    BidBook bids;
    AskBook asks;
    std::unordered_map<std::uint64_t, std::list<Order>::iterator> orders_by_id_;
};

#endif //TGT_EXCHANGE_ORDER_BOOK_H
