import React from "react";
import ReactDOM from "react-dom/client"; // Import the new API for rendering
import App from "./App";

const root = ReactDOM.createRoot(document.getElementById("root"));
root.render(
    <React.StrictMode>
        <App />
    </React.StrictMode>
);