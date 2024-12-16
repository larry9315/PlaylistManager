// src/components/Header.js
import React from "react";
import "./../styles/Home.css";

function Header() {
    return React.createElement(
        "header",
        { className: "header" },
        React.createElement("h1", null, "Playlist Manager"),
        React.createElement(
            "nav",
            { className: "nav" },
            React.createElement(
                "a",
                { href: "/", className: "nav-link" },
                "Home"
            ),
            React.createElement(
                "a",
                { href: "/about", className: "nav-link" },
                "About"
            )
        )
    );
}

export default Header;