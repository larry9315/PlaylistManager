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
using namespace std;

YouTubeAuth::YouTubeAuth(const string &clientId, const string &clientSecret, const string &redirectUri)
    : clientId(clientId), clientSecret(clientSecret), redirectUri(redirectUri) {}


// Function to URL-encode a string
std::string urlEncode(const std::string &value) {
    std::ostringstream encoded;
    for (char c : value) {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            encoded << c;
        } else {
            encoded << '%' << std::uppercase << std::hex << static_cast<int>(static_cast<unsigned char>(c));
        }
    }
    return encoded.str();
}

std::string YouTubeAuth::getAuthUrl() const {
    std::string encodedRedirectUri = urlEncode(redirectUri);
    return "https://accounts.google.com/o/oauth2/auth?"
           "client_id=" + clientId +
           "&redirect_uri=" + encodedRedirectUri +
           "&response_type=code"
           "&scope=https://www.googleapis.com/auth/youtube.readonly"
           "&state=youtube";
           // "&prompt=consent";
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

std::vector<std::pair<std::string, std::string>> YouTubeAuth::fetchYouTubePlaylists(const string &accessToken) const {
    try {
        io_context ioc;
        ssl::context sslCtx(ssl::context::sslv23_client);
        ssl::stream<ip::tcp::socket> sslStream(ioc, sslCtx);

        // Connect to YouTube Data API
        ip::tcp::resolver resolver(ioc);
        auto const results = resolver.resolve("www.googleapis.com", "443");
        connect(get_lowest_layer(sslStream), results.begin(), results.end());
        sslStream.handshake(ssl::stream_base::client);

        // Prepare HTTP GET request
        http::request<http::string_body> req(http::verb::get, "/youtube/v3/playlists?mine=true&part=snippet,contentDetails", 11);
        req.set(http::field::host, "www.googleapis.com");
        req.set(http::field::authorization, "Bearer " + accessToken);
        req.prepare_payload();

        // Send the request
        http::write(sslStream, req);

        // Receive the response
        flat_buffer buffer;
        http::response<http::string_body> res;
        http::read(sslStream, buffer, res);

        // Parse JSON response
        auto jsonResponse = json::parse(res.body());
        std::vector<std::pair<std::string, std::string>> playlists;

        if (jsonResponse.contains("items")) {
            for (const auto &item : jsonResponse["items"]) {
                std::string playlistId = item["id"].get<std::string>();
                std::string title = item["snippet"]["title"].get<std::string>();
                playlists.emplace_back(playlistId, title);
            }
        }

        return playlists;
    } catch (const std::exception &e) {
        std::cerr << "Error fetching YouTube playlists: " << e.what() << std::endl;
        return {};
    }
}

std::vector<std::string> YouTubeAuth::fetchYouTubePlaylistSongs(const std::string &accessToken, const std::string &playlistId) const {
    try {
        io_context ioc;
        ssl::context sslCtx(ssl::context::sslv23_client);
        ssl::stream<ip::tcp::socket> sslStream(ioc, sslCtx);

        ip::tcp::resolver resolver(ioc);
        auto const results = resolver.resolve("www.googleapis.com", "443");
        connect(get_lowest_layer(sslStream), results.begin(), results.end());
        sslStream.handshake(ssl::stream_base::client);

        std::vector<std::string> songs;

        // Construct request
        std::string target = "/youtube/v3/playlistItems?part=snippet&maxResults=50&playlistId=" + playlistId;
        http::request<http::string_body> req(http::verb::get, target, 11);
        req.set(http::field::host, "www.googleapis.com");
        req.set(http::field::authorization, "Bearer " + accessToken);
        req.prepare_payload();

        http::write(sslStream, req);

        flat_buffer buffer;
        http::response<http::string_body> res;
        http::read(sslStream, buffer, res);

        // Parse the response
        auto jsonResponse = json::parse(res.body());
        std::cout << "HTTP Response: " << res.body() << std::endl;

        // Check for errors in the API response
        if (jsonResponse.contains("error")) {
            std::cerr << "YouTube API Error: " << jsonResponse.dump() << std::endl;
            throw std::runtime_error(jsonResponse["error"]["message"].get<std::string>());
        }

        if (jsonResponse.contains("items")) {
            for (const auto &item : jsonResponse["items"]) {
                if (item.contains("snippet") && item["snippet"].contains("title")) {
                    songs.push_back(item["snippet"]["title"].get<std::string>());
                }
            }
        }

        return songs;
    } catch (const std::exception &e) {
        std::cerr << "Error fetching YouTube playlist songs: " << e.what() << std::endl;
        return {};
    }
}