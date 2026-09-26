#include "snapshot.h"

#include "exchange_state.h"

#include <json/json.h>
#include <vector>

std::string book_snapshot_json()
{
    std::vector<Level> bid_levels;
    std::vector<Level> ask_levels;
    book().top(bid_levels, ask_levels);

    Json::Value bids(Json::arrayValue);
    for (const Level& level : bid_levels)
    {
        Json::Value item;
        item["price"] = static_cast<Json::UInt64>(level.price);
        item["quantity"] = static_cast<Json::UInt64>(level.quantity);
        bids.append(std::move(item));
    }
    Json::Value asks(Json::arrayValue);
    for (const Level& level : ask_levels)
    {
        Json::Value item;
        item["price"] = static_cast<Json::UInt64>(level.price);
        item["quantity"] = static_cast<Json::UInt64>(level.quantity);
        asks.append(std::move(item));
    }

    Json::Value root;
    root["bids"] = std::move(bids);
    root["asks"] = std::move(asks);

    Json::StreamWriterBuilder builder;
    builder["indentation"] = "";
    return Json::writeString(builder, root);
}
