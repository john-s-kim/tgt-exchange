#pragma once

#include <drogon/WebSocketController.h>

#include <string>

void publish_market_data(const std::string& message);

class MarketData : public drogon::WebSocketController<MarketData>
{
public:
    void handleNewMessage(const drogon::WebSocketConnectionPtr& connection,
                          std::string&& message,
                          const drogon::WebSocketMessageType& type) override;
    void handleNewConnection(const drogon::HttpRequestPtr& request,
                             const drogon::WebSocketConnectionPtr& connection) override;
    void handleConnectionClosed(const drogon::WebSocketConnectionPtr& connection) override;

    WS_PATH_LIST_BEGIN
    WS_PATH_ADD("/marketdata");
    WS_PATH_LIST_END
};
