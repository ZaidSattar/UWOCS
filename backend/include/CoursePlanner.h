#ifndef COURSE_PLANNER_H
#define COURSE_PLANNER_H

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Forward declarations
class Term;
class Year;
class CourseStatus;

enum class TermType {
    FALL,
    WINTER,
    SUMMER
};

enum class CourseState {
    IN_PROGRESS,
    COMPLETED,
    INCOMPLETE
};

// Add this declaration before the classes
std::string getTermString(TermType type);

class PlannerCourse {
public:
    explicit PlannerCourse(const std::string& code);
    std::string getCode() const;
    CourseState getState() const;
    void setState(CourseState state);

private:
    std::string code;
    CourseState state;
};

class Term {
public:
    Term() : type(TermType::FALL) {}
    explicit Term(TermType type);
    void addCourse(const PlannerCourse& course);
    void removeCourse(const std::string& courseCode);
    std::vector<PlannerCourse> getCourses() const;
    TermType getType() const;
    void updateCourseStates(const std::chrono::system_clock::time_point& currentDate) const;

private:
    TermType type;
    mutable std::vector<PlannerCourse> courses;
};

class Year {
public:
    Year() : yearNumber(1) {
        terms[TermType::FALL] = Term(TermType::FALL);
        terms[TermType::WINTER] = Term(TermType::WINTER);
        terms[TermType::SUMMER] = Term(TermType::SUMMER);
    }
    explicit Year(int yearNumber);
    void addCourse(TermType term, const PlannerCourse& course);
    void removeCourse(TermType term, const std::string& courseCode);
    const std::map<TermType, Term>& getTerms() const { return terms; }
    int getYearNumber() const;

private:
    int yearNumber;
    std::map<TermType, Term> terms;
};

class CoursePlanner {
public:
    CoursePlanner();
    std::string loadPlannerData();
    std::string updatePlannerData(const json& planData, int currentYear);
    std::string updateCurrentYear(int year);
    std::string calculateCourseStatuses(const json& planData, int currentYear);

private:
    static const std::string PLANNER_PATH;
    std::map<int, Year> years;
    int currentYear;
    
    void updateCourseStatuses();
    bool isWithinTermDates(TermType term, const std::chrono::system_clock::time_point& date) const;
    CourseState determineCourseState(TermType term, int year) const;
    void saveToFile() const;
    void loadFromFile();
    void initializeEmptyFile();
};

#endif // COURSE_PLANNER_H 