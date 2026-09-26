#pragma once

#include <drogon/HttpController.h>

class Orders : public drogon::HttpController<Orders>
{
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(Orders::place, "/orders", drogon::Post);
    ADD_METHOD_TO(Orders::remove, "/orders/{1}", drogon::Delete);
    ADD_METHOD_TO(Orders::show, "/book", drogon::Get);
    METHOD_LIST_END

    void place(const drogon::HttpRequestPtr& request,
               std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void remove(const drogon::HttpRequestPtr& request,
                std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                std::string id);
    void show(const drogon::HttpRequestPtr& request,
              std::function<void(const drogon::HttpResponsePtr&)>&& callback);
};
