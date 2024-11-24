#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <ctime>

using json = nlohmann::json;

struct ScheduleEvent {
    std::string id;
    std::string title;
    std::string type;
    std::time_t date;
    std::string description;
    std::string course;
    int reminderDays;
};

class ScheduleHandler {
public:
    ScheduleHandler();
    
    std::string getSchedule();
    bool addEvent(const json& eventData);
    void checkReminders();
    
private:
    const std::string SCHEDULE_PATH = "../storage/schedule/schedule.txt";
    std::vector<ScheduleEvent> events;
    
    void loadEvents();
    void saveEvents();
    void sendReminders();
    std::string generateId();
};