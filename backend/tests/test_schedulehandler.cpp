#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../include/ScheduleHandler.h"
#include "mocks/MockScheduleHandler.h"

using ::testing::Return;
using ::testing::_;
using ::testing::AtLeast;

class ScheduleHandlerTest : public ::testing::Test {
protected:
    MockScheduleHandler mockHandler;
};

TEST_F(ScheduleHandlerTest, AddEventTest) {
    json eventData = {
        {"title", "Test Event"},
        {"type", "assignment"},
        {"date", std::time(nullptr)},
        {"description", "Test Description"},
        {"course", "CS3307"},
        {"reminderDays", 1}
    };
    
    EXPECT_CALL(mockHandler, addEvent(eventData))
        .Times(1)
        .WillOnce(Return(true));
    
    EXPECT_TRUE(mockHandler.addEvent(eventData));
}

TEST_F(ScheduleHandlerTest, GetScheduleTest) {
    json expectedSchedule = {
        {"events", json::array({
            {
                {"id", "event_1"},
                {"title", "Test Event"},
                {"type", "assignment"}
            }
        })}
    };
    
    EXPECT_CALL(mockHandler, getSchedule())
        .Times(1)
        .WillOnce(Return(expectedSchedule.dump()));
    
    std::string result = mockHandler.getSchedule();
    json schedule = json::parse(result);
    
    EXPECT_TRUE(schedule.contains("events"));
    EXPECT_EQ(schedule["events"].size(), 1);
}

TEST_F(ScheduleHandlerTest, CheckRemindersTest) {
    EXPECT_CALL(mockHandler, checkReminders())
        .Times(AtLeast(1));
    
    mockHandler.checkReminders();
} 