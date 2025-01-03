#include "YouTubeAuth.h"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <nlohmann/json.hpp>
#include <sstream>
#include <iostream>
#include <json/value.h>

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
           "&scope=https://www.googleapis.com/auth/youtube https://www.googleapis.com/auth/youtube.force-ssl"
           "&state=youtube"
           "&prompt=consent";  // Force the consent screen
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

std::vector<std::pair<std::string, std::string>> YouTubeAuth::fetchYouTubePlaylistSongs(const std::string &accessToken, const std::string &playlistId) const {
    try {
        io_context ioc;
        ssl::context sslCtx(ssl::context::sslv23_client);
        ssl::stream<ip::tcp::socket> sslStream(ioc, sslCtx);

        ip::tcp::resolver resolver(ioc);
        auto const results = resolver.resolve("www.googleapis.com", "443");
        connect(get_lowest_layer(sslStream), results.begin(), results.end());
        sslStream.handshake(ssl::stream_base::client);

        std::vector<std::pair<std::string, std::string>> songs;

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

        // Check for errors in the API response
        if (jsonResponse.contains("error")) {
            std::cerr << "YouTube API Error: " << jsonResponse.dump() << std::endl;
            throw std::runtime_error(jsonResponse["error"]["message"].get<std::string>());
        }

        if (jsonResponse.contains("items")) {
            for (const auto &item : jsonResponse["items"]) {
                if (item.contains("snippet")) {
                    const auto &snippet = item["snippet"];

                    // Extract song title
                    std::string title = snippet.contains("title") ? snippet["title"].get<std::string>() : "Unknown Title";

                    // Extract artist from video description
                    std::string artist = "Unknown Artist";
                    if (snippet.contains("description") && snippet["description"].is_string()) {
                        std::string description = snippet["description"].get<std::string>();

                        // Find the position of the song title
                        auto titlePos = description.find(title);
                        if (titlePos != std::string::npos) {
                            // Find the position of the ℗ character after the title
                            auto pCharPos = description.find("℗", titlePos);
                            if (pCharPos != std::string::npos) {
                                // Extract the substring between the title and the ℗ character
                                auto start = titlePos + title.length();
                                artist = description.substr(start, pCharPos - start);

                                // Trim whitespace
                                artist.erase(0, artist.find_first_not_of(" \n\r\t"));
                                artist.erase(artist.find_last_not_of(" \n\r\t") + 1);
                            }
                        }
                    }

                    songs.emplace_back(title, artist);
                }
            }
        }


        return songs;
    } catch (const std::exception &e) {
        std::cerr << "Error fetching YouTube playlist songs: " << e.what() << std::endl;
        return {};
    }
}

std::string YouTubeAuth::createYouTubePlaylist(const std::string &accessToken, const std::string &playlistName) const {
    try {
        io_context ioc;
        ssl::context sslCtx(ssl::context::sslv23_client);
        ssl::stream<ip::tcp::socket> sslStream(ioc, sslCtx);

        ip::tcp::resolver resolver(ioc);
        auto const results = resolver.resolve("www.googleapis.com", "443");
        connect(get_lowest_layer(sslStream), results.begin(), results.end());
        sslStream.handshake(ssl::stream_base::client);

        // Prepare the HTTP POST request
        http::request<http::string_body> req(http::verb::post, "/youtube/v3/playlists?part=snippet", 11);
        req.set(http::field::host, "www.googleapis.com");
        req.set(http::field::authorization, "Bearer " + accessToken);
        req.set(http::field::content_type, "application/json");

        // Construct JSON body using nlohmann::json
        nlohmann::json body;
        body["snippet"]["title"] = playlistName;
        body["snippet"]["description"] = "Playlist imported from Spotify";

        req.body() = body.dump(); // Serialize JSON to string
        req.prepare_payload();

        // Send the request
        http::write(sslStream, req);

        // Receive the response
        flat_buffer buffer;
        http::response<http::string_body> res;
        http::read(sslStream, buffer, res);

        // Parse JSON response
        auto jsonResponse = nlohmann::json::parse(res.body());
        if (jsonResponse.contains("id")) {
            return jsonResponse["id"].get<std::string>();
        } else if (jsonResponse.contains("error")) {
            // Log error details for debugging
            std::cerr << "YouTube API Error: " << jsonResponse["error"].dump() << std::endl;
        } else {
            std::cerr << "Unexpected YouTube API Response: " << jsonResponse.dump() << std::endl;
        }
    } catch (const std::exception &e) {
        std::cerr << "Error creating YouTube playlist: " << e.what() << std::endl;
    }
    return {};
}

std::string YouTubeAuth::searchYouTubeSong(const std::string &accessToken, const std::string &title, const std::string &artist) const {
    try {
        io_context ioc;
        ssl::context sslCtx(ssl::context::sslv23_client);
        ssl::stream<ip::tcp::socket> sslStream(ioc, sslCtx);

        ip::tcp::resolver resolver(ioc);
        auto const results = resolver.resolve("www.googleapis.com", "443");
        connect(get_lowest_layer(sslStream), results.begin(), results.end());
        sslStream.handshake(ssl::stream_base::client);

        // Prepare request
        std::string query = title + " " + artist;
        std::string target = "/youtube/v3/search?part=snippet&q=" + urlEncode(query) + "&type=video";
        http::request<http::string_body> req(http::verb::get, target, 11);
        req.set(http::field::host, "www.googleapis.com");
        req.set(http::field::authorization, "Bearer " + accessToken);
        req.prepare_payload();

        // Send the request
        http::write(sslStream, req);

        // Receive the response
        flat_buffer buffer;
        http::response<http::string_body> res;
        http::read(sslStream, buffer, res);

        // Check for HTTP errors
        if (res.result() != http::status::ok) {
            std::cerr << "HTTP Error: " << res.result_int() << " - " << res.body() << std::endl;
            throw std::runtime_error("YouTube API request failed");
        }

        // Log the raw response for debugging
        std::cout << "Raw HTTP Response: " << res.body() << std::endl;

        // Parse JSON response
        auto jsonResponse = json::parse(res.body());
        if (jsonResponse.contains("items") && !jsonResponse["items"].empty()) {
            return jsonResponse["items"][0]["id"]["videoId"].get<std::string>();
        } else {
            throw std::runtime_error("No video found");
        }
    } catch (const std::exception &e) {
        std::cerr << "Error searching YouTube song: " << e.what() << std::endl;
        return {};
    }
}

void YouTubeAuth::addSongToYouTubePlaylist(const std::string &accessToken, const std::string &playlistId, const std::string &videoId) const {
    try {
        io_context ioc;
        ssl::context sslCtx(ssl::context::sslv23_client);
        ssl::stream<ip::tcp::socket> sslStream(ioc, sslCtx);

        ip::tcp::resolver resolver(ioc);
        auto const results = resolver.resolve("www.googleapis.com", "443");
        connect(get_lowest_layer(sslStream), results.begin(), results.end());
        sslStream.handshake(ssl::stream_base::client);

        // Prepare request
        http::request<http::string_body> req(http::verb::post, "/youtube/v3/playlistItems?part=snippet", 11);
        req.set(http::field::host, "www.googleapis.com");
        req.set(http::field::authorization, "Bearer " + accessToken);
        req.set(http::field::content_type, "application/json");

        Json::Value body;
        body["snippet"]["playlistId"] = playlistId;
        body["snippet"]["resourceId"]["kind"] = "youtube#video";
        body["snippet"]["resourceId"]["videoId"] = videoId;

        req.body() = body.toStyledString();
        req.prepare_payload();

        // Send request
        http::write(sslStream, req);

        // Receive response
        flat_buffer buffer;
        http::response<http::string_body> res;
        http::read(sslStream, buffer, res);
    } catch (const std::exception &e) {
        std::cerr << "Error adding song to YouTube playlist: " << e.what() << std::endl;
    }
}