import React, { useState, useEffect } from "react";

const Playlists = ({ service }) => {
    // Initialize the playlists state
    const [playlists, setPlaylists] = useState([]);

    useEffect(() => {
        // Fetch playlists for the specified service
        fetch(`https://3e85-207-102-214-250.ngrok-free.app/playlists/${service}`)
            .then((response) => response.json())
            .then((data) => {
                if (data.playlists) {
                    setPlaylists(data.playlists);
                }
            })
            .catch((err) => console.error("Error fetching playlists:", err));
    }, [service]);

    return (
        <div className="playlists-container">
            <h1>{service === "youtube" ? "YouTube Playlists" : "Spotify Playlists"}</h1>
            <ul>
                {playlists.map((playlist, index) => (
                    <li key={index}>{playlist}</li>
                ))}
            </ul>
        </div>
    );
};

export default Playlists;