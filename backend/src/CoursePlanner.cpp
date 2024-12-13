#include "../include/CoursePlanner.h"
#include <fstream>
#include <filesystem>
#include <iostream>
#include <ctime>

const std::string CoursePlanner::PLANNER_PATH = "../storage/planner/courses.txt";

// Move this function to the top, after the constants
std::string getTermString(TermType type) {
    switch(type) {
        case TermType::FALL: return "Fall Term";
        case TermType::WINTER: return "Winter Term";
        case TermType::SUMMER: return "Summer Term";
        default: return "Unknown Term";
    }
}

// Course Implementation
PlannerCourse::PlannerCourse(const std::string& code) 
    : code(code), state(CourseState::INCOMPLETE) {}

std::string PlannerCourse::getCode() const { return code; }
CourseState PlannerCourse::getState() const { return state; }
void PlannerCourse::setState(CourseState newState) { state = newState; }

// Term Implementation
Term::Term(TermType type) : type(type) {}

void Term::addCourse(const PlannerCourse& course) {
    courses.push_back(course);
}

void Term::removeCourse(const std::string& courseCode) {
    courses.erase(
        std::remove_if(courses.begin(), courses.end(),
            [&courseCode](const PlannerCourse& c) { return c.getCode() == courseCode; }),
        courses.end()
    );
}

std::vector<PlannerCourse> Term::getCourses() const { return courses; }
TermType Term::getType() const { return type; }

void Term::updateCourseStates(const std::chrono::system_clock::time_point& currentDate) const {
    auto now = std::chrono::system_clock::to_time_t(currentDate);
    std::tm* ltm = std::localtime(&now);
    
    // Define term dates
    bool isFall = (ltm->tm_mon >= 8 && ltm->tm_mon <= 11);  // Sept-Dec
    bool isWinter = (ltm->tm_mon >= 0 && ltm->tm_mon <= 3); // Jan-Apr
    bool isSummer = (ltm->tm_mon >= 4 && ltm->tm_mon <= 7); // May-Aug

    CourseState newState;
    switch(type) {
        case TermType::FALL:
            if (isFall) newState = CourseState::IN_PROGRESS;
            else if (ltm->tm_mon > 11) newState = CourseState::COMPLETED;
            else newState = CourseState::INCOMPLETE;
            break;
            
        case TermType::WINTER:
            if (isWinter) newState = CourseState::IN_PROGRESS;
            else if (ltm->tm_mon > 3) newState = CourseState::COMPLETED;
            else newState = CourseState::INCOMPLETE;
            break;
            
        case TermType::SUMMER:
            if (isSummer) newState = CourseState::IN_PROGRESS;
            else if (ltm->tm_mon > 7) newState = CourseState::COMPLETED;
            else newState = CourseState::INCOMPLETE;
            break;
    }

    for (auto& course : courses) {
        course.setState(newState);
    }
}

// Year Implementation
Year::Year(int yearNumber) : yearNumber(yearNumber) {
    terms[TermType::FALL] = Term(TermType::FALL);
    terms[TermType::WINTER] = Term(TermType::WINTER);
    terms[TermType::SUMMER] = Term(TermType::SUMMER);
}

void Year::addCourse(TermType term, const PlannerCourse& course) {
    terms[term].addCourse(course);
}

void Year::removeCourse(TermType term, const std::string& courseCode) {
    terms[term].removeCourse(courseCode);
}

int Year::getYearNumber() const { return yearNumber; }

// CoursePlanner Implementation
CoursePlanner::CoursePlanner() : currentYear(1) {
    try {
        // Create all necessary directories
        std::filesystem::create_directories("../storage/planner");
        
        if (!std::filesystem::exists(PLANNER_PATH)) {
            std::cout << "Creating new planner file at: " << PLANNER_PATH << std::endl;
            std::ofstream file(PLANNER_PATH);
            if (!file.is_open()) {
                throw std::runtime_error("Failed to create planner file");
            }
            // Initialize with empty data structure
            json initialData = {
                {"currentYear", 1},
                {"years", json::object()}
            };
            file << initialData.dump(4);
            file.close();
        }
        
        loadFromFile();  // Load saved data when constructed
    } catch (const std::exception& e) {
        std::cerr << "Error in CoursePlanner constructor: " << e.what() << std::endl;
    }
}

