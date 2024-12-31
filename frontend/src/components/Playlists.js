import React, { useEffect, useState } from "react";
import { useParams, useNavigate } from "react-router-dom"; // Import useParams for dynamic routing
import "./../styles/Playlists.css";

const Playlists = () => {
    const { service } = useParams(); // Get the service parameter from the URL
    const [playlists, setPlaylists] = useState([]); // List of playlists
    const [expandedPlaylist, setExpandedPlaylist] = useState(null); // Tracks which playlist is expanded
    const [songs, setSongs] = useState({}); // Store songs for each playlist
    const [portStatus, setPortStatus] = useState(null);

    const navigate = useNavigate();

    // Fetch playlists on component mount
    useEffect(() => {
        fetch(`https://localhost:8080/playlists/${service}`)
            .then((response) => response.json())
            .then((data) => {
                if (data.playlists) {
                    setPlaylists(data.playlists);
                }
            })
            .catch((err) => console.error("Error fetching playlists:", err));
    }, [service]);

    // Handle expanding/collapsing a playlist
    const handleExpand = (playlistId) => {
        // Toggle expanded state
        setExpandedPlaylist((prev) => (prev === playlistId ? null : playlistId));

        // Fetch songs if not already loaded
        if (!songs[playlistId]) {
            fetch(`https://localhost:8080/playlists/${service}/${playlistId}/songs`)
                .then((response) => response.json())
                .then((data) => {
                    if (data.songs) {
                        setSongs((prevSongs) => ({
                            ...prevSongs,
                            [playlistId]: data.songs,
                        }));
                    }
                })
                .catch((err) => console.error("Error fetching songs:", err));
        }
    };

    const handlePortPlaylist = (spotifyPlaylistId) => {
        fetch("https://localhost:8080/port/spotify-to-youtube", {
            method: "POST",
            headers: {
                "Content-Type": "application/json",
            },
            body: JSON.stringify({
                spotifyPlaylistId,
            }),
        })
            .then((response) => response.json())
            .then((data) => {
                if (data.success) {
                    setPortStatus("Playlist ported successfully!");
                } else {
                    setPortStatus("Error porting playlist: " + data.error);
                }
            })
            .catch((err) => {
                console.error("Error porting playlist:", err);
                setPortStatus("Error porting playlist.");
            });
    };

    // Handle back button click
    const handleBackToHome = () => {
        navigate("/"); // Redirect to the landing page
    };

    return (
        <div>
            <h1 className="playlist-header">
                {service.charAt(0).toUpperCase() + service.slice(1)} Playlists
            </h1>
            <ul className="playlist-list">
                {playlists.map((playlist, index) => (
                    <li key={index} className="playlist-item">
                        {/* Title that expands/collapses songs */}
                        <div
                            className="playlist-title"
                            onClick={() => handleExpand(playlist.id)} // Expand on click
                        >
                            {playlist.name}
                        </div>

                        {/* "Port to YouTube" button separate from title */}
                        {service === "spotify" && (
                            <button
                                className="port-button"
                                onClick={(e) => {
                                    e.stopPropagation(); // Prevent expansion when clicking the button
                                    handlePortPlaylist(playlist.id);
                                }}
                            >
                                Port to YouTube
                            </button>
                        )}

                        {/* Expanded section for songs */}
                        {expandedPlaylist === playlist.id && (
                            <ul className="songs-list">
                                {songs[playlist.id] ? (
                                    songs[playlist.id].map((song, idx) => (
                                        <li key={idx} className="song-item">
                                            <span className="song-title">{song.title}</span>
                                            <br/>
                                            <span className="song-artist">{song.artist}</span>
                                        </li>
                                    ))
                                ) : (
                                    <li>Loading songs...</li>
                                )}
                            </ul>
                        )}
                    </li>
                ))}
            </ul>
            <button onClick={handleBackToHome} className="back-button">
                Back to Home
            </button>
            {/* Display porting status */}
            {portStatus && <p className="port-status">{portStatus}</p>}
        </div>
    );
};

export default Playlists;