// src/components/Home.js
import React, { useState } from "react";
import Header from "./Header";
import "./../styles/Home.css";

function Home() {
    const [isLoggedIn, setIsLoggedIn] = useState(false);
    const [youtubePlaylists, setYouTubePlaylists] = useState([]);
    const [spotifyPlaylists, setSpotifyPlaylists] = useState([]);
    const [loading, setLoading] = useState(false);
    const [error, setError] = useState("");

    const handleYouTubeLogin = () => {
        window.location.href = "https://3e85-207-102-214-250.ngrok-free.app/auth/youtube";
    };

    const handleSpotifyLogin = () => {
        window.location.href = "https://3e85-207-102-214-250.ngrok-free.app/auth/spotify";
    };

    const fetchYouTubePlaylists = async () => {
        setLoading(true);
        setError("");
        try {
            const response = await fetch("http://localhost:8080/playlists/youtube");
            if (!response.ok) throw new Error("Failed to fetch YouTube playlists");
            const data = await response.json();
            setYouTubePlaylists(data.playlists);
            setIsLoggedIn(true);
        } catch (err) {
            console.error(err);
            setError("Error fetching YouTube playlists");
        } finally {
            setLoading(false);
        }
    };

    const fetchSpotifyPlaylists = async () => {
        setLoading(true);
        setError("");
        try {
            const response = await fetch("http://localhost:8080/playlists/spotify");
            if (!response.ok) throw new Error("Failed to fetch Spotify playlists");
            const data = await response.json();
            setSpotifyPlaylists(data.playlists);
            setIsLoggedIn(true);
        } catch (err) {
            console.error(err);
            setError("Error fetching Spotify playlists");
        } finally {
            setLoading(false);
        }
    };

    return (
        <div className="home-container">
            <div className="content">
                <h1>Welcome to Playlist Manager</h1>
                <p>Manage your YouTube and Spotify playlists with ease.</p>

                {/* Render login buttons if not logged in */}
                {!isLoggedIn ? (
                    <div className="buttons">
                        <button className="login-button youtube" onClick={handleYouTubeLogin}>
                            Sign in with YouTube
                        </button>
                        <button className="login-button spotify" onClick={handleSpotifyLogin}>
                            Sign in with Spotify
                        </button>
                    </div>
                ) : (
                    <>
                        {/* Fetch and display playlists if logged in */}
                        <div className="buttons">
                            <button className="fetch-button" onClick={fetchYouTubePlaylists}>
                                Fetch YouTube Playlists
                            </button>
                            <button className="fetch-button" onClick={fetchSpotifyPlaylists}>
                                Fetch Spotify Playlists
                            </button>
                        </div>

                        {loading && <p>Loading...</p>}
                        {error && <p style={{ color: "red" }}>{error}</p>}

                        <div>
                            <h2>YouTube Playlists</h2>
                            <ul>
                                {youtubePlaylists.length > 0 ? (
                                    youtubePlaylists.map((playlist, index) => (
                                        <li key={index}>{playlist}</li>
                                    ))
                                ) : (
                                    <p>No YouTube playlists found.</p>
                                )}
                            </ul>
                        </div>

                        <div>
                            <h2>Spotify Playlists</h2>
                            <ul>
                                {spotifyPlaylists.length > 0 ? (
                                    spotifyPlaylists.map((playlist, index) => (
                                        <li key={index}>{playlist}</li>
                                    ))
                                ) : (
                                    <p>No Spotify playlists found.</p>
                                )}
                            </ul>
                        </div>
                    </>
                )}
            </div>
        </div>
    );
}

export default Home;