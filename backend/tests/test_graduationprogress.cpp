#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include "../include/GraduationProgress.h"

using json = nlohmann::json;

class GraduationProgressTest : public ::testing::Test {
protected:
    GraduationProgress progress;
};

TEST_F(GraduationProgressTest, GetModuleRequirementsTest) {
    std::string moduleId = "honors_cs";
    std::string response = progress.getModuleRequirements(moduleId);
    EXPECT_FALSE(response.empty());
    
    json data = json::parse(response);
    EXPECT_TRUE(data.contains("requirements"));
}

TEST_F(GraduationProgressTest, CalculateProgressTest) {
    std::vector<std::string> completedCourses = {
        "CS3307", "CS3305", "CS3342"
    };
    
    std::string moduleId = "honors_cs";
    std::string response = progress.calculateProgress(moduleId, completedCourses);
    EXPECT_FALSE(response.empty());
    
    json data = json::parse(response);
    EXPECT_TRUE(data.contains("progress"));
}

TEST_F(GraduationProgressTest, LoadProgressTest) {
    std::string moduleId;
    auto completedCourses = progress.loadProgress(moduleId);
    
    EXPECT_FALSE(moduleId.empty());
    EXPECT_FALSE(completedCourses.empty());
}

