#include "Playlist.h"

void Playlist::asyncHandleHttpRequest(const HttpRequestPtr &req,
                                      std::function<void(const HttpResponsePtr &)> &&callback) {
    // Example JSON response
    Json::Value responseJson;
    responseJson["message"] = "Playlist API is working!";
    responseJson["status"] = "success";

    // Create the response
    auto response = HttpResponse::newHttpJsonResponse(responseJson);

    // Send the response via the callback
    callback(response);
}