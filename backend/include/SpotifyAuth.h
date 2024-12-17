#pragma once
#include <string>

using namespace std;

class SpotifyAuth {
public:
    SpotifyAuth(const string &clientId, const string &clientSecret, const string &redirectUri);

    string getAuthUrl() const;

    string exchangeCodeForToken(const string &code) const;

    vector<string> fetchSpotifyPlaylists(const string &accessToken) const;

private:
    string clientId;
    string clientSecret;
    string redirectUri;
};