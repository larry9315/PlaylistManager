// src/components/Home.js
import React from "react";
import Header from "./Header";
import "./../styles/Home.css";

function Home() {
    const handleLogin = () => {
        // Redirect to your backend's OAuth endpoint
        window.location.href = "https://3e85-207-102-214-250.ngrok-free.app/auth"; // Replace with your backend's URL

    };

    return (
        <div className="home-container">
            <Header />
            <div className="home-header">
                <h1>Welcome to Playlist Manager</h1>
                <p>Manage your YouTube playlists with ease.</p>
                <button className="login-button" onClick={handleLogin}>
                    Sign in with Google
                </button>
            </div>
        </div>
    );
}

export default Home;