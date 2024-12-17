import { BrowserRouter as Router, Routes, Route } from "react-router-dom";
import Home from "./components/Home";
import Playlists from "./components/Playlists";

function App() {
    return (
        <Router basename="/">
            <Routes>
                <Route path="/" element={<Home />} />
                <Route path="/playlists/youtube" element={<Playlists service="youtube" />} />
                <Route path="/playlists/spotify" element={<Playlists service="spotify" />} />
            </Routes>
        </Router>
    );
}

export default App;