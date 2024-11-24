import React, { useState, useMemo, useEffect } from 'react';
import { Calendar, momentLocalizer, Views } from 'react-big-calendar';
import moment from 'moment';
import { useWebSocket } from '../../contexts/WebSocketContext.tsx';
import { FaCalendarAlt, FaPlus, FaClock, FaBook, FaInfoCircle, FaCheck, FaTimes } from 'react-icons/fa';
import 'react-big-calendar/lib/css/react-big-calendar.css';

const localizer = momentLocalizer(moment);

const EventComponent = ({ event }) => {
    const [isCardOpen, setIsCardOpen] = useState(false);
    const [position, setPosition] = useState({ x: 0, y: 0 });
    const [isDragging, setIsDragging] = useState(false);
    const [dragStart, setDragStart] = useState({ x: 0, y: 0 });

    const handleClick = (e) => {
        e.stopPropagation();
        setIsCardOpen(!isCardOpen);
        setPosition({ x: 0, y: 0 });
    };

    const handleMouseDown = (e) => {
        if (e.target.closest('.close-button')) return;
        setIsDragging(true);
        setDragStart({
            x: e.clientX - position.x,
            y: e.clientY - position.y
        });
    };

    const handleMouseMove = (e) => {
        if (!isDragging) return;
        setPosition({
            x: e.clientX - dragStart.x,
            y: e.clientY - dragStart.y
        });
    };

    const handleMouseUp = () => {
        setIsDragging(false);
    };

    useEffect(() => {
        if (isDragging) {
            document.addEventListener('mousemove', handleMouseMove);
            document.addEventListener('mouseup', handleMouseUp);
        }
        return () => {
            document.removeEventListener('mousemove', handleMouseMove);
            document.removeEventListener('mouseup', handleMouseUp);
        };
    }, [isDragging, dragStart]);

    return (
        <div className="calendar-event-wrapper">
            <button
                className="calendar-event-circle"
                onClick={handleClick}
            >
                <FaBook />
            </button>
            {isCardOpen && (
                <div
                    className="calendar-event-card"
                    style={{
                        transform: `translate(calc(-50% + ${position.x}px), calc(-50% + ${position.y}px))`,
                        cursor: isDragging ? 'grabbing' : 'grab'
                    }}
                    onMouseDown={handleMouseDown}
                    onClick={(e) => e.stopPropagation()}
                >
                    <div className="card-header">
                        <h4>{event.title}</h4>
                        <button
                            className="close-button"
                            onClick={(e) => {
                                e.stopPropagation();
                                setIsCardOpen(false);
                            }}
                        >
                            <FaTimes />
                        </button>
                    </div>
                    <div className="event-details">
                        <p><FaBook /> Course: {event.course}</p>
                        <p><FaClock /> Date: {moment(event.start).format('MMMM Do, h:mm a')}</p>
                        <p><FaInfoCircle /> Type: {event.type}</p>
                        <p><FaInfoCircle /> Description: {event.description}</p>
                        <p><FaClock /> Reminder: {event.reminderDays} days before</p>
                    </div>
                </div>
            )}
        </div>
    );
};


