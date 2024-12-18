#ifndef GPA_CALCULATOR_H
#define GPA_CALCULATOR_H

#include <string>
#include <vector>

struct Course {
    std::string name;
    double grade;
    double weight;
};

class GPACalculator {
public:
    GPACalculator() = default;
    std::string calculateGPA(const std::string& coursesData);
    double calculateGradePoints(double grade);
    std::string createErrorResponse(const std::string& message);

private:
    double calculateWeightedGPA(const std::vector<Course>& courses);
    std::vector<Course> parseCoursesFromJson(const std::string& jsonStr);
    void saveGPA(const std::vector<Course>& courses, double gpa);
};

#endif // GPA_CALCULATOR_H 