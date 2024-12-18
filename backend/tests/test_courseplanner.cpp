#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include "../include/CoursePlanner.h"

using json = nlohmann::json;

class CoursePlannerTest : public ::testing::Test {
protected:
    CoursePlanner planner;
};

TEST_F(CoursePlannerTest, LoadPlannerDataTest) {
    std::string data = planner.loadPlannerData();
    EXPECT_FALSE(data.empty());
    
    json planData = json::parse(data);
    EXPECT_TRUE(planData.contains("planData"));
    EXPECT_TRUE(planData.contains("currentYear"));
    EXPECT_TRUE(planData.contains("type"));
    EXPECT_EQ(planData["type"], "plannerData");
}

TEST_F(CoursePlannerTest, CalculateCourseStatusesTest) {
    json planData = {
        "1", {
            "Fall Term", json::array({"CS3307"}),
            "Winter Term", json::array({"CS3305"})
        }
    };
    
    try {
        std::string response = planner.calculateCourseStatuses(planData, 1);
        EXPECT_FALSE(response.empty());
        
        json statusData = json::parse(response);
        EXPECT_TRUE(statusData.contains("statuses"));
    } catch (const std::exception& e) {
        FAIL() << "Exception thrown: " << e.what();
    }
}

TEST_F(CoursePlannerTest, EmptyPlanTest) {
    json emptyPlan = {
        {"years", json::object()}
    };
    
    std::string response = planner.calculateCourseStatuses(emptyPlan, 1);
    json statusData = json::parse(response);
    
    EXPECT_TRUE(statusData.contains("statuses"));
    EXPECT_TRUE(statusData["statuses"].empty());
}

TEST_F(CoursePlannerTest, InvalidYearTest) {
    json planData = {
        {"years", {
            {"1", {
                {"Fall Term", {"CS3307"}}
            }}
        }}
    };
    
    // Test with invalid year
    std::string response = planner.calculateCourseStatuses(planData, 5);
    json statusData = json::parse(response);
    
    EXPECT_TRUE(statusData.contains("error") || 
                (statusData.contains("statuses") && statusData["statuses"].empty()));
} 