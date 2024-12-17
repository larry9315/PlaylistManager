

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

    const string redirectUri = "https://3e85-207-102-214-250.ngrok-free.app/auth/callback";

    // Initialize the YouTubeAuth and SpotifyAuth objects
    YouTubeAuth youtubeAuth(clientIdYouTube, clientSecretYouTube, redirectUri);
    SpotifyAuth spotifyAuth(clientIdSpotify, clientSecretSpotify, redirectUri);

    // Allow CORS globally
    drogon::app().registerPostHandlingAdvice([](const drogon::HttpRequestPtr &req, const drogon::HttpResponsePtr &resp) {
        resp->addHeader("Access-Control-Allow-Origin", "*");
        resp->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        resp->addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
    });

    // Route for YouTube OAuth consent screen
    drogon::app().registerHandler("/auth/youtube", [youtubeAuth](const drogon::HttpRequestPtr &req,
                                                                 std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k302Found);  // HTTP 302 Redirect
        resp->addHeader("Location", youtubeAuth.getAuthUrl());  // Redirect to Google OAuth
        callback(resp);
    });

    // Route for Spotify OAuth consent screen
    drogon::app().registerHandler("/auth/spotify", [spotifyAuth](const drogon::HttpRequestPtr &req,
                                                                 std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k302Found);  // HTTP 302 Redirect
        resp->addHeader("Location", spotifyAuth.getAuthUrl());  // Redirect to Spotify OAuth
        callback(resp);
    });

    // route to /auth/callback
    // drogon::app().registerHandler(
    // "/auth/callback",
    // [youtubeAuth, spotifyAuth](const drogon::HttpRequestPtr &req,
    //                             std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
    //     // Log query parameters
    //     auto params = req->getParameters();
    //     std::cout << "Received Query Parameters:" << std::endl;
    //     for (const auto &p : params) {
    //         std::cout << p.first << ": " << p.second << std::endl;
    //     }
    //
    //     // Extract "code" parameter
    //     auto authCode = params["code"];
    //     if (authCode.empty()) {
    //         std::cerr << "Error: Authorization code missing!" << std::endl;
    //         auto resp = drogon::HttpResponse::newHttpJsonResponse({{"error", "Authorization code missing"}});
    //         callback(resp);
    //         return;
    //     }
    //
    //     // Extract "service" parameter or fallback to "state"
    //     auto service = params["service"];
    //     if (service.empty()) {
    //         service = params["state"];  // Use "state" as a fallback
    //     }
    //
    //     if (service.empty() || (service != "youtube" && service != "spotify")) {
    //         std::cerr << "Error: Missing or invalid service parameter!" << std::endl;
    //         auto resp = drogon::HttpResponse::newHttpJsonResponse({{"error", "Invalid or missing service"}});
    //         callback(resp);
    //         return;
    //     }
    //
    //     // Exchange token based on service
    //     std::string token;
    //     try {
    //         if (service == "youtube") {
    //             token = youtubeAuth.exchangeCodeForToken(authCode);
    //         } else if (service == "spotify") {
    //             token = spotifyAuth.exchangeCodeForToken(authCode);
    //         }
    //     } catch (const std::exception &e) {
    //         std::cerr << "Error during token exchange: " << e.what() << std::endl;
    //         auto resp = drogon::HttpResponse::newHttpJsonResponse({{"error", "Token exchange failed"}});
    //         callback(resp);
    //         return;
    //     }
    //
    //     // Check if the token was retrieved
    //     if (!token.empty()) {
    //         auto resp = drogon::HttpResponse::newHttpResponse();
    //         resp->setStatusCode(drogon::k302Found);
    //         resp->addHeader("Location", "/success");
    //         callback(resp);
    //     } else {
    //         std::cerr << "Error: Failed to exchange token for service: " << service << std::endl;
    //         auto resp = drogon::HttpResponse::newHttpJsonResponse({{"error", "Failed to exchange token"}});
    //         callback(resp);
    //     }
    // });
    drogon::app().registerHandler("/auth/callback", [youtubeAuth, spotifyAuth](const drogon::HttpRequestPtr &req,
                                                                               std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
        auto params = req->getParameters();

        // Log parameters for debugging
        std::cerr << "Received Query Parameters:\n";
        for (const auto &p : params) {
            std::cerr << p.first << ": " << p.second << std::endl;
        }

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
            resp->addHeader("Location", "/success");
            callback(resp);
        } else {
            std::cerr << "Error: Failed to exchange token for service: " << service << std::endl;
            auto resp = drogon::HttpResponse::newHttpJsonResponse({{"error", "Failed to exchange token"}});
            callback(resp);
        }
    });

    // Route to fetch YouTube playlists
    drogon::app().registerHandler(
        "/playlists/youtube",
        [youtubeAuth](const drogon::HttpRequestPtr &req,
                      std::function<void(const drogon::HttpResponsePtr &)> &&callback) {

            if (youtubeAccessToken.empty()) {
                auto resp = drogon::HttpResponse::newHttpJsonResponse({{"error", "Missing access token for YouTube"}});
                callback(resp);
                return;
            }

            auto playlists = youtubeAuth.fetchYouTubePlaylists(youtubeAccessToken);

            // Convert std::vector<std::string> to Json::Value
            Json::Value playlistJson(Json::arrayValue);
            for (const auto &playlist : playlists) {
                playlistJson.append(playlist);
            }

            Json::Value response;
            response["playlists"] = playlistJson;

            auto resp = drogon::HttpResponse::newHttpJsonResponse(response);

            callback(resp);
        });

    // Route to fetch Spotify playlists
    drogon::app().registerHandler(
        "/playlists/spotify",
        [spotifyAuth](const drogon::HttpRequestPtr &req,
                      std::function<void(const drogon::HttpResponsePtr &)> &&callback) {

            if (spotifyAccessToken.empty()) {
                auto resp = drogon::HttpResponse::newHttpJsonResponse({{"error", "Missing access token for Spotify"}});
                callback(resp);
                return;
            }

            auto playlists = spotifyAuth.fetchSpotifyPlaylists(spotifyAccessToken);

            // Convert std::vector<std::string> to Json::Value
            Json::Value playlistJson(Json::arrayValue);
            for (const auto &playlist : playlists) {
                playlistJson.append(playlist);
            }

            Json::Value response;
            response["playlists"] = playlistJson;

            auto resp = drogon::HttpResponse::newHttpJsonResponse(response);

            callback(resp);
        });

    // Success Route
    drogon::app().registerHandler("/success", [](const drogon::HttpRequestPtr &req,
                                                 std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setBody(
            "<html>"
            "<head>"
            "<title>Authorization Successful</title>"
            "<script>"
            "  setTimeout(function() {"
            "    window.location.href = 'https://3e85-207-102-214-250.ngrok-free.app/playlists/youtube';"
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
    drogon::app().addListener("127.0.0.1", 8080).run();
    return 0;
}