#include "YouTubeAuth.h"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <nlohmann/json.hpp>
#include <sstream>
#include <iostream>

using namespace boost::beast;
using namespace boost::asio;
using namespace std;
using json = nlohmann::json;

YouTubeAuth::YouTubeAuth(const string &clientId, const string &clientSecret, const string &redirectUri)
    : clientId(clientId), clientSecret(clientSecret), redirectUri(redirectUri) {}


std::string YouTubeAuth::getAuthUrl() const {
    return "https://accounts.google.com/o/oauth2/auth?"
           "client_id=" + clientId +
           "&redirect_uri=" + redirectUri +
           "&response_type=code&scope=https://www.googleapis.com/auth/youtube.readonly"
           "&state=youtube" +
           "&prompt=consent";  // Force re-authentication;  // Add "state" to track service

}

std::string YouTubeAuth::exchangeCodeForToken(const std::string &authCode) const {
    try {
        io_context ioc;
        ssl::context sslCtx(ssl::context::sslv23_client);
        ssl::stream<ip::tcp::socket> sslStream(ioc, sslCtx);

        // Connect to Google's OAuth server
        ip::tcp::resolver resolver(ioc);
        auto const results = resolver.resolve("oauth2.googleapis.com", "443");
        connect(get_lowest_layer(sslStream), results.begin(), results.end());
        sslStream.handshake(ssl::stream_base::client);

        // HTTP POST request
        http::request<http::string_body> req(http::verb::post, "/token", 11);
        req.set(http::field::host, "oauth2.googleapis.com");
        req.set(http::field::content_type, "application/x-www-form-urlencoded");

        std::ostringstream body;
        body << "code=" << authCode
             << "&client_id=" << clientId
             << "&client_secret=" << clientSecret
             << "&redirect_uri=" << redirectUri
             << "&grant_type=authorization_code";

        req.body() = body.str();
        req.prepare_payload();

        // Send the request
        http::write(sslStream, req);

        // Receive the response
        flat_buffer buffer;
        http::response<http::string_body> res;
        http::read(sslStream, buffer, res);

        // Parse the JSON response
        auto jsonResponse = json::parse(res.body());
        if (jsonResponse.contains("access_token") && !jsonResponse["access_token"].is_null()) {
            return jsonResponse["access_token"].get<std::string>();
        } else {
            return "";  // Access token not found
        }
    } catch (const std::exception &e) {
        return "";  // Handle all exceptions gracefully
    }
}