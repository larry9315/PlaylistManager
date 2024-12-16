#pragma once
#include <string>

class SpotifyAuth {
public:
    SpotifyAuth(const std::string &clientId, const std::string &clientSecret, const std::string &redirectUri);
    std::string getAuthUrl() const;
    std::string exchangeCodeForToken(const std::string &code) const;

private:
    std::string clientId;
    std::string clientSecret;
    std::string redirectUri;
};