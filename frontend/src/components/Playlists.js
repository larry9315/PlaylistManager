import React, { useEffect, useState } from "react";
import { useParams } from "react-router-dom"; // Import useParams
import "./../styles/Playlists.css";

const Playlists = () => {
    const { service } = useParams(); // Get the service parameter from the URL
    const [playlists, setPlaylists] = useState([]);

    useEffect(() => {
        // Fetch playlists for the specified service
        fetch(`https://localhost:8080/playlists/${service}`)
            .then((response) => response.json())
            .then((data) => {
                if (data.playlists) {
                    setPlaylists(data.playlists);
                }
            })
            .catch((err) => console.error("Error fetching playlists:", err));
    }, [service]);

    return (
        <div>
            <h1 className="playlist-header">{service.charAt(0).toUpperCase() + service.slice(1)} Playlists</h1>
            <ul>
                {playlists.map((playlist, index) => (
                    <li key={index}>{playlist}</li>
                ))}
            </ul>
        </div>
    );
};

export default Playlists;