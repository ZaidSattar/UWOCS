#include "../include/ScheduleHandler.h"
#include <fstream>
#include <sstream>
#include <chrono>
#include <random>
#include <algorithm>
#include <iostream>
#include <filesystem>
#include <cstring>
#include <cerrno>

ScheduleHandler::ScheduleHandler() {
    loadEvents();
}

std::string ScheduleHandler::getSchedule() {
    loadEvents();  // Make sure to load events first
    
    json eventsArray = json::array();
    for (const auto& event : events) {
        json eventJson = {
            {"id", event.id},
            {"title", event.title},
            {"type", event.type},
            {"date", event.date},
            {"description", event.description},
            {"course", event.course},
            {"reminderDays", event.reminderDays}
        };
        eventsArray.push_back(eventJson);
    }
    
    std::string jsonStr = eventsArray.dump();
    std::cout << "Debug: Sending schedule to frontend: " << jsonStr << std::endl;
    return jsonStr;
}

bool ScheduleHandler::addEvent(const json& eventData) {
    try {
        std::cout << "Debug: Starting to add event" << std::endl;
        
        ScheduleEvent event;
        event.id = generateId();
        event.title = eventData["title"];
        event.type = eventData["type"];
        event.description = eventData["description"];
        event.course = eventData["course"];
        event.reminderDays = eventData["reminderDays"];
        
        // Convert numeric timestamp to time_t
        event.date = static_cast<std::time_t>(eventData["date"]);
        
        events.push_back(event);
        saveEvents();  // Use the existing saveEvents() function instead of direct file writing
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error adding event: " << e.what() << std::endl;
        return false;
    }
}

void ScheduleHandler::checkReminders() {
    auto now = std::chrono::system_clock::now();
    auto currentTime = std::chrono::system_clock::to_time_t(now);
    
    std::vector<ScheduleEvent> upcomingEvents;
    for (const auto& event : events) {
        double diffHours = std::difftime(event.date, currentTime) / 3600.0;
        if (diffHours <= event.reminderDays * 24 && diffHours > 0) {
            upcomingEvents.push_back(event);
        }
    }
    
    if (!upcomingEvents.empty()) {
        sendReminders();
    }
}

void ScheduleHandler::saveEvents() {
    std::cout << "Debug: Starting to save events" << std::endl;
    
    std::stringstream ss;
    ss << "SCHEDULE EVENTS\n";
    ss << "---------------\n\n";
    
    for (const auto& event : events) {
        ss << "Event ID: " << event.id << "\n";
        ss << "Title: " << event.title << "\n";
        ss << "Type: " << event.type << "\n";
        ss << "Date: " << event.date << "\n";
        ss << "Description: " << event.description << "\n";
        ss << "Course: " << event.course << "\n";
        ss << "Reminder Days: " << event.reminderDays << "\n";
        ss << "---------------\n\n";
    }
    
    try {
        std::filesystem::create_directories(std::filesystem::path(SCHEDULE_PATH).parent_path());
        
        // Open file in text mode
        std::ofstream file(SCHEDULE_PATH, std::ios::out | std::ios::trunc);
        if (!file) {
            throw std::runtime_error("Failed to open file");
        }
        
        std::string content = ss.str();
        file << content;
        
        // Force write to disk
        file.flush();
        file.close();
        
        std::cout << "Debug: Wrote content:\n" << content << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error saving events: " << e.what() << std::endl;
    }
}

void ScheduleHandler::loadEvents() {
    std::cout << "Debug: Starting to load events" << std::endl;
    events.clear();
    
    try {
        if (!std::filesystem::exists(SCHEDULE_PATH)) {
            return;
        }
        
        std::ifstream file(SCHEDULE_PATH);
        if (!file.is_open()) {
            return;
        }

        ScheduleEvent currentEvent;
        std::string line;
        while (std::getline(file, line)) {
            if (line.find("Event ID: ") == 0) {
                currentEvent.id = line.substr(10);
            } else if (line.find("Title: ") == 0) {
                currentEvent.title = line.substr(7);
            } else if (line.find("Type: ") == 0) {
                currentEvent.type = line.substr(6);
            } else if (line.find("Date: ") == 0) {
                currentEvent.date = std::stoll(line.substr(6));
            } else if (line.find("Description: ") == 0) {
                currentEvent.description = line.substr(13);
            } else if (line.find("Course: ") == 0) {
                currentEvent.course = line.substr(8);
            } else if (line.find("Reminder Days: ") == 0) {
                currentEvent.reminderDays = std::stoi(line.substr(15));
                events.push_back(currentEvent);  // Add event when we've read all fields
            }
        }
        
        std::cout << "Debug: Loaded " << events.size() << " events" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error loading events: " << e.what() << std::endl;
    }
}

std::string ScheduleHandler::generateId() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 999999);
    
    return "event_" + std::to_string(dis(gen));
}

void ScheduleHandler::sendReminders() {
    // TODO: Implement actual reminder functionality
    std::cout << "Debug: Sending reminders (not implemented)" << std::endl;
}

