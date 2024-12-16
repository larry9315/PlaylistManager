#pragma once
#include <drogon/HttpSimpleController.h>

using namespace drogon;

class Playlist : public drogon::HttpSimpleController<Playlist> {
public:
    // Define the async handler function
    virtual void asyncHandleHttpRequest(const HttpRequestPtr &req,
                                        std::function<void(const HttpResponsePtr &)> &&callback) override;

    // Specify the path of the route
    PATH_LIST_BEGIN
    PATH_ADD("/playlist", Get);  // Route for HTTP GET at /playlist
    PATH_LIST_END
};