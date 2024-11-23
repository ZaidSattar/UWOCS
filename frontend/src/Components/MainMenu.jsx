import React, { useState } from 'react';
import { Route, Link, Routes, Navigate } from 'react-router-dom';
import './styles/MainMenu.css';

// Import menu components
import GraduationProgress from './menucomponents/GraduationProgress.jsx';
import ScheduleManagement from './menucomponents/ScheduleManagement.jsx';
import CoursePlanner from './menucomponents/CoursePlanner.jsx';
import GPACalculator from './menucomponents/GPACalculator.jsx';
import DocumentRepository from './menucomponents/DocumentRepository.jsx';

const MainMenu = () => {
    const [activeItem, setActiveItem] = useState('graduation-progress');

    const menuItems = [
        { path: '/graduation-progress', icon: '🎓', title: 'Graduation Progress', description: 'Track your academic journey' },
        { path: '/schedule-management', icon: '📅', title: 'Schedule Management', description: 'Organize your academic calendar' },
        { path: '/course-planner', icon: '📚', title: 'Course Planner', description: 'Plan your future semesters' },
        { path: '/gpa-calculator', icon: '🎯', title: 'GPA Calculator', description: 'Calculate and track your GPA' },
        { path: '/document-repository', icon: '📁', title: 'Document Repository', description: 'Access your academic documents' }
    ];

    return (
        <div className="main-menu">
            <div className="menu-sidebar">
                <div className="profile-section">
                    <div className="profile-avatar">
                        <span>👤</span>
                    </div>
                    <h2>Welcome Back</h2>
                    <p> Fellow UWO Computer Science Student</p>
                </div>
                <div className="menu-items">
                    {menuItems.map((item) => (
                        <Link
                            key={item.path}
                            to={item.path}
                            className={`menu-item ${activeItem === item.path.slice(1) ? 'active' : ''}`}
                            onClick={() => setActiveItem(item.path.slice(1))}
                        >
                            <div className="menu-item-icon">{item.icon}</div>
                            <div className="menu-item-content">
                                <h3>{item.title}</h3>
                                <p>{item.description}</p>
                            </div>
                            <div className="menu-item-arrow">→</div>
                        </Link>
                    ))}
                </div>
            </div>
            <div className="menu-content">
                <div className="content-header">
                    <h1>{menuItems.find(item => item.path.slice(1) === activeItem)?.title}</h1>
                </div>
                <div className="content-body">
                    <Routes>
                        <Route path="/" element={<Navigate to="/graduation-progress" replace />} />
                        <Route path="/graduation-progress" element={<GraduationProgress />} />
                        <Route path="/schedule-management" element={<ScheduleManagement />} />
                        <Route path="/course-planner" element={<CoursePlanner />} />
                        <Route path="/gpa-calculator" element={<GPACalculator />} />
                        <Route path="/document-repository" element={<DocumentRepository />} />
                    </Routes>
                </div>
            </div>
        </div>
    );
};

export default MainMenu;