#include "../include/GPACalculator.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <cstring>
#include <cerrno>

const std::string GPA_PATH = "documents/gpa.txt";

std::vector<Course> GPACalculator::parseCoursesFromJson(const std::string& jsonStr) {
    std::vector<Course> courses;
    std::istringstream json(jsonStr);
    std::string line;
    Course currentCourse;
    
    // Debug the entire JSON string
    std::cout << "Full JSON string: " << jsonStr << std::endl;
    
    // Find the position of "courses":[
    size_t coursesStart = jsonStr.find("\"courses\":[");
    if (coursesStart == std::string::npos) {
        std::cout << "No courses array found" << std::endl;
        return courses;
    }
    
    // Find the array content
    size_t arrayStart = jsonStr.find('[', coursesStart);
    size_t arrayEnd = jsonStr.find(']', arrayStart);
    if (arrayStart == std::string::npos || arrayEnd == std::string::npos) {
        std::cout << "Invalid array format" << std::endl;
        return courses;
    }
    
    // Get the array content
    std::string arrayContent = jsonStr.substr(arrayStart + 1, arrayEnd - arrayStart - 1);
    std::cout << "Array content: " << arrayContent << std::endl;
    
    // Split by },{
    size_t pos = 0;
    size_t end;
    while ((end = arrayContent.find("},{", pos)) != std::string::npos) {
        std::string courseJson = arrayContent.substr(pos, end - pos + 1);
        
        // Parse individual course
        Course course;
        
        // Extract name
        size_t nameStart = courseJson.find("\"name\":\"") + 8;
        size_t nameEnd = courseJson.find("\"", nameStart);
        course.name = courseJson.substr(nameStart, nameEnd - nameStart);
        
        // Extract grade
        size_t gradeStart = courseJson.find("\"grade\":") + 8;
        size_t gradeEnd = courseJson.find(",", gradeStart);
        course.grade = std::stod(courseJson.substr(gradeStart, gradeEnd - gradeStart));
        
        // Extract weight
        size_t weightStart = courseJson.find("\"weight\":") + 9;
        size_t weightEnd = courseJson.find("}", weightStart);
        double weight = std::stod(courseJson.substr(weightStart, weightEnd - weightStart));
        course.weight = (weight == 1.0) ? 1.0 : 0.5;
        
        std::cout << "Parsed course - Name: " << course.name 
                 << ", Grade: " << course.grade 
                 << ", Weight: " << course.weight << std::endl;
        
        courses.push_back(course);
        pos = end + 2;
    }
    
    // Handle the last course
    std::string courseJson = arrayContent.substr(pos);
    Course course;
    
    // Extract name
    size_t nameStart = courseJson.find("\"name\":\"") + 8;
    size_t nameEnd = courseJson.find("\"", nameStart);
    course.name = courseJson.substr(nameStart, nameEnd - nameStart);
    
    // Extract grade
    size_t gradeStart = courseJson.find("\"grade\":") + 8;
    size_t gradeEnd = courseJson.find(",", gradeStart);
    course.grade = std::stod(courseJson.substr(gradeStart, gradeEnd - gradeStart));
    
    // Extract weight
    size_t weightStart = courseJson.find("\"weight\":") + 9;
    size_t weightEnd = courseJson.find("}", weightStart);
    double weight = std::stod(courseJson.substr(weightStart, weightEnd - weightStart));
    course.weight = (weight == 1.0) ? 1.0 : 0.5;
    
    std::cout << "Parsed last course - Name: " << course.name 
             << ", Grade: " << course.grade 
             << ", Weight: " << course.weight << std::endl;
    
    courses.push_back(course);
    
    return courses;
}

double GPACalculator::calculateWeightedGPA(const std::vector<Course>& courses) {
    double totalWeightedGrade = 0.0;
    double totalWeight = 0.0;

    std::cout << "\n=== GPA Calculation Details ===\n";
    
    for (const auto& course : courses) {
        // Calculate weighted grade (grade * weight)
        double weightedGrade = course.grade * course.weight;
        
        std::cout << "\nProcessing course: " << course.name << std::endl;
        std::cout << "Raw grade: " << course.grade << std::endl;
        std::cout << "Credit weight: " << course.weight << std::endl;
        std::cout << "Weighted contribution: " << weightedGrade << std::endl;
        
        totalWeightedGrade += weightedGrade;
        totalWeight += course.weight;
    }

    std::cout << "\nFinal Calculation:" << std::endl;
    std::cout << "Total weighted points: " << totalWeightedGrade << std::endl;
    std::cout << "Total credits: " << totalWeight << std::endl;

    if (totalWeight == 0.0) {
        std::cout << "Warning: Total weight is 0, returning 0" << std::endl;
        return 0.0;
    }

    double gpa = totalWeightedGrade / totalWeight;
    std::cout << "Final GPA = " << totalWeightedGrade << " / " << totalWeight << " = " << gpa << "%\n";
    std::cout << "============================\n\n";
    
    return gpa;
}

std::string GPACalculator::calculateGPA(const std::string& coursesData) {
    try {
        std::vector<Course> courses = parseCoursesFromJson(coursesData);
        double gpa = calculateWeightedGPA(courses);
        
        // Save to file
        saveGPA(courses, gpa);

        std::stringstream response;
        response << "{\"type\":\"gpaCalculated\",\"gpa\":" << std::fixed << std::setprecision(2) << gpa << "}";
        
        return response.str();
    } catch (const std::exception& e) {
        std::cerr << "Error calculating GPA: " << e.what() << std::endl;
        return "{\"type\":\"error\",\"message\":\"Failed to calculate GPA\"}";
    }
}

void GPACalculator::saveGPA(const std::vector<Course>& courses, double gpa) {
    std::cout << "Debug: Starting to save GPA records" << std::endl;
    
    std::stringstream ss;
    ss << "GPA RECORDS\n";
    ss << "---------------\n\n";
    ss << "Current GPA: " << std::fixed << std::setprecision(2) << gpa << "\n\n";
    ss << "COURSES:\n";
    ss << "---------------\n\n";
    
    for (const auto& course : courses) {
        ss << "Course Name: " << course.name << "\n";
        ss << "Grade: " << course.grade << "\n";
        ss << "Weight: " << course.weight << "\n";
        ss << "---------------\n\n";
    }
    
    try {
        // Create directories if they don't exist
        std::filesystem::create_directories(std::filesystem::path(GPA_PATH).parent_path());
        
        // Open file with proper error handling
        std::ofstream file(GPA_PATH);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << std::strerror(errno) << std::endl;
            return;
        }
        
        // Write content
        std::string content = ss.str();
        file << content;
        
        // Ensure content is written
        file.flush();
        if (file.fail()) {
            std::cerr << "Failed to write to file: " << std::strerror(errno) << std::endl;
            file.close();
            return;
        }
        
        file.close();
        std::cout << "Debug: Successfully saved GPA records to file" << std::endl;
        std::cout << "Debug: Wrote content:\n" << content << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error saving GPA records: " << e.what() << std::endl;
    }
} 