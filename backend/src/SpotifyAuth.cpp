#include "SpotifyAuth.h"
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <sstream>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using json = nlohmann::json;

SpotifyAuth::SpotifyAuth(const std::string &clientId, const std::string &clientSecret, const std::string &redirectUri)
    : clientId(clientId), clientSecret(clientSecret), redirectUri(redirectUri) {}

std::string SpotifyAuth::getAuthUrl() const {
    std::ostringstream url;
    url << "https://accounts.spotify.com/authorize?"
        << "client_id=" << clientId
        << "&response_type=code"
        << "&redirect_uri=" << redirectUri
        << "&scope=user-read-private user-read-email";
    return url.str();
}

std::string SpotifyAuth::exchangeCodeForToken(const std::string &code) const {
    try {
        net::io_context ioc;
        net::ssl::context sslContext(net::ssl::context::sslv23_client);
        net::ssl::stream<net::ip::tcp::socket> sslStream(ioc, sslContext);

        // Resolve and connect
        net::ip::tcp::resolver resolver(ioc);
        auto const results = resolver.resolve("accounts.spotify.com", "443");
        net::connect(sslStream.next_layer(), results.begin(), results.end());
        sslStream.handshake(net::ssl::stream_base::client);

        // Prepare HTTP POST request
        std::ostringstream body;
        body << "grant_type=authorization_code"
             << "&code=" << code
             << "&redirect_uri=" << redirectUri
             << "&client_id=" << clientId
             << "&client_secret=" << clientSecret;

        http::request<http::string_body> req(http::verb::post, "/api/token", 11);
        req.set(http::field::host, "accounts.spotify.com");
        req.set(http::field::content_type, "application/x-www-form-urlencoded");
        req.body() = body.str();
        req.prepare_payload();

        // Send request
        http::write(sslStream, req);

        // Read response
        beast::flat_buffer buffer;
        http::response<http::string_body> res;
        http::read(sslStream, buffer, res);

        // Parse response
        json response = json::parse(res.body());
        if (response.contains("access_token")) {
            return response["access_token"].get<std::string>();
        } else {
            std::cerr << "Spotify Token Exchange Error: " << response.dump() << std::endl;
            return "";
        }
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return "";
    }
}