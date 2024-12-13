import React, { useState } from 'react';
import { useWebSocket } from '../../contexts/WebSocketContext.tsx';
import { FaPlus, FaCalculator, FaTimes, FaBook, FaPercent, FaWeight } from 'react-icons/fa';

const CourseForm = ({ course, onUpdate, onRemove }) => {
    return (
        <div className="gpa-course-entry">
            <div className="course-input-group">
                <FaBook className="course-icon" />
                <input
                    type="text"
                    className="course-input"
                    placeholder="Course Code"
                    value={course.name}
                    onChange={(e) => onUpdate({ ...course, name: e.target.value })}
                />
            </div>
            <div className="course-input-group">
                <FaPercent className="course-icon" />
                <input
                    type="number"
                    className="course-input"
                    placeholder="Grade %"
                    min="0"
                    max="100"
                    value={course.grade}
                    onChange={(e) => onUpdate({ ...course, grade: parseFloat(e.target.value) })}
                />
            </div>
            <div className="course-input-group">
                <FaWeight className="course-icon" />
                <select
                    className="course-select"
                    value={course.weight}
                    onChange={(e) => onUpdate({ ...course, weight: parseFloat(e.target.value) })}
                >
                    <option value="0.5">0.5 Credits</option>
                    <option value="1">1.0 Credits</option>
                </select>
            </div>
            <button className="course-remove-btn" onClick={() => onRemove(course.id)}>
                <FaTimes />
            </button>
        </div>
    );
};

const GPACalculator = () => {
    const { sendMessage, ws } = useWebSocket();
    const [courses, setCourses] = useState([]);
    const [gpa, setGpa] = useState(null);

    const addCourse = () => {
        setCourses([
            ...courses,
            {
                id: Date.now(),
                name: '',
                grade: '',
                weight: ''
            }
        ]);
    };

    const updateCourse = (updatedCourse) => {
        setCourses(courses.map(course => 
            course.id === updatedCourse.id ? updatedCourse : course
        ));
    };

    const removeCourse = (courseId) => {
        setCourses(courses.filter(course => course.id !== courseId));
    };

    const calculateGPA = () => {
        // Validate inputs
        const invalidCourses = courses.filter(course => 
            !course.name || 
            course.grade === '' || 
            course.weight === '' ||
            course.grade < 0 || 
            course.grade > 100 ||
            course.weight < 0
        );

        if (invalidCourses.length > 0) {
            alert('Please fill in all fields correctly');
            return;
        }

        // Send to backend for calculation
        sendMessage({
            type: 'calculateGPA',
            courses: courses.map(({ name, grade, weight }) => ({ name, grade, weight }))
        });
    };

    // Update the WebSocket message handler
    React.useEffect(() => {
        if (!ws) return;

        const handleWebSocketMessage = (event) => {
            try {
                const data = JSON.parse(event.data);
                console.log("Received WebSocket response:", data);
                
                if (data.type === "gpaCalculated") {
                    setGpa(data.gpa);
                }
            } catch (error) {
                console.error("Error handling WebSocket message:", error);
            }
        };

        // Add message listener to existing WebSocket
        ws.addEventListener('message', handleWebSocketMessage);

        // Cleanup
        return () => {
            ws.removeEventListener('message', handleWebSocketMessage);
        };
    }, [ws]);

    return (
        <div className="gpa-calculator">
            <div className="calculator-header">
                <h2><FaCalculator /> GPA Calculator</h2>
                <button onClick={addCourse}><FaPlus /> Add Course</button>
            </div>

            {/* Individual course entries */}
            {courses.map(course => (
                <div key={course.id} className="course-entry-container">
                    <CourseForm
                        course={course}
                        onUpdate={updateCourse}
                        onRemove={removeCourse}
                    />
                </div>
            ))}

            {courses.length > 0 && (
                <div className="calculator-actions">
                    <button className="calculate-button" onClick={calculateGPA}>
                        <FaCalculator /> Calculate GPA
                    </button>
                </div>
            )}

            {gpa !== null && (
                <div className="gpa-result">
                    <h3>Your GPA: {gpa.toFixed(2)}%</h3>
                    <p className="gpa-explanation">
                        Calculated using weighted average: Σ(grade × weight) / Σ(weight)
                    </p>
                </div>
            )}
        </div>
    );
};

export default GPACalculator;