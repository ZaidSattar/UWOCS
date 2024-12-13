import React, { useState } from 'react';
import { useWebSocket } from '../../contexts/WebSocketContext.tsx';
import { FaCheckCircle, FaCircle, FaChevronDown, FaChevronRight, FaGraduationCap, FaCalculator } from 'react-icons/fa';

const ModuleCard = ({ title, description, isSelected, onClick }) => (
    <div 
        className={`module-card ${isSelected ? 'selected' : ''}`}
        onClick={onClick}
    >
        <div className="module-header">
            <h3>{title}</h3>
            {isSelected ? <FaChevronDown /> : <FaChevronRight />}
        </div>
        <p className="module-description">{description}</p>
    </div>
);

const CourseCheckbox = ({ course, isCompleted, onToggle, isDisabled }) => (
    <div 
        className={`course-checkbox ${isDisabled ? 'disabled' : ''}`} 
        onClick={() => !isDisabled && onToggle()}
    >
        {isCompleted ? 
            <FaCheckCircle className="check-icon completed" /> : 
            <FaCircle className="check-icon" />
        }
        <span>{course}</span>
    </div>
);

const CourseGroup = ({ title, courses, requiredCredits, completedCourses, onCourseToggle }) => {
    // Calculate how many courses can be selected (2 courses = 1.0 credit)
    const maxSelectable = requiredCredits * 2;
    const selectedCount = courses.filter(course => completedCourses.includes(course)).length;

    return (
        <div className="course-group">
            <div className="course-group-header">
                <h4>{title}</h4>
                <span className="course-count">
                    Selected: {selectedCount} / {maxSelectable} courses
                </span>
            </div>
            <div className="course-list">
                {courses.map(course => (
                    <CourseCheckbox
                        key={course}
                        course={course}
                        isCompleted={completedCourses.includes(course)}
                        onToggle={() => {
                            // Only allow toggle if under max or removing
                            if (!completedCourses.includes(course) && selectedCount >= maxSelectable) {
                                alert(`You can only select ${maxSelectable} courses from this group`);
                                return;
                            }
                            onCourseToggle(course);
                        }}
                        isDisabled={!completedCourses.includes(course) && selectedCount >= maxSelectable}
                    />
                ))}
            </div>
        </div>
    );
};

