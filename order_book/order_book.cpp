#include "order_book.h"

#include <algorithm>
#include <cassert>
#include <iterator>

namespace {

bool crosses(Side taker_side, std::uint64_t taker_price, std::uint64_t maker_price)
{
    if (taker_side == Side::Buy)
    {
        return taker_price >= maker_price;
    }
    return taker_price <= maker_price;
}

template <typename SideBook>
void erase_from_side(SideBook& book_side, std::uint64_t price, std::list<Order>::iterator order)
{
    const auto level = book_side.find(price);
    assert(level != book_side.end());
    level->second.erase(order);
    if (level->second.empty())
    {
        book_side.erase(level);
    }
}

template <typename SideBook>
std::vector<Level> top_levels(const SideBook& book_side, std::size_t depth)
{
    std::vector<Level> levels;
    for (auto level = book_side.begin(); level != book_side.end() && levels.size() < depth; ++level)
    {
        std::uint64_t quantity = 0;
        for (const Order& order : level->second)
        {
            quantity += order.quantity;
        }
        levels.push_back(Level{level->first, quantity});
    }
    return levels;
}

}  // namespace

template <typename SideBook>
void OrderBook::match_side(SideBook& opposite, Order& incoming)
{
    while (incoming.quantity > 0 && !opposite.empty())
    {
        const auto level = opposite.begin();
        if (!crosses(incoming.side, incoming.price, level->first))
        {
            return;
        }

        Order& resting = level->second.front();
        const std::uint64_t fill = std::min(incoming.quantity, resting.quantity);
        incoming.quantity -= fill;
        resting.quantity -= fill;
        if (resting.quantity == 0)
        {
            erase_resting(level->second.begin());
        }
    }
}

template <typename SideBook>
void OrderBook::rest_on(SideBook& book_side, const Order& order)
{
    std::list<Order>& level = book_side[order.price];
    level.push_back(order);
    orders_by_id_[order.id] = std::prev(level.end());
}

void OrderBook::erase_resting(std::list<Order>::iterator order)
{
    const Side side = order->side;
    const std::uint64_t price = order->price;
    orders_by_id_.erase(order->id);
    if (side == Side::Buy)
    {
        erase_from_side(bids, price, order);
    }
    else
    {
        erase_from_side(asks, price, order);
    }
}

std::uint64_t OrderBook::submit(Side side, std::uint64_t price, std::uint64_t quantity)
{
    if (price == 0 || quantity == 0)
    {
        return 0;
    }

    Order incoming{next_id_, side, price, quantity};
    ++next_id_;

    if (side == Side::Buy)
    {
        match_side(asks, incoming);
        if (incoming.quantity > 0)
        {
            rest_on(bids, incoming);
        }
    }
    else
    {
        match_side(bids, incoming);
        if (incoming.quantity > 0)
        {
            rest_on(asks, incoming);
        }
    }
    return incoming.id;
}

bool OrderBook::cancel(std::uint64_t id)
{
    const auto found = orders_by_id_.find(id);
    if (found == orders_by_id_.end())
    {
        return false;
    }
    const std::list<Order>::iterator order = found->second;
    erase_resting(order);
    return true;
}

void OrderBook::top(std::vector<Level>& bid_levels, std::vector<Level>& ask_levels, std::size_t depth) const
{
    bid_levels = top_levels(bids, depth);
    ask_levels = top_levels(asks, depth);
}
