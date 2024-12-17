#pragma once
#include <string>
// #include <vector>

using namespace std;

class YouTubeAuth {
public:
    // Constructor: Takes client credentials and redirect URI
    YouTubeAuth(const string &clientId, const string &clientSecret, const string &redirectUri);

    // Generate the Google OAuth URL
    string getAuthUrl() const;

    // Exchange an authorization code for an access token
    string exchangeCodeForToken(const string &authCode) const;

    vector<string> fetchYouTubePlaylists(const string &accessToken) const;

private:
    string clientId;        // Google API Client ID
    string clientSecret;    // Google API Client Secret
    string redirectUri;     // Redirect URI for OAuth callback
};