const GraduationProgress = () => {
    const { sendMessage, ws } = useWebSocket();
    const [selectedModule, setSelectedModule] = useState(null);
    const [moduleData, setModuleData] = useState(null);
    const [completedCourses, setCompletedCourses] = useState([]);
    const [progress, setProgress] = useState(null);

    const modules = [
        {
            id: 'honors_cs',
            title: 'Honors Specialization in Computer Science',
            description: 'This is the program that will give you the most concentrated background and deep knowledge of almost all areas of computer science. In order to receive the Honors designation, you must maintain a 70% average and you must get at least a 60% in every required course.'
        },
        {
            id: 'honors_is',
            title: 'Honors Specialization in Information Systems',
            description: 'This program provides a solid foundation in computing and information systems with a focus towards applications of computing in modern enterprises and organizations. This program was developed in consultation with the Ivey Business School, and is a perfect fit for the combined HBA program, discussed under Interdisciplinary Programs.'
        },
        {
            id: 'honors_bio',
            title: 'Honors Specialization in Bioinformatics',
            description: 'This is an interdisciplinary program focussed on the study of methods for storing, retrieving, organizing, and analyzing biological data. Doing so entails the development of new software tools and techniques for generating and making sense of new useful biological knowledge. This is an elite, intense, and extremely rewarding program at Western.'
        },
        {
            id: 'spec_cs',
            title: 'Specialization in Computer Science',
            description: 'This program will give a strong background in almost all areas of computer science. You could learn about graphics, networks, security, databases, the internet, computer law. This module offers a wealth of possiblities!'
        },
        {
            id: 'major_cs',
            title: 'Major in Computer Science',
            description: 'This program provides a solid background in computer science, covering the foundational elements of the discipline. While perhaps best used as part of a Double Major (see Add-On Modules for Other Degrees), this module can be used as a basis for a degree in Computer Science as well.'
        },
        {
            id: 'minor_game',
            title: 'Minor in Game Development',
            description: 'Love playing games, then why not get in the driver\'s seat and write the games? This module will teach you not only about choices to make when designing a game, but also things to consider when dealing with the hardware that the game will run on. Western is one of the first two universities in Canada to offer a program devoted to game development.'
        },
        {
            id: 'minor_se',
            title: 'Minor in Software Engineering',
            description: 'The minor is available to students in either the Honours Specialization or Specialization program. This option offers a set of courses related to software engineering that will provide students with an in-depth knowledge needed to design and manage large software projects.'
        }
    ];

    const handleModuleSelect = (moduleId) => {
        setSelectedModule(moduleId);
        sendMessage({
            type: 'getModuleRequirements',
            moduleId: moduleId
        });
    };

    const handleCourseToggle = (course) => {
        setCompletedCourses(prev => {
            const newCompleted = prev.includes(course)
                ? prev.filter(c => c !== course)
                : [...prev, course];
            return newCompleted;
        });
    };

    const calculateProgress = () => {
        sendMessage({
            type: 'calculateProgress',
            moduleId: selectedModule,
            completedCourses: completedCourses
        });
    };

    // Add WebSocket message handler
    React.useEffect(() => {
        if (!ws) return;

        // Load saved progress when component mounts
        sendMessage({
            type: 'loadProgress'
        });

        const handleMessage = (event) => {
            const data = JSON.parse(event.data);
            if (data.type === 'moduleRequirements') {
                setModuleData(data.requirements);
            } else if (data.type === 'progressCalculated') {
                setProgress(data.progress);
            } else if (data.type === 'savedProgress') {
                // Set saved module and completed courses
                setSelectedModule(data.moduleId);
                setCompletedCourses(data.completedCourses);
                // Request module requirements for saved module
                if (data.moduleId) {
                    sendMessage({
                        type: 'getModuleRequirements',
                        moduleId: data.moduleId
                    });
                }
            }
        };

        ws.addEventListener('message', handleMessage);
        return () => ws.removeEventListener('message', handleMessage);
    }, [ws]);

    return (
        <div className="graduation-progress">
            <div className="progress-header">
                <h2><FaGraduationCap /> Graduation Progress Tracking</h2>
            </div>

            <div className="modules-grid">
                {modules.map(module => (
                    <ModuleCard
                        key={module.id}
                        {...module}
                        isSelected={selectedModule === module.id}
                        onClick={() => handleModuleSelect(module.id)}
                    />
                ))}
            </div>

            {moduleData && (
                <div className="requirements-section">
                    <h3>Module Requirements</h3>
                    {moduleData.courseGroups.map((group, index) => (
                        <CourseGroup
                            key={index}
                            {...group}
                            completedCourses={completedCourses}
                            onCourseToggle={handleCourseToggle}
                        />
                    ))}

                    <button className="calculate-progress" onClick={calculateProgress}>
                        <FaCalculator /> Track Progress
                    </button>

                    {progress && (
                        <div className="progress-results">
                            <div className="progress-bar">
                                <div 
                                    className="progress-fill"
                                    style={{ width: `${progress.percentage}%` }}
                                />
                            </div>
                            <div className="progress-stats">
                                <div className="stat">
                                    <h4>Completed</h4>
                                    <p>{progress.completed} courses</p>
                                </div>
                                <div className="stat">
                                    <h4>Remaining</h4>
                                    <p>{progress.remaining} courses</p>
                                </div>
                                <div className="stat">
                                    <h4>Progress</h4>
                                    <p>{progress.percentage}%</p>
                                </div>
                            </div>
                        </div>
                    )}
                </div>
            )}
        </div>
    );
};

export default GraduationProgress;
