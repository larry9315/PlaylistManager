#include <drogon/drogon.h>
#include "YouTubeAuth.h"

using namespace std;

int main() {
    // Replace with your actual Google OAuth credentials
    const string clientId = "284200204826-g6dpheuj2bup9c7380amkevv6i82pi8f.apps.googleusercontent.com";
    const string clientSecret = "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";
    const string redirectUri = "https://3e85-207-102-214-250.ngrok-free.app/auth/callback";

    // Initialize the YouTubeAuth object
    YouTubeAuth youtubeAuth(clientId, clientSecret, redirectUri);
    //
    // drogon::app().registerPostHandlingAdvice([](const drogon::HttpRequestPtr &req, const drogon::HttpResponsePtr &resp) {
    //     resp->addHeader("Access-Control-Allow-Origin", "*");
    //     resp->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    //     resp->addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization, ngrok-skip-browser-warning");
    //     resp->addHeader("ngrok-skip-browser-warning", "true"); // Add this header
    // });

    drogon::app().registerPostHandlingAdvice([](const drogon::HttpRequestPtr &req, const drogon::HttpResponsePtr &resp) {
        resp->addHeader("Access-Control-Allow-Origin", "*");
        resp->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        resp->addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
    });

    // Route to redirect users to the Google OAuth consent screen
    drogon::app().registerHandler("/auth", [youtubeAuth](const drogon::HttpRequestPtr &req,
                                                         std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k302Found);  // HTTP 302 Redirect
        resp->addHeader("Location", youtubeAuth.getAuthUrl());  // Redirect to Google OAuth
        callback(resp);
    });

    // Route to handle the Google OAuth callback
    drogon::app().registerHandler("/auth/callback", [youtubeAuth](const drogon::HttpRequestPtr &req,
                                                                  std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
        auto params = req->getParameters();
        auto authCode = params["code"];  // Extract the authorization code

        if (authCode.empty()) {
            auto resp = drogon::HttpResponse::newHttpJsonResponse({{"error", "Authorization code missing"}});
            callback(resp);
            return;
        }

        // Exchange the authorization code for an access token
        std::string token = youtubeAuth.exchangeCodeForToken(authCode);

        if (!token.empty()) {
            // Redirect the user to a success page or main application
            auto resp = drogon::HttpResponse::newHttpResponse();
            resp->setStatusCode(drogon::k302Found);  // 302 Redirect
            resp->addHeader("Location", "/success");  // Redirect to /success (you can define this route)
            callback(resp);
        } else {
            auto resp = drogon::HttpResponse::newHttpJsonResponse({{"error", "Failed to exchange token"}});
            callback(resp);
        }
    });

    drogon::app().registerHandler("/success", [](const drogon::HttpRequestPtr &req,
                                             std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
        // Create an HTML response
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setBody("<h1>Authorization Successful</h1><p>You have been authenticated successfully!</p>");
        resp->setContentTypeString("text/html");
        callback(resp);
    });

    // Start the Drogon server
    drogon::app().addListener("127.0.0.1", 8080).run();
    return 0;
}