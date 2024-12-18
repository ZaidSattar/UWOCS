#include <gtest/gtest.h>
#include "../include/GPACalculator.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class GPACalculatorTest : public ::testing::Test {
protected:
    GPACalculator calculator;
};

TEST_F(GPACalculatorTest, CalculatesGPACorrectly) {
    // Create test input
    json input = {
        {"courses", {
            {
                {"name", "CS3307"},
                {"grade", 85.0},
                {"weight", 1.0}
            }
        }}
    };

    std::string result = calculator.calculateGPA(input.dump());
    EXPECT_FALSE(result.empty());

    json output = json::parse(result);
    EXPECT_TRUE(output.contains("gpa"));
} 