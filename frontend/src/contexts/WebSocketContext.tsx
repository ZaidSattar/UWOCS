import React, { createContext, useContext, useEffect, useState } from 'react';

interface ScheduleEvent {
    id: string;
    title: string;
    type: string;
    date: Date;
    description: string;
    course: string;
    reminderDays: number;
}

interface WebSocketContextType {
    ws: WebSocket | null;
    isConnected: boolean;
    scheduleEvents: ScheduleEvent[];
    sendMessage: (message: any) => void;
    addScheduleEvent: (event: Omit<ScheduleEvent, 'id'>) => void;
    deleteScheduleEvent: (id: string) => void;
    updateScheduleEvent: (event: ScheduleEvent) => void;
}

const WebSocketContext = createContext<WebSocketContextType>({
    ws: null,
    isConnected: false,
    scheduleEvents: [],
    sendMessage: () => {},
    addScheduleEvent: () => {},
    deleteScheduleEvent: () => {},
    updateScheduleEvent: () => {}
});

export const WebSocketProvider: React.FC<{ children: React.ReactNode }> = ({ children }) => {
    const [ws, setWs] = useState<WebSocket | null>(null);
    const [isConnected, setIsConnected] = useState(false);
    const [scheduleEvents, setScheduleEvents] = useState<ScheduleEvent[]>([]);

    useEffect(() => {
        const websocket = new WebSocket('ws://localhost:8080');

        websocket.onopen = () => {
            console.log('WebSocket Connected');
            setIsConnected(true);
            // Request initial schedule data
            websocket.send(JSON.stringify({ type: 'getSchedule' }));
        };

        websocket.onclose = () => {
            console.log('WebSocket Disconnected');
            setIsConnected(false);
        };

        websocket.onerror = (error) => {
            console.error('WebSocket error:', error);
        };

        websocket.onmessage = (event) => {
            const data = JSON.parse(event.data);
            console.log("Raw WebSocket data:", data);
            
            if (Array.isArray(data)) {
                console.log("Processing schedule events array");
                const updatedEvents = data.map(event => ({
                    ...event,
                    date: event.date
                }));
                console.log("Processed events:", updatedEvents);
                setScheduleEvents(updatedEvents);
            } else if (data.type === 'eventAdded') {
                console.log("Event added:", data.event);
                websocket.send(JSON.stringify({ type: 'getSchedule' }));
            }
        };

        setWs(websocket);

        return () => {
            websocket.close();
        };
    }, []);

    const sendMessage = (message: any) => {
        if (ws && isConnected) {
            ws.send(JSON.stringify(message));
        }
    };

    const addScheduleEvent = (event: Omit<ScheduleEvent, 'id'>) => {
        if (ws && isConnected) {
            try {
                console.log("Sending new event:", event);
                const message = {
                    type: 'addScheduleEvent',
                    event: {
                        ...event,
                        date: Math.floor(event.date.getTime() / 1000)
                    }
                };
                console.log("Sending message:", message);
                ws.send(JSON.stringify(message));
            } catch (error) {
                console.error("Error sending event:", error);
            }
        } else {
            console.error("WebSocket not connected");
        }
    };

    const deleteScheduleEvent = (id: string) => {
        if (ws && isConnected) {
            sendMessage({
                type: 'deleteScheduleEvent',
                id
            });
        }
    };

    const updateScheduleEvent = (event: ScheduleEvent) => {
        if (ws && isConnected) {
            sendMessage({
                type: 'updateScheduleEvent',
                event
            });
        }
    };

    return (
        <WebSocketContext.Provider 
            value={{ 
                ws, 
                isConnected, 
                scheduleEvents,
                sendMessage,
                addScheduleEvent,
                deleteScheduleEvent,
                updateScheduleEvent
            }}
        >
            {children}
        </WebSocketContext.Provider>
    );
};

export const useWebSocket = () => useContext(WebSocketContext);