# Western Computer Science Student Portal

A comprehensive web application for Western University Computer Science students to manage their academic journey. Built with React frontend and C++ backend using WebSocket communication.

## Features

### Course Planner
- Plan courses across all 4 years of your degree
- Organize courses by Fall, Winter and Summer terms
- Track course completion status (completed, in progress, incomplete)
- Add/remove courses from each term
- Calculate course prerequisites and requirements
- View course status updates in real-time

### GPA Calculator
- Calculate cumulative GPA
- Add/remove courses with their grades and weights
- Automatic GPA computation based on Western's grading scale
- Persistent storage of GPA records
- Real-time updates as grades are modified

### Graduation Progress Tracking
- Monitor progress towards degree completion
- Track completed required courses
- View remaining requirements
- Support for different modules (e.g. Honors Computer Science)
- Visual progress indicators
- Automatic status updates as courses are completed

### Schedule Management
- Interactive calendar interface
- Add course assignments, tests and deadlines
- Set reminders for upcoming events
- Filter events by course
- Color-coded event categories
- Automatic reminder notifications

### Document Repository
- Store and access course materials
- Upload/download PDF documents
- Search functionality for quick access
- Organized by course
- Preview documents before downloading
- Secure document storage

## Prerequisites

- Node.js (v14 or higher)
- CMake (v3.10 or higher)
- C++ Compiler (supporting C++17)
- Boost Library
- nlohmann/json Library

## Installation & Setup

### Backend Setup

1. Navigate to backend directory:
```bash
cd backend
```

2. Navigate to build directory:
```bash
 cd build
```

3. Generate build files with CMake:
```bash
cmake ..
```

4. Build the project:
```bash
make
```

5. Run the server:
```bash
./server
```

The WebSocket server will start running on `ws://localhost:8080`

### Frontend Setup

1. Open a new terminal and navigate to frontend directory:
```bash
cd frontend
```

2. Install dependencies:
```bash
npm install
```

3. Start the development server:
```bash
npm start
```

The application will open automatically in your default browser at `http://localhost:3000`

## Project Structure

```
.
├── backend/
│   ├── include/          # Header files
│   ├── src/             # C++ source files
│   └── storage/         # Data storage files
├── frontend/
│   ├── public/          # Static files
│   └── src/             # React components and logic
│       ├── Components/  # React components
│       ├── contexts/    # React contexts
│       └── styles/      # CSS files
```

## Dependencies

### Backend
- Boost.Beast (WebSocket)
- nlohmann/json
- C++17 Standard Library

### Frontend
- React
- React Router
- React Icons
- WebSocket API

## Development

- Backend runs on port 8080 handling WebSocket connections
- Frontend development server runs on port 3000
- Real-time communication between frontend and backend via WebSocket
- Data persistence handled in backend storage files