const EventForm = ({ newEvent, setNewEvent, handleAddEvent, onClose }) => {
    return (
        <div className="event-form-overlay">
            <form className="event-form" onSubmit={(e) => {
                e.preventDefault();
                handleAddEvent();
            }}>
                <h3><FaCalendarAlt /> Add New Event</h3>
                <div className="form-group">
                    <FaInfoCircle className="form-icon" />
                    <input
                        type="text"
                        placeholder="Event Title"
                        value={newEvent.title}
                        onChange={e => setNewEvent({...newEvent, title: e.target.value})}
                    />
                </div>
                <div className="form-group">
                    <FaBook className="form-icon" />
                    <select 
                        value={newEvent.type}
                        onChange={e => setNewEvent({...newEvent, type: e.target.value})}
                    >
                        <option value="assignment">📝 Assignment</option>
                        <option value="exam">📄 Exam</option>
                        <option value="project">🎯 Project</option>
                        <option value="quiz">✍️ Quiz</option>
                        <option value="tutorial">👥 Tutorial</option>
                        <option value="participation">🗣️ In-Person Participation</option>
                        <option value="lab">🔬 Laboratory</option>
                        <option value="presentation">🎤 Presentation</option>
                        <option value="groupwork">👥 Group Work</option>
                        <option value="deadline">⏰ Important Deadline</option>
                        <option value="meeting">🤝 Meeting</option>
                        <option value="workshop">🛠️ Workshop</option>
                    </select>
                </div>
                <div className="form-group">
                    <FaClock className="form-icon" />
                    <input
                        type="datetime-local"
                        value={moment(newEvent.date).format('YYYY-MM-DDTHH:mm')}
                        onChange={e => setNewEvent({...newEvent, date: new Date(e.target.value)})}
                    />
                </div>
                <div className="form-group">
                    <FaBook className="form-icon" />
                    <input
                        type="text"
                        placeholder="Course Code"
                        value={newEvent.course}
                        onChange={e => setNewEvent({...newEvent, course: e.target.value})}
                    />
                </div>
                <div className="form-group">
                    <FaInfoCircle className="form-icon" />
                    <textarea
                        placeholder="Description"
                        value={newEvent.description}
                        onChange={e => setNewEvent({...newEvent, description: e.target.value})}
                    />
                </div>
                <div className="form-group">
                    <FaClock className="form-icon" />
                    <input
                        type="number"
                        placeholder="Reminder Days Before"
                        value={newEvent.reminderDays}
                        onChange={e => setNewEvent({...newEvent, reminderDays: parseInt(e.target.value)})}
                    />
                </div>
                <div className="form-buttons">
                    <button type="button" className="btn-cancel" onClick={onClose}>
                        <FaTimes /> Cancel
                    </button>
                    <button type="submit" className="btn-save">
                        <FaCheck /> Save Event
                    </button>
                </div>
            </form>
        </div>
    );
};

const CustomToolbar = () => null;

const ScheduleManagement = () => {
    const { ws, isConnected, scheduleEvents, addScheduleEvent } = useWebSocket();
    const [showAddForm, setShowAddForm] = useState(false);
    const [newEvent, setNewEvent] = useState({
        title: '',
        type: 'assignment',
        date: new Date(),
        description: '',
        course: '',
        reminderDays: 1
    });

    const testEvents = [
        {
            id: '1',
            title: 'Final',
            start: new Date(), 
            end: new Date(),  
            course: 'CS3307',
            type: 'Exam',
            description: 'Midterm',
            reminderDays: 1
        },
        {
            id: '2',
            title: 'Test Event',
            start: new Date(),
            end: new Date(),   
            course: 'CS101',
            type: 'exam',
            description: 'Test description',
            reminderDays: 1
        }
    ];

    return (
        <div className="schedule-container">
            <div className="schedule-header">
                <h2><FaCalendarAlt /> Schedule Management</h2>
                <button onClick={() => setShowAddForm(true)}><FaPlus /> Add Event</button>
            </div>
            
            {showAddForm && (
                <EventForm 
                    newEvent={newEvent}
                    setNewEvent={setNewEvent}
                    handleAddEvent={() => {
                        if (!newEvent.title) {
                            alert('Please enter an event title');
                            return;
                        }
                        if (!newEvent.date) {
                            alert('Please select a date');
                            return;
                        }
                        if (!newEvent.course) {
                            alert('Please enter a course code');
                            return;
                        }
                        
                        try {
                            addScheduleEvent(newEvent);
                            setShowAddForm(false);
                            setNewEvent({
                                title: '',
                                type: 'assignment',
                                date: new Date(),
                                description: '',
                                course: '',
                                reminderDays: 1
                            });
                        } catch (error) {
                            alert('Failed to add event. Please try again.');
                            console.error('Error adding event:', error);
                        }
                    }}
                    onClose={() => setShowAddForm(false)}
                />
            )}
            
            <Calendar
                localizer={localizer}
                events={testEvents}
                defaultView="month"
                views={["month"]}
                startAccessor="start"
                endAccessor="end"
                style={{ 
                    height: 500,
                    margin: '20px',
                    background: 'rgba(255, 255, 255, 0.05)',
                    borderRadius: '15px',
                    padding: '15px'
                }}
                components={{
                    event: EventComponent 
                }}
            />
        </div>
    );
};

export default ScheduleManagement; 