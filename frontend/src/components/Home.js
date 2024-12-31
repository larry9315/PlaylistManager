// src/components/Home.js
import React from "react";
import "./../styles/Home.css";

function Home() {
    const handleYouTubeLogin = () => {
        window.location.href = "https://localhost:8080/auth/youtube?redirect_uri=https://localhost:3000/playlists/youtube";
    };

    const handleSpotifyLogin = () => {
        window.location.href = "https://localhost:8080/auth/spotify?redirect_uri=https://localhost:3000/playlists/spotify";
    };

    return (
        <div className="home-container">
            <div className="content">
                <h1>Playlist Manager</h1>
                <p>Manage your YouTube and Spotify playlists with ease.</p>
                <div className="buttons">
                    <button className="login-button youtube" onClick={handleYouTubeLogin}>
                        Sign in with YouTube
                    </button>
                    <button className="login-button spotify" onClick={handleSpotifyLogin}>
                        Sign in with Spotify
                    </button>
                </div>
            </div>
        </div>
    );
}

export default Home;