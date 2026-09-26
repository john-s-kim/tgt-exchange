#include "market_data.h"

#include "exchange_state.h"
#include "snapshot.h"

#include <mutex>
#include <vector>

namespace {

std::mutex g_connections_mutex;
std::vector<drogon::WebSocketConnectionPtr> g_connections;

}  // namespace

void publish_market_data(const std::string& message)
{
    std::lock_guard<std::mutex> lock(g_connections_mutex);
    for (const drogon::WebSocketConnectionPtr& connection : g_connections)
    {
        if (connection && connection->connected())
        {
            connection->send(message);
        }
    }
}

void MarketData::handleNewMessage(const drogon::WebSocketConnectionPtr&,
                                  std::string&&,
                                  const drogon::WebSocketMessageType&)
{
}

void MarketData::handleNewConnection(const drogon::HttpRequestPtr&,
                                     const drogon::WebSocketConnectionPtr& connection)
{
    {
        std::lock_guard<std::mutex> lock(g_connections_mutex);
        g_connections.push_back(connection);
    }

    std::string snapshot;
    {
        std::lock_guard<std::mutex> lock(book_mutex());
        snapshot = book_snapshot_json();
    }
    connection->send(snapshot);
}

void MarketData::handleConnectionClosed(const drogon::WebSocketConnectionPtr& connection)
{
    std::lock_guard<std::mutex> lock(g_connections_mutex);
    std::erase(g_connections, connection);
}
