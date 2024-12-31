

#include <drogon/drogon.h>
#include "YouTubeAuth.h"
#include "SpotifyAuth.h"

using namespace std;

string youtubeAccessToken;  // Global variable to store YouTube token
string spotifyAccessToken;  // Global variable to store Spotify token


int main() {
    // Replace with your actual OAuth credentials
    const string clientIdYouTube = "284200204826-g6dpheuj2bup9c7380amkevv6i82pi8f.apps.googleusercontent.com";
    const string clientSecretYouTube = "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";

    const string clientIdSpotify = "0baf3021cea147b5aed3fccbece055ae";
    const string clientSecretSpotify = "48f305da7af44b278fceed59b57de33b";

    const string redirectUri = "https://localhost:8080/auth/callback";

    // Initialize the YouTubeAuth and SpotifyAuth objects
    YouTubeAuth youtubeAuth(clientIdYouTube, clientSecretYouTube, redirectUri);
    SpotifyAuth spotifyAuth(clientIdSpotify, clientSecretSpotify, redirectUri);

    // Allow CORS globally
    drogon::app().registerPostHandlingAdvice([](const drogon::HttpRequestPtr &req, const drogon::HttpResponsePtr &resp) {
        resp->addHeader("Access-Control-Allow-Origin", "*");
        resp->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        resp->addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
    });

    // home route
    drogon::app().registerHandler("/", [](const drogon::HttpRequestPtr &req,
                                      std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
    // Redirect to the frontend running on localhost:3000 (or your frontend URL)
    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setStatusCode(drogon::k302Found);  // HTTP 302 Redirect
    resp->addHeader("Location", "http://localhost:3000");  // Replace with your frontend URL
    callback(resp);
});

    // Route for Auth consent screen
    drogon::app().registerHandler(
    "/auth/{service}",
    [youtubeAuth, spotifyAuth](const drogon::HttpRequestPtr &req,
                               std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                               const std::string &service) {
        auto resp = drogon::HttpResponse::newHttpResponse();

        if (service == "youtube") {
            resp->setStatusCode(drogon::k302Found);  // HTTP 302 Redirect
            resp->addHeader("Location", youtubeAuth.getAuthUrl());  // Redirect to Google OAuth
        } else if (service == "spotify") {
            resp->setStatusCode(drogon::k302Found);  // HTTP 302 Redirect
            resp->addHeader("Location", spotifyAuth.getAuthUrl());  // Redirect to Spotify OAuth
        } else {
            // Invalid service handling
            resp->setStatusCode(drogon::k400BadRequest);  // HTTP 400 Bad Request
            resp->setBody("Invalid service specified.");
        }

        callback(resp);
    });


    drogon::app().registerHandler("/auth/callback", [youtubeAuth, spotifyAuth](const drogon::HttpRequestPtr &req,
                                                                               std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
        auto params = req->getParameters();

        // Extract the authorization code
        auto authCode = params["code"];
        if (authCode.empty()) {
            std::cerr << "Error: Authorization code missing!" << std::endl;
            auto resp = drogon::HttpResponse::newHttpJsonResponse({{"error", "Authorization code missing"}});
            callback(resp);
            return;
        }

        // Extract the service from the 'state' parameter
        auto service = params["state"];  // OAuth sends 'state' back
        if (service != "youtube" && service != "spotify") {
            std::cerr << "Error: Missing or invalid service parameter!" << std::endl;
            auto resp = drogon::HttpResponse::newHttpJsonResponse({{"error", "Invalid or missing service"}});
            callback(resp);
            return;
        }

        // Perform token exchange
        std::string token;
        try {
            if (service == "youtube") {
                youtubeAccessToken = youtubeAuth.exchangeCodeForToken(authCode);
                token = youtubeAccessToken;
            } else if (service == "spotify") {
                spotifyAccessToken = spotifyAuth.exchangeCodeForToken(authCode);
                token = spotifyAccessToken;
            }
        } catch (const std::exception &e) {
            std::cerr << "Error during token exchange: " << e.what() << std::endl;
            auto resp = drogon::HttpResponse::newHttpJsonResponse({{"error", "Token exchange failed"}});
            callback(resp);
            return;
        }

        // Check if token exchange was successful
        if (!token.empty()) {
            std::cerr << "Successfully exchanged token for service: " << service << std::endl;
            auto resp = drogon::HttpResponse::newHttpResponse();
            resp->setStatusCode(drogon::k302Found);
            resp->addHeader("Location", "/success?state=" + service);
            callback(resp);
        } else {
            std::cerr << "Error: Failed to exchange token for service: " << service << std::endl;
            auto resp = drogon::HttpResponse::newHttpJsonResponse({{"error", "Failed to exchange token"}});
            callback(resp);
        }
    });

    drogon::app().registerHandler(
    "/playlists/{service}",
    [youtubeAuth, spotifyAuth](const drogon::HttpRequestPtr &req,
       std::function<void(const drogon::HttpResponsePtr &)> &&callback,
       const std::string &service) {
        Json::Value response;
        std::vector<std::pair<std::string, std::string>> playlists;

        if (service == "youtube") {
            if (youtubeAccessToken.empty()) {
                response["error"] = "YouTube access token missing";
                callback(drogon::HttpResponse::newHttpJsonResponse(response));
                return;
            }
            playlists = youtubeAuth.fetchYouTubePlaylists(youtubeAccessToken);
        } else if (service == "spotify") {
            if (spotifyAccessToken.empty()) {
                response["error"] = "Spotify access token missing";
                callback(drogon::HttpResponse::newHttpJsonResponse(response));
                return;
            }
            playlists = spotifyAuth.fetchSpotifyPlaylists(spotifyAccessToken);
        } else {
            response["error"] = "Invalid service";
            callback(drogon::HttpResponse::newHttpJsonResponse(response));
            return;
        }

        // Convert std::vector<std::string> to Json::Value
        Json::Value playlistJson(Json::arrayValue);
        for (const auto &playlist : playlists) {
            // Append each playlist string to the JSON array
            Json::Value playlistObj;
            playlistObj["id"] = playlist.first;   // playlistId
            playlistObj["name"] = playlist.second; // playlistTitle
            playlistJson.append(playlistObj);
        }

        // Prepare response JSON
        response["playlists"] = playlistJson;
        // Send the response
        auto resp = drogon::HttpResponse::newHttpJsonResponse(response);
        callback(resp);
    });

    // Route to fetch songs for a specific playlist
    drogon::app().registerHandler(
        "/playlists/{service}/{playlistId}/songs",
        [youtubeAuth, spotifyAuth](const drogon::HttpRequestPtr &req,
           std::function<void(const drogon::HttpResponsePtr &)> &&callback,
           const std::string &service, const std::string &playlistId) {
            Json::Value response;
            std::vector<std::pair<std::string, std::string>> songs;

            try {
                // Determine the service and fetch songs
                if (service == "youtube") {
                    if (youtubeAccessToken.empty()) {
                        throw std::runtime_error("Missing access token for YouTube");
                    }
                    songs = youtubeAuth.fetchYouTubePlaylistSongs(youtubeAccessToken, playlistId);
                } else if (service == "spotify") {
                    if (spotifyAccessToken.empty()) {
                        throw std::runtime_error("Missing access token for Spotify");
                    }
                    songs = spotifyAuth.fetchSpotifyPlaylistSongs(spotifyAccessToken, playlistId);
                } else {
                    throw std::runtime_error("Invalid service");
                }

                // Convert songs to JSON
                Json::Value songsJson(Json::arrayValue);
                for (const auto &song : songs) {
                    Json::Value songJson;
                    songJson["title"] = song.first; // Add other song details here if needed
                    songJson["artist"] = song.second;
                    songsJson.append(songJson);
                }

                // Prepare response
                response["songs"] = songsJson;
                callback(drogon::HttpResponse::newHttpJsonResponse(response));
            } catch (const std::exception &e) {
                // Handle errors
                response["error"] = e.what();
                callback(drogon::HttpResponse::newHttpJsonResponse(response));
            }
        });

    drogon::app().registerHandler(
    "/port/spotify-to-youtube",
    [youtubeAuth, spotifyAuth](const drogon::HttpRequestPtr &req,
                               std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
        Json::Value response;

        // Extract Spotify playlist ID and YouTube access token from the request
        auto json = req->getJsonObject();
        // if (!json || !json->isMember("spotifyPlaylistId")) {
        //     response["error"] = "Missing parameters: spotifyPlaylistId or youtubeAccessToken";
        //     callback(drogon::HttpResponse::newHttpJsonResponse(response));
        //     return;
        // }

        const std::string spotifyPlaylistId = (*json)["spotifyPlaylistId"].asString();

        // Fetch Spotify playlist songs
        auto spotifySongs = spotifyAuth.fetchSpotifyPlaylistSongs(spotifyAccessToken, spotifyPlaylistId);

        // Create a new YouTube playlist
        const std::string youtubePlaylistName = "Imported from Spotify";
        const std::string youtubePlaylistId =
            youtubeAuth.createYouTubePlaylist(youtubeAccessToken, youtubePlaylistName);

        if (youtubePlaylistId.empty()) {
            response["error"] = "Failed to create YouTube playlist";
            callback(drogon::HttpResponse::newHttpJsonResponse(response));
            return;
        }

        // Search for each song on YouTube and add to playlist
        for (const auto &song : spotifySongs) {
            std::string videoId = youtubeAuth.searchYouTubeSong(youtubeAccessToken, song.first, song.second);
            if (!videoId.empty()) {
                youtubeAuth.addSongToYouTubePlaylist(youtubeAccessToken, youtubePlaylistId, videoId);
            }
        }

        response["success"] = "Playlist ported successfully";
        response["youtubePlaylistId"] = youtubePlaylistId;
        callback(drogon::HttpResponse::newHttpJsonResponse(response));
    });

    // Success Route
    drogon::app().registerHandler("/success", [](const drogon::HttpRequestPtr &req,
                                                 std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
        // Extract the "state" query parameter
        auto params = req->getParameters();
        auto service = params["state"];  // Extract state (youtube or spotify)

        std::string redirectUrl;
        if (service == "youtube") {
            redirectUrl = "http://localhost:3000/playlists/youtube";
        } else if (service == "spotify") {
            redirectUrl = "http://localhost:3000/playlists/spotify";
        } else {
            redirectUrl = "http://localhost:3000";  // Default or fallback
        }

        // HTML response with dynamic redirect
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setBody(
            "<html>"
            "<head>"
            "<title>Authorization Successful</title>"
            "<script>"
            "  setTimeout(function() {"
            "    window.location.href = '" + redirectUrl + "';"
            "  }, 3000);"
            "</script>"
            "</head>"
            "<body>"
            "<h1>Authorization Successful</h1>"
            "<p>You have been authenticated successfully! Redirecting to playlists...</p>"
            "</body>"
            "</html>");
        resp->setContentTypeString("text/html");
        callback(resp);
    });

    // Start the Drogon server
    // drogon::app().addListener("127.0.0.1", 8080).run();
    drogon::app().addListener("0.0.0.0", 8080, true)  // Enable SSL
    .setSSLFiles("../cert.pem", "../key.pem")
    .run();
    return 0;
}