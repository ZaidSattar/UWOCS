#include <gtest/gtest.h>
#include "../include/ScheduleHandler.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class ScheduleHandlerTest : public ::testing::Test {
protected:
    ScheduleHandler handler;

    void SetUp() override {
        // Setup code if needed
    }

    void TearDown() override {
        // Cleanup code if needed
    }
};

TEST_F(ScheduleHandlerTest, AddEventTest) {
    json eventData = {
        {"title", "Test Assignment"},
        {"type", "assignment"},
        {"date", std::time(nullptr)},
        {"description", "Test Description"},
        {"course", "CS3307"},
        {"reminderDays", 1}
    };

    EXPECT_TRUE(handler.addEvent(eventData));
}

TEST_F(ScheduleHandlerTest, GetScheduleTest) {
    std::string schedule = handler.getSchedule();
    EXPECT_FALSE(schedule.empty());
    
    // Verify JSON structure
    json scheduleJson = json::parse(schedule);
    EXPECT_TRUE(scheduleJson.is_array());
}

TEST_F(ScheduleHandlerTest, CheckRemindersTest) {
    // Add event with reminder
    json eventData = {
        {"title", "Test Event"},
        {"type", "test"},
        {"date", std::time(nullptr) + 86400}, // Tomorrow
        {"description", "Test Description"},
        {"course", "CS3307"},
        {"reminderDays", 1}
    };
    
    handler.addEvent(eventData);
    handler.checkReminders();
    // Verify reminder functionality (would need mock for actual notification testing)
} 