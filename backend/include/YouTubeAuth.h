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

    std::vector<std::pair<std::string, std::string>> fetchYouTubePlaylists(const string &accessToken) const;

    std::vector<std::pair<std::string, std::string>> fetchYouTubePlaylistSongs(const std::string &accessToken, const std::string &playlistId) const;

    std::string createYouTubePlaylist(const std::string &accessToken, const std::string &playlistName) const;

    std::string searchYouTubeSong(const std::string &accessToken, const std::string &title, const std::string &artist) const;

    void addSongToYouTubePlaylist(const std::string &accessToken, const std::string &playlistId, const std::string &videoId) const;
private:
    string clientId;        // Google API Client ID
    string clientSecret;    // Google API Client Secret
    string redirectUri;     // Redirect URI for OAuth callback
};