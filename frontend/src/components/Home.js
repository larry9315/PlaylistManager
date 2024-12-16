// src/components/Home.js
import React from "react";
import Header from "./Header";
import "./../styles/Home.css";

function Home() {
    const handleYouTubeLogin = () => {
        // Redirect to backend YouTube OAuth endpoint
        window.location.href = "https://3e85-207-102-214-250.ngrok-free.app/auth/youtube";
    };

    const handleSpotifyLogin = () => {
        // Redirect to backend Spotify OAuth endpoint
        window.location.href = "https://3e85-207-102-214-250.ngrok-free.app/auth/spotify";
    };

    return (
        <div className="home-container">
            <div className="content">
                <h1>Welcome to Playlist Manager</h1>
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