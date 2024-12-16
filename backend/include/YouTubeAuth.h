#pragma once
#include <string>

class YouTubeAuth {
public:
    // Constructor: Takes client credentials and redirect URI
    YouTubeAuth(const std::string &clientId, const std::string &clientSecret, const std::string &redirectUri);

    // Generate the Google OAuth URL
    std::string getAuthUrl() const;

    // Exchange an authorization code for an access token
    std::string exchangeCodeForToken(const std::string &authCode) const;

private:
    std::string clientId;        // Google API Client ID
    std::string clientSecret;    // Google API Client Secret
    std::string redirectUri;     // Redirect URI for OAuth callback
};