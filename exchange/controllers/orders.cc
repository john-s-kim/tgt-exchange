#include "orders.h"

#include "exchange_state.h"
#include "market_data.h"
#include "snapshot.h"

#include <json/json.h>

#include <charconv>
#include <cstdint>
#include <mutex>
#include <string>

namespace {

drogon::HttpResponsePtr json_response(Json::Value body, drogon::HttpStatusCode status)
{
    auto response = drogon::HttpResponse::newHttpJsonResponse(std::move(body));
    response->setStatusCode(status);
    return response;
}

bool read_u64(const Json::Value& value, std::uint64_t& out)
{
    if (value.isUInt64())
    {
        out = value.asUInt64();
        return true;
    }
    if (value.isInt64() && value.asInt64() >= 0)
    {
        out = static_cast<std::uint64_t>(value.asInt64());
        return true;
    }
    return false;
}

}  // namespace

void Orders::place(const drogon::HttpRequestPtr& request,
                   std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    const auto body = request->getJsonObject();
    std::uint64_t price = 0;
    std::uint64_t quantity = 0;
    if (body == nullptr || !(*body)["side"].isString() || !read_u64((*body)["price"], price) ||
        !read_u64((*body)["quantity"], quantity))
    {
        Json::Value error;
        error["error"] = "invalid order";
        callback(json_response(std::move(error), drogon::k400BadRequest));
        return;
    }
    const std::string side_text = (*body)["side"].asString();
    if (side_text != "buy" && side_text != "sell")
    {
        Json::Value error;
        error["error"] = "invalid order";
        callback(json_response(std::move(error), drogon::k400BadRequest));
        return;
    }

    std::uint64_t id = 0;
    std::string snapshot;
    {
        std::lock_guard<std::mutex> lock(book_mutex());
        id = book().submit(side_text == "buy" ? Side::Buy : Side::Sell, price, quantity);
        if (id != 0)
        {
            snapshot = book_snapshot_json();
        }
    }
    if (id == 0)
    {
        Json::Value error;
        error["error"] = "invalid order";
        callback(json_response(std::move(error), drogon::k400BadRequest));
        return;
    }
    publish_market_data(snapshot);

    Json::Value ok;
    ok["id"] = static_cast<Json::UInt64>(id);
    callback(json_response(std::move(ok), drogon::k201Created));
}

void Orders::remove(const drogon::HttpRequestPtr&,
                    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                    std::string id_text)
{
    std::uint64_t id = 0;
    const char* begin = id_text.data();
    const char* end = begin + id_text.size();
    const auto parsed = std::from_chars(begin, end, id);
    if (parsed.ec != std::errc{} || parsed.ptr != end)
    {
        id = 0;
    }

    bool removed = false;
    std::string snapshot;
    {
        std::lock_guard<std::mutex> lock(book_mutex());
        removed = book().cancel(id);
        snapshot = book_snapshot_json();
    }
    publish_market_data(snapshot);

    if (!removed)
    {
        Json::Value error;
        error["error"] = "order not found";
        callback(json_response(std::move(error), drogon::k404NotFound));
        return;
    }

    Json::Value ok;
    ok["cancelled"] = true;
    callback(json_response(std::move(ok), drogon::k200OK));
}

void Orders::show(const drogon::HttpRequestPtr&,
                  std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    std::string snapshot;
    {
        std::lock_guard<std::mutex> lock(book_mutex());
        snapshot = book_snapshot_json();
    }
    auto response = drogon::HttpResponse::newHttpResponse();
    response->setStatusCode(drogon::k200OK);
    response->setContentTypeCode(drogon::CT_APPLICATION_JSON);
    response->setBody(std::move(snapshot));
    callback(response);
}
