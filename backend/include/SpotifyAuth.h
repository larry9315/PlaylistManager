#pragma once
#include <string>

using namespace std;

class SpotifyAuth {
public:
    SpotifyAuth(const string &clientId, const string &clientSecret, const string &redirectUri);

    string getAuthUrl() const;

    string exchangeCodeForToken(const string &code) const;

    std::vector<std::pair<std::string, std::string>> fetchSpotifyPlaylists(const string &accessToken) const;

    std::vector<std::pair<std::string, std::string>> fetchSpotifyPlaylistSongs(const std::string &accessToken, const std::string &playlistId) const;

private:
    string clientId;
    string clientSecret;
    string redirectUri;
};