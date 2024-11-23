import React, { useEffect, useState } from "react";
import "./App.css";
import MainMenu from './Components/MainMenu.jsx';
import { WebSocketProvider } from './contexts/WebSocketContext.tsx';

const App = () => {
    const [message, setMessage] = useState("");
    const [started, setStarted] = useState(false);
    const [mousePosition, setMousePosition] = useState({ x: 0, y: 0 });

    useEffect(() => {
        const handleMouseMove = (e) => {
            setMousePosition({ x: e.clientX, y: e.clientY });
        };
        window.addEventListener('mousemove', handleMouseMove);
        return () => window.removeEventListener('mousemove', handleMouseMove);
    }, []);

    const features = [
        {
            title: "Degree Navigator",
            description: "Track your progress towards graduation with real-time updates",
            icon: "🎓"
        },
        {
            title: "Smart Scheduler",
            description: "AI-powered course planning and conflict resolution",
            icon: "📅"
        },
        {
            title: "Resource Hub",
            description: "Access course materials and calculate GPA instantly",
            icon: "📚"
        },
        {
            title: "Prerequisites Check",
            description: "Verify course eligibility and requirements",
            icon: "✓"
        }
    ];

    return (
        <WebSocketProvider>
            <div className="App">
                <div className="background-grid" />
                <div className="cursor-glow" style={{ left: mousePosition.x, top: mousePosition.y }} />
                {!started ? (
                    <div className="landing-container">
                        <div className="title-container">
                            <div className="logo-container">
                                <img src="/uwologo.png" alt="Western Logo" className="western-logo" />
                            </div>
                            <h1 className="main-title">Western Computer Science Portal</h1>
                            <p className="subtitle">Your Academic Journey, Simplified</p>
                        </div>
                        <button className="start-button" onClick={() => setStarted(true)}>
                            <span className="button-text">Launch Portal</span>
                            <span className="button-glow" />
                        </button>
                        <div className="feature-cards">
                            {features.map((feature, index) => (
                                <div key={index} className="feature-card">
                                    <div className="feature-icon">{feature.icon}</div>
                                    <h3>{feature.title}</h3>
                                    <p>{feature.description}</p>
                                </div>
                            ))}
                        </div>
                    </div>
                ) : (
                    <MainMenu />
                )}
            </div>
        </WebSocketProvider>
    );
};

export default App;