std::string CoursePlanner::loadPlannerData() {
    std::cout << "[DEBUG] Starting loadPlannerData()" << std::endl;
    
    // First load the data from file
    loadFromFile();

    // Then prepare the response
    json response;
    response["type"] = "plannerData";
    
    try {
        std::cout << "[DEBUG] Converting internal data to JSON response" << std::endl;
        // Convert our internal data structure to JSON response
        json planData;
        json statusData;
        
        std::cout << "[DEBUG] Number of years in internal data: " << years.size() << std::endl;
        
        for (const auto& [yearNum, year] : years) {
            std::cout << "[DEBUG] Processing year " << yearNum << std::endl;
            for (const auto& [termType, term] : year.getTerms()) {
                std::string termStr = getTermString(termType);
                std::cout << "[DEBUG] Processing term " << termStr << std::endl;
                
                auto courses = term.getCourses();
                std::cout << "[DEBUG] Found " << courses.size() << " courses in term" << std::endl;
                
                for (const auto& course : courses) {
                    std::cout << "[DEBUG] Adding course " << course.getCode() << std::endl;
                    planData[std::to_string(yearNum)][termStr].push_back(course.getCode());
                }
            }
        }
        
        response["planData"] = planData;
        response["courseStatus"] = statusData;
        response["currentYear"] = currentYear;
        
        std::cout << "[DEBUG] Final response: " << response.dump(4) << std::endl;

        // Calculate initial statuses if we have data
        if (!years.empty()) {
            std::cout << "[DEBUG] Calculating initial course statuses" << std::endl;
            updateCourseStatuses();
        } else {
            std::cout << "[DEBUG] No data to calculate statuses for" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Error preparing planner data response: " << e.what() << std::endl;
        return "{\"type\":\"error\",\"message\":\"Failed to load planner data\"}";
    }
    
    return response.dump();
}

std::string CoursePlanner::updatePlannerData(const json& planData, int newCurrentYear) {
    try {
        std::cout << "Updating planner data with new year: " << newCurrentYear << std::endl;
        years.clear();
        currentYear = newCurrentYear;
        
        // Convert JSON data to our internal structure
        for (const auto& [year, yearData] : planData.items()) {
            int yearNum = std::stoi(year);
            years[yearNum] = Year(yearNum);
            
            for (const auto& [termStr, courses] : yearData.items()) {
                TermType termType;
                if (termStr == "Fall Term") termType = TermType::FALL;
                else if (termStr == "Winter Term") termType = TermType::WINTER;
                else termType = TermType::SUMMER;
                
                for (const auto& courseCode : courses) {
                    years[yearNum].addCourse(termType, PlannerCourse(courseCode));
                }
            }
        }
        
        std::cout << "Saving updated planner data..." << std::endl;
        saveToFile();  // Save after updating
        return loadPlannerData(); // Return updated data with statuses
        
    } catch (const std::exception& e) {
        std::cerr << "Error updating planner data: " << e.what() << std::endl;
        return "{\"type\":\"error\",\"message\":\"Failed to update planner data\"}";
    }
}

void CoursePlanner::saveToFile() const {
    try {
        std::cout << "Attempting to save to file: " << PLANNER_PATH << std::endl;
        
        json data;
        data["currentYear"] = currentYear;
        
        // Save all course data
        for (const auto& [yearNum, year] : years) {
            for (const auto& [termType, term] : year.getTerms()) {
                std::string termStr = getTermString(termType);
                for (const auto& course : term.getCourses()) {
                    data["years"][std::to_string(yearNum)][termStr].push_back(course.getCode());
                }
            }
        }

        // Create directories if they don't exist
        std::filesystem::create_directories(std::filesystem::path(PLANNER_PATH).parent_path());
        
        // Write to file with pretty printing
        std::ofstream file(PLANNER_PATH);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open planner file for writing");
        }
        
        std::string jsonStr = data.dump(4);
        file << jsonStr;
        file.close();
        
        std::cout << "Successfully saved planner data: " << jsonStr << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error saving planner data: " << e.what() << std::endl;
    }
}

