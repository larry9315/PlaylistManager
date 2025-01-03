import React, { useState, useEffect } from "react";
import "./../styles/Home.css";

function Home() {
    const [isYouTubeSignedIn, setYouTubeSignedIn] = useState(false);
    const [isSpotifySignedIn, setSpotifySignedIn] = useState(false);

    useEffect(() => {
        // Check login status dynamically for YouTube and Spotify
        const checkLoginStatus = async (service, setStatus) => {
            try {
                const response = await fetch(`https://localhost:8080/auth/${service}/status`);
                const data = await response.json();
                if (data.signedIn) {
                    setStatus(true);
                }
            } catch (err) {
                console.error(`Error checking ${service} login status:`, err);
            }
        };

        checkLoginStatus("youtube", setYouTubeSignedIn);
        checkLoginStatus("spotify", setSpotifySignedIn);
    }, []);

    const handleLogin = (service, isSignedIn) => {
        if (isSignedIn) {
            window.location.href = `/playlists/${service}`;
        } else {
            window.location.href = `https://localhost:8080/auth/${service}`;
        }
    };
    const handleYouTubeSignOut = async () => {
        try {
            await fetch("https://localhost:8080/auth/youtube/signout", {
                method: "POST",
            });
            setYouTubeSignedIn(false); // Update state
        } catch (err) {
            console.error("Error signing out from YouTube:", err);
        }
    };

    const handleSpotifySignOut = async () => {
        try {
            await fetch("https://localhost:8080/auth/spotify/signout", {
                method: "POST",
            });
            setSpotifySignedIn(false); // Update state
        } catch (err) {
            console.error("Error signing out from Spotify:", err);
        }
    };

    const handleConfirmSignOut = (service) => {
        if (window.confirm(`Are you sure you want to sign out from ${service}?`)) {
            service === "youtube" ? handleYouTubeSignOut() : handleSpotifySignOut();
        }
    };

    return (
        <div className="home-container">
            <div className="content">
                <h1>Welcome to Playlist Manager</h1>
                <p>Manage your YouTube and Spotify playlists with ease.</p>
                <div className="buttons">
                    <div>
                        <button
                            className="login-button youtube"
                            onClick={() => handleLogin("youtube", isYouTubeSignedIn)}
                        >
                            {isYouTubeSignedIn ? "Go to YouTube Playlists" : "Sign in with YouTube"}
                        </button>
                    </div>
                    <div>
                        <button
                            className="login-button spotify"
                            onClick={() => handleLogin("spotify", isSpotifySignedIn)}
                        >
                            {isSpotifySignedIn ? "Go to Spotify Playlists" : "Sign in with Spotify"}
                        </button>
                    </div>
                </div>
            </div>

            <div className="signout-container">
                {isYouTubeSignedIn && (
                    <button
                        className="signout-button"
                        onClick={() => handleConfirmSignOut("youtube")}
                    >
                        Sign Out YouTube
                    </button>
                )}
                {isSpotifySignedIn && (
                    <button
                        className="signout-button"
                        onClick={() => handleConfirmSignOut("spotify")}
                    >
                        Sign Out Spotify
                    </button>
                )}
            </div>
        </div>
    );
}

export default Home;