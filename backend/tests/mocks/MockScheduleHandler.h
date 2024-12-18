#pragma once
#include <gmock/gmock.h>
#include "../../include/ScheduleHandler.h"

class MockScheduleHandler : public ScheduleHandler {
public:
    MOCK_METHOD(std::string, getSchedule, (), (override));
    MOCK_METHOD(bool, addEvent, (const json& eventData), (override));
    MOCK_METHOD(void, checkReminders, (), (override));
}; 