void CoursePlanner::updateCourseStatuses() {
    auto now = std::chrono::system_clock::now();
    for (auto& [yearNum, year] : years) {
        for (auto& [termType, term] : year.getTerms()) {
            term.updateCourseStates(now);
        }
    }
}

bool CoursePlanner::isWithinTermDates(TermType term, const std::chrono::system_clock::time_point& date) const {
    auto timeT = std::chrono::system_clock::to_time_t(date);
    std::tm* ltm = std::localtime(&timeT);
    
    switch(term) {
        case TermType::FALL:
            return (ltm->tm_mon >= 8 && ltm->tm_mon <= 11); // Sept-Dec
        case TermType::WINTER:
            return (ltm->tm_mon >= 0 && ltm->tm_mon <= 3);  // Jan-Apr
        case TermType::SUMMER:
            return (ltm->tm_mon >= 4 && ltm->tm_mon <= 7);  // May-Aug
        default:
            return false;
    }
}

CourseState CoursePlanner::determineCourseState(TermType term, int year) const {
    auto now = std::chrono::system_clock::now();
    auto timeT = std::chrono::system_clock::to_time_t(now);
    std::tm* ltm = std::localtime(&timeT);
    
    if (year < currentYear) return CourseState::COMPLETED;
    if (year > currentYear) return CourseState::INCOMPLETE;
    
    // Current year logic
    if (isWithinTermDates(term, now)) return CourseState::IN_PROGRESS;
    
    switch(term) {
        case TermType::FALL:
            return (ltm->tm_mon > 11) ? CourseState::COMPLETED : CourseState::INCOMPLETE;
        case TermType::WINTER:
            return (ltm->tm_mon > 3) ? CourseState::COMPLETED : CourseState::INCOMPLETE;
        case TermType::SUMMER:
            return (ltm->tm_mon > 7) ? CourseState::COMPLETED : CourseState::INCOMPLETE;
        default:
            return CourseState::INCOMPLETE;
    }
} 

std::string CoursePlanner::calculateCourseStatuses(const json& planData, int currentYear) {
    // First update and save the internal data
    try {
        std::cout << "Updating internal data before calculating statuses..." << std::endl;
        years.clear();
        this->currentYear = currentYear;
        
        // Convert JSON data to our internal structure
        for (const auto& [year, yearData] : planData.items()) {
            int yearNum = std::stoi(year);
            years[yearNum] = Year(yearNum);
            
            for (const auto& [termStr, courses] : yearData.items()) {
                TermType termType;
                if (termStr == "Fall Term") termType = TermType::FALL;
                else if (termStr == "Winter Term") termType = TermType::WINTER;
                else termType = TermType::SUMMER;
                
                for (const auto& courseCode : courses) {
                    years[yearNum].addCourse(termType, PlannerCourse(courseCode));
                }
            }
        }
        
        // Save the updated data
        std::cout << "Saving updated data to file..." << std::endl;
        saveToFile();
    } catch (const std::exception& e) {
        std::cerr << "Error updating internal data: " << e.what() << std::endl;
    }

    // Then calculate and return statuses
    json response;
    response["type"] = "courseStatuses";
    json statuses;
    
    auto now = std::chrono::system_clock::now();
    auto timeT = std::chrono::system_clock::to_time_t(now);
    std::tm* ltm = std::localtime(&timeT);
    
    // Current month (0-11)
    int currentMonth = ltm->tm_mon;
    
    // Academic year starts in September (month 8)
    // For a given academic year:
    // Fall: Sept-Dec (8-11) - First term
    // Winter: Jan-Apr (0-3) - Second term
    // Summer: May-Aug (4-7) - Third term
    
    for (const auto& [year, yearData] : planData.items()) {
        int yearNum = std::stoi(year);
        
        for (const auto& [term, courses] : yearData.items()) {
            std::string status;
            
            if (yearNum < currentYear) {
                status = "completed";
            } else if (yearNum > currentYear) {
                status = "incomplete";
            } else {  // Current year
                if (currentMonth >= 8) {  // We're in Fall term (Sept-Dec)
                    if (term == "Fall Term") {
                        status = "in_progress";
                    } else {  // Winter and Summer terms haven't started yet
                        status = "incomplete";
                    }
                } else if (currentMonth >= 0 && currentMonth <= 3) {  // We're in Winter term (Jan-Apr)
                    if (term == "Fall Term") {
                        status = "completed";
                    } else if (term == "Winter Term") {
                        status = "in_progress";
                    } else {  // Summer term hasn't started yet
                        status = "incomplete";
                    }
                } else {  // We're in Summer term (May-Aug)
                    if (term == "Fall Term" || term == "Winter Term") {
                        status = "completed";
                    } else if (term == "Summer Term") {
                        status = "in_progress";
                    }
                }
            }
            
            // Add each course with its status
            for (const auto& course : courses) {
                json courseStatus;
                courseStatus["code"] = course;
                courseStatus["status"] = status;
                statuses[year][term].push_back(courseStatus);
            }
        }
    }
    
    response["statuses"] = statuses;
    return response.dump();
} 

void CoursePlanner::loadFromFile() {
    try {
        std::cout << "[DEBUG] Attempting to load from file: " << PLANNER_PATH << std::endl;
        
        if (!std::filesystem::exists(PLANNER_PATH)) {
            std::cout << "[DEBUG] File doesn't exist, creating new one" << std::endl;
            initializeEmptyFile();
            return;
        }

        std::ifstream file(PLANNER_PATH);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open planner file for reading");
        }

        std::string fileContents;
        file.seekg(0, std::ios::end);
        fileContents.reserve(file.tellg());
        file.seekg(0, std::ios::beg);
        fileContents.assign((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        
        std::cout << "[DEBUG] File contents: " << fileContents << std::endl;

        if (fileContents.empty()) {
            std::cout << "[DEBUG] File is empty, initializing" << std::endl;
            file.close();
            initializeEmptyFile();
            return;
        }

        json data = json::parse(fileContents);
        std::cout << "[DEBUG] Parsed JSON data: " << data.dump(4) << std::endl;

        // Load current year
        if (data.contains("currentYear")) {
            currentYear = data["currentYear"];
            std::cout << "Loaded current year: " << currentYear << std::endl;
        }

        // Load course data
        if (data.contains("years")) {
            for (const auto& [year, yearData] : data["years"].items()) {
                int yearNum = std::stoi(year);
                years[yearNum] = Year(yearNum);
                std::cout << "Loading data for year " << yearNum << std::endl;

                for (const auto& [termStr, courses] : yearData.items()) {
                    TermType termType;
                    if (termStr == "Fall Term") termType = TermType::FALL;
                    else if (termStr == "Winter Term") termType = TermType::WINTER;
                    else termType = TermType::SUMMER;

                    std::cout << "Loading courses for term: " << termStr << std::endl;
                    for (const auto& courseCode : courses) {
                        std::cout << "Adding course: " << courseCode << std::endl;
                        years[yearNum].addCourse(termType, PlannerCourse(courseCode));
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Error in loadFromFile: " << e.what() << std::endl;
    }
}

void CoursePlanner::initializeEmptyFile() {
    try {
        std::filesystem::create_directories(std::filesystem::path(PLANNER_PATH).parent_path());
        
        // Initialize with empty data structure
        json initialData = {
            {"currentYear", currentYear},
            {"years", json::object()}
        };

        std::ofstream file(PLANNER_PATH);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to create planner file");
        }
        file << initialData.dump(4);
        file.close();
        
        std::cout << "Initialized empty planner file with structure: " << initialData.dump(4) << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error initializing planner file: " << e.what() << std::endl;
    }
}