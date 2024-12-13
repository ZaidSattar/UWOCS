import React, { useState, useEffect } from 'react';
import { useWebSocket } from '../../contexts/WebSocketContext.tsx';
import { FaChevronDown, FaChevronRight, FaPlus, FaTrash, FaClock, FaCheck, FaTimes, FaCalculator } from 'react-icons/fa';
import '../styles/ComponentStyles.css';

const Term = ({ term, courses, onAddCourse, onRemoveCourse, status }) => {
    const [isExpanded, setIsExpanded] = useState(false);
    const [newCourse, setNewCourse] = useState('');

    const getStatusIcon = (courseStatus) => {
        switch(courseStatus) {
            case 'in_progress': return <FaClock className="status-icon in-progress" />;
            case 'completed': return <FaCheck className="status-icon completed" />;
            case 'incomplete': return <FaTimes className="status-icon incomplete" />;
            default: return null;
        }
    };

    return (
        <div className="term-section">
            <div className="term-header" onClick={() => setIsExpanded(!isExpanded)}>
                {isExpanded ? <FaChevronDown /> : <FaChevronRight />}
                <h4>{term}</h4>
            </div>
            {isExpanded && (
                <div className="term-content">
                    <div className="course-list">
                        {courses.map((course, index) => (
                            <div key={index} className="course-item">
                                {getStatusIcon(status)}
                                <span>{course}</span>
                                <FaTrash 
                                    className="remove-course" 
                                    onClick={() => onRemoveCourse(term, course)} 
                                />
                            </div>
                        ))}
                    </div>
                    <div className="add-course">
                        <input
                            type="text"
                            value={newCourse}
                            onChange={(e) => setNewCourse(e.target.value)}
                            placeholder="Enter course code"
                        />
                        <button onClick={() => {
                            if (newCourse) {
                                onAddCourse(term, newCourse);
                                setNewCourse('');
                            }
                        }}>
                            <FaPlus /> Add Course
                        </button>
                    </div>
                </div>
            )}
        </div>
    );
};

const Year = ({ year, yearData, onAddCourse, onRemoveCourse, courseStatus }) => {
    const [isExpanded, setIsExpanded] = useState(false);
    const terms = ['Fall Term', 'Winter Term', 'Summer Term'];

    return (
        <div className="year-section">
            <div className="year-header" onClick={() => setIsExpanded(!isExpanded)}>
                {isExpanded ? <FaChevronDown /> : <FaChevronRight />}
                <h3>Year {year}</h3>
            </div>
            {isExpanded && (
                <div className="year-content">
                    {terms.map(term => (
                        <Term
                            key={term}
                            term={term}
                            courses={yearData[term] || []}
                            onAddCourse={onAddCourse}
                            onRemoveCourse={onRemoveCourse}
                            status={courseStatus[term]}
                        />
                    ))}
                </div>
            )}
        </div>
    );
};

const StatusTable = ({ courseStatuses }) => {
    console.log("Course Statuses:", courseStatuses);

    return (
        <div className="status-table-container">
            <h3>Course Status Overview</h3>
            <div className="status-table">
                <div className="status-header">
                    <div>Course</div>
                    <div>Term</div>
                    <div>Year</div>
                    <div>Status</div>
                </div>
                {(!courseStatuses || Object.keys(courseStatuses).length === 0) ? (
                    <div className="status-row empty-state">
                        <div colSpan="4">No courses added yet</div>
                    </div>
                ) : (
                    Object.entries(courseStatuses).map(([year, terms]) => {
                        console.log(`Year ${year}:`, terms);
                        return Object.entries(terms).map(([term, courses]) => {
                            console.log(`Term ${term}:`, courses);
                            return courses.map((course, index) => (
                                <div key={`${year}-${term}-${index}`} className="status-row">
                                    <div>{course.code}</div>
                                    <div>{term}</div>
                                    <div>Year {year}</div>
                                    <div className={`status ${course.status}`}>
                                        {course.status === 'in_progress' && <FaClock />}
                                        {course.status === 'completed' && <FaCheck />}
                                        {course.status === 'incomplete' && <FaTimes />}
                                        {course.status}
                                    </div>
                                </div>
                            ));
                        });
                    })
                )}
            </div>
        </div>
    );
};

const CoursePlanner = () => {
    const { sendMessage, ws } = useWebSocket();
    const [currentYear, setCurrentYear] = useState(1);
    const [planData, setPlanData] = useState({});
    const [courseStatus, setCourseStatus] = useState({});
    const [courseStatuses, setCourseStatuses] = useState({});
    const [isLoading, setIsLoading] = useState(true);

    const calculateStatuses = () => {
        sendMessage({
            type: 'calculateCourseStatuses',
            planData: planData,
            currentYear
        });
    };

    useEffect(() => {
        if (!ws) return;

        const loadSavedData = () => {
            console.log("[DEBUG] Attempting to load saved planner data...");
            sendMessage({ type: 'loadPlanner' });
        };

        ws.addEventListener('open', loadSavedData);

        if (ws.readyState === WebSocket.OPEN) {
            console.log("[DEBUG] WebSocket is open, loading data immediately");
            loadSavedData();
        }

        const handleMessage = (event) => {
            const data = JSON.parse(event.data);
            console.log("[DEBUG] Received message:", data);
            
            if (data.type === 'plannerData') {
                console.log("[DEBUG] Setting planner data:", data.planData);
                setPlanData(data.planData || {});
                setCourseStatus(data.courseStatus || {});
                setCurrentYear(data.currentYear || 1);
                setIsLoading(false);

                if (Object.keys(data.planData || {}).length > 0) {
                    console.log("[DEBUG] Found existing courses, calculating statuses");
                    sendMessage({
                        type: 'calculateCourseStatuses',
                        planData: data.planData,
                        currentYear: data.currentYear
                    });
                }
            } else if (data.type === 'courseStatuses') {
                console.log("[DEBUG] Setting course statuses:", data.statuses);
                setCourseStatuses(data.statuses || {});
            }
        };

        ws.addEventListener('message', handleMessage);
        return () => {
            ws.removeEventListener('message', handleMessage);
            ws.removeEventListener('open', loadSavedData);
        };
    }, [ws]);

    const handleAddCourse = (year, term, course) => {
        const updatedData = { ...planData };
        if (!updatedData[year]) updatedData[year] = {};
        if (!updatedData[year][term]) updatedData[year][term] = [];
        updatedData[year][term].push(course);
        
        setPlanData(updatedData);
        sendMessage({
            type: 'updatePlanner',
            planData: updatedData,
            currentYear
        });
        sendMessage({
            type: 'calculateCourseStatuses',
            planData: updatedData,
            currentYear
        });
    };

    const handleRemoveCourse = (year, term, course) => {
        const updatedData = { ...planData };
        if (updatedData[year]?.[term]) {
            updatedData[year][term] = updatedData[year][term].filter(c => c !== course);
        }
        
        setPlanData(updatedData);
        sendMessage({
            type: 'updatePlanner',
            planData: updatedData,
            currentYear
        });
        sendMessage({
            type: 'calculateCourseStatuses',
            planData: updatedData,
            currentYear
        });
    };

    if (isLoading) {
        return <div>Loading course planner...</div>;
    }

    return (
        <div className="course-planner">
            <h2>Course Planner</h2>
            
            <div className="current-year-section">
                <h3>Current Year</h3>
                <select 
                    value={currentYear} 
                    onChange={(e) => {
                        const year = parseInt(e.target.value);
                        setCurrentYear(year);
                        sendMessage({
                            type: 'updateCurrentYear',
                            currentYear: year
                        });
                    }}
                >
                    {[1, 2, 3, 4].map(year => (
                        <option key={year} value={year}>Year {year}</option>
                    ))}
                </select>
            </div>

            <div className="years-container">
                {[1, 2, 3, 4].map(year => (
                    <Year
                        key={year}
                        year={year}
                        yearData={planData[year] || {}}
                        onAddCourse={(term, course) => handleAddCourse(year, term, course)}
                        onRemoveCourse={(term, course) => handleRemoveCourse(year, term, course)}
                        courseStatus={courseStatus[year] || {}}
                    />
                ))}
            </div>

            <button className="calculate-status-button" onClick={calculateStatuses}>
                <FaCalculator /> Refresh Course Statuses
            </button>

            <StatusTable courseStatuses={courseStatuses || {}} />
        </div>
    );
};

export default CoursePlanner;