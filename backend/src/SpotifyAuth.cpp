#include "SpotifyAuth.h"
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <sstream>

using namespace boost::beast;
using namespace http;
using namespace boost::asio;
using json = nlohmann::json;
using namespace std;


SpotifyAuth::SpotifyAuth(const std::string &clientId, const std::string &clientSecret, const std::string &redirectUri)
    : clientId(clientId), clientSecret(clientSecret), redirectUri(redirectUri) {}

std::string SpotifyAuth::getAuthUrl() const {
    return "https://accounts.spotify.com/authorize?"
           "response_type=code&client_id=" + clientId +
           "&redirect_uri=" + redirectUri +
           "&state=spotify";  // Add "state" to track service
            // "&prompt=consent"     // Force re-authentication
            // "&show_dialog=true";   // Force re-authentication
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
        flat_buffer buffer;
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

std::vector<std::pair<std::string, std::string>> SpotifyAuth::fetchSpotifyPlaylists(const string &accessToken) const {
    try {
        io_context ioc;
        ssl::context sslCtx(ssl::context::sslv23_client);
        ssl::stream<ip::tcp::socket> sslStream(ioc, sslCtx);

        // Connect to Spotify API
        ip::tcp::resolver resolver(ioc);
        auto const results = resolver.resolve("api.spotify.com", "443");
        connect(get_lowest_layer(sslStream), results.begin(), results.end());
        sslStream.handshake(ssl::stream_base::client);

        // Prepare HTTP GET request
        http::request<http::string_body> req(http::verb::get, "/v1/me/playlists", 11);
        req.set(http::field::host, "api.spotify.com");
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
                std::string title = item["name"].get<std::string>();
                playlists.emplace_back(playlistId, title);
            }
        }

        return playlists;
    } catch (const std::exception &e) {
        std::cerr << "Error fetching Spotify playlists: " << e.what() << std::endl;
        return {};
    }
}

std::vector<std::pair<std::string, std::string>> SpotifyAuth::fetchSpotifyPlaylistSongs(const std::string &accessToken, const std::string &playlistId) const {
    try {
        io_context ioc;
        ssl::context sslCtx(ssl::context::sslv23_client);
        ssl::stream<ip::tcp::socket> sslStream(ioc, sslCtx);

        // Connect to Spotify API
        ip::tcp::resolver resolver(ioc);
        auto const results = resolver.resolve("api.spotify.com", "443");
        connect(get_lowest_layer(sslStream), results.begin(), results.end());
        sslStream.handshake(ssl::stream_base::client);

        // Prepare HTTP GET request for playlist items
        std::string target = "/v1/playlists/" + playlistId + "/tracks";
        http::request<http::string_body> req(http::verb::get, target, 11);
        req.set(http::field::host, "api.spotify.com");
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
        std::vector<std::pair<std::string, std::string>> songs;

        if (jsonResponse.contains("items")) {
            for (const auto &item : jsonResponse["items"]) {
                if (item.contains("track")) {
                    const auto &track = item["track"];
                    std::string title = track.contains("name") ? track["name"].get<std::string>() : "Unknown Title";
                    std::string artist = "Unknown Artist";

                    // Extract artist names
                    if (track.contains("artists") && track["artists"].is_array() && !track["artists"].empty()) {
                        artist = track["artists"][0]["name"].get<std::string>();
                    }

                    songs.emplace_back(title, artist);
                }
            }
        }

        return songs;
    } catch (const std::exception &e) {
        std::cerr << "Error fetching Spotify playlist songs: " << e.what() << std::endl;
        return {};
    }
}