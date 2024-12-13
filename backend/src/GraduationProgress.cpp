#include "../include/GraduationProgress.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cmath>
#include <sstream>

const std::string PROGRESS_PATH = "storage/graduation/progress.txt";

// CourseGroup Implementation
CourseGroup::CourseGroup(const std::string& title, double credits) 
    : title(title), requiredCredits(credits) {}

void CourseGroup::addCourse(const std::string& course) {
    courses.push_back(course);
}

bool CourseGroup::isCompleted(const std::vector<std::string>& completedCourses) const {
    int completed = 0;
    for (const auto& course : courses) {
        if (std::find(completedCourses.begin(), completedCourses.end(), course) != completedCourses.end()) {
            completed++;
        }
    }
    return completed >= (requiredCredits * 2); // Each credit = 2 courses
}

json CourseGroup::toJson() const {
    json groupJson;
    groupJson["title"] = title;
    groupJson["courses"] = courses;
    groupJson["requiredCredits"] = requiredCredits;
    return groupJson;
}

// SelectionCourseGroup Implementation
SelectionCourseGroup::SelectionCourseGroup(const std::string& title, double credits, int maxSelect)
    : CourseGroup(title, credits), maxSelectable(maxSelect) {}

bool SelectionCourseGroup::isCompleted(const std::vector<std::string>& completedCourses) const {
    int completed = 0;
    for (const auto& course : courses) {
        if (std::find(completedCourses.begin(), completedCourses.end(), course) != completedCourses.end()) {
            completed++;
        }
    }
    return completed == maxSelectable;
}

// Module Implementation
Module::Module(const std::string& id, const std::string& title, double minGpa, double minGrade)
    : id(id), title(title), minGpaRequired(minGpa), minCourseGrade(minGrade) {}

void Module::addCourseGroup(std::shared_ptr<CourseGroup> group) {
    courseGroups.push_back(group);
}

json Module::getRequirements() const {
    json requirements;
    requirements["title"] = title;
    requirements["minGpa"] = minGpaRequired;
    requirements["minGrade"] = minCourseGrade;

    json courseGroupsJson = json::array();
    for (const auto& group : courseGroups) {
        courseGroupsJson.push_back(group->toJson());
    }
    requirements["courseGroups"] = courseGroupsJson;

    return requirements;
}

bool Module::meetsRequirements(const std::vector<std::string>& completedCourses, double gpa) const {
    if (gpa < minGpaRequired) return false;
    
    for (const auto& group : courseGroups) {
        if (!group->isCompleted(completedCourses)) {
            return false;
        }
    }
    return true;
}

// HonorsModule Implementation
HonorsModule::HonorsModule(const std::string& id, const std::string& title)
    : Module(id, title, 70.0, 60.0) {}

bool HonorsModule::meetsRequirements(const std::vector<std::string>& completedCourses, double gpa) const {
    return Module::meetsRequirements(completedCourses, gpa);
}

// SpecializationModule Implementation
SpecializationModule::SpecializationModule(const std::string& id, const std::string& title)
    : Module(id, title, 60.0, 50.0) {}  // 60% average, 50% minimum grade

// MinorModule Implementation
MinorModule::MinorModule(const std::string& id, const std::string& title)
    : Module(id, title, 60.0, 50.0) {}  // 60% average, 50% minimum grade

// ModuleFactory Implementation
std::shared_ptr<Module> ModuleFactory::createModule(const std::string& moduleId) {
    if (moduleId == "honors_cs") return createHonorsCS();
    if (moduleId == "honors_is") return createHonorsIS();
    if (moduleId == "honors_bio") return createHonorsBio();
    if (moduleId == "spec_cs") return createSpecCS();
    if (moduleId == "major_cs") return createMajorCS();
    if (moduleId == "minor_game") return createMinorGame();
    if (moduleId == "minor_se") return createMinorSE();
    return nullptr;
}

std::shared_ptr<Module> ModuleFactory::createHonorsCS() {
    auto module = std::make_shared<HonorsModule>("honors_cs", "Honors Specialization in Computer Science (9.0 courses)");
    
    // 5.5 courses - required core
    auto coreGroup = std::make_shared<CourseGroup>("Required Core Courses (5.5 courses - all required)", 5.5);
    std::vector<std::string> coreCourses = {
        "Computer Science 2208A/B", "Computer Science 2209A/B",
        "Computer Science 2210A/B", "Computer Science 2211A/B",
        "Computer Science 2212A/B/Y", "Computer Science 3305A/B",
        "Computer Science 3307A/B/Y", "Computer Science 3331A/B",
        "Computer Science 3340A/B", "Computer Science 3342A/B",
        "Computer Science 3350A/B"
    };
    for (const auto& course : coreCourses) {
        coreGroup->addCourse(course);
    }
    module->addCourseGroup(coreGroup);

    // 0.5 course from math options
    auto mathGroup = std::make_shared<SelectionCourseGroup>(
        "Mathematics Requirement (0.5 course - select one)", 0.5, 1);
    mathGroup->addCourse("Computer Science 2214A/B");
    mathGroup->addCourse("Mathematics 2155F/G");
    module->addCourseGroup(mathGroup);

    // 0.5 course from writing options
    auto writingGroup = std::make_shared<SelectionCourseGroup>(
        "Writing Requirement (0.5 course - select one)", 0.5, 1);
    writingGroup->addCourse("Writing 2101F/G");
    writingGroup->addCourse("Writing 2111F/G");
    writingGroup->addCourse("Writing 2125F/G");
    writingGroup->addCourse("Writing 2131F/G");
    module->addCourseGroup(writingGroup);

    // 0.5 course project
    auto projectGroup = std::make_shared<CourseGroup>("Project Course (0.5 course - required)", 0.5);
    projectGroup->addCourse("Computer Science 4490Z");
    module->addCourseGroup(projectGroup);

    // 1.0 course from senior courses
    auto seniorGroup = std::make_shared<SelectionCourseGroup>(
        "Senior Courses (1.0 course - select two)", 1.0, 2);
    seniorGroup->addCourse("Computer Science 4XXX");  // Any 4000 level CS course
    seniorGroup->addCourse("Data Science 3000A/B");
    module->addCourseGroup(seniorGroup);

    // 0.5 course from additional options
    auto additionalGroup = std::make_shared<SelectionCourseGroup>(
        "Additional Courses (0.5 course - select one)", 0.5, 1);
    additionalGroup->addCourse("Computer Science 3XXX");  // Any 3000+ level CS course
    additionalGroup->addCourse("Science 3377A/B");
    additionalGroup->addCourse("Mathematics 2156A/B");
    additionalGroup->addCourse("Mathematics 3159A/B");
    module->addCourseGroup(additionalGroup);

    // 0.5 course from stats options
    auto statsGroup = std::make_shared<SelectionCourseGroup>(
        "Statistics Requirement (0.5 course - select one)", 0.5, 1);
    statsGroup->addCourse("Statistical Sciences 2141A/B");
    statsGroup->addCourse("Statistical Sciences 2244A/B");
    statsGroup->addCourse("Biology 2244A/B");
    statsGroup->addCourse("Statistical Sciences 2857A/B");
    module->addCourseGroup(statsGroup);

    return module;
}

std::shared_ptr<Module> ModuleFactory::createHonorsIS() {
    auto module = std::make_shared<HonorsModule>("honors_is", "Honors Specialization in Information Systems (9.0 courses)");
    
    // 5.0 courses - required core
    auto coreGroup = std::make_shared<CourseGroup>("Required Core Courses (5.0 courses - all required)", 5.0);
    std::vector<std::string> coreCourses = {
        "Computer Science 2208A/B", "Computer Science 2209A/B",
        "Computer Science 2210A/B", "Computer Science 2211A/B",
        "Computer Science 2212A/B/Y", "Computer Science 3305A/B",
        "Computer Science 3307A/B/Y", "Computer Science 3331A/B",
        "Computer Science 3340A/B", "Computer Science 3350A/B"
    };
    for (const auto& course : coreCourses) {
        coreGroup->addCourse(course);
    }
    module->addCourseGroup(coreGroup);

    // 2.0 courses - IS specific
    auto isGroup = std::make_shared<CourseGroup>("Required Information Systems Courses (2.0 courses - all required)", 2.0);
    std::vector<std::string> isCourses = {
        "Computer Science 3346A/B", "Computer Science 3349A/B",
        "Computer Science 3357A/B", "Computer Science 3375A/B"
    };
    for (const auto& course : isCourses) {
        isGroup->addCourse(course);
    }
    module->addCourseGroup(isGroup);

    // 0.5 course from stats options
    auto statsGroup = std::make_shared<SelectionCourseGroup>(
        "Statistics Requirement (0.5 course - select one)", 0.5, 1);
    statsGroup->addCourse("Statistical Sciences 2141A/B");
    statsGroup->addCourse("Statistical Sciences 2244A/B");
    statsGroup->addCourse("Biology 2244A/B");
    statsGroup->addCourse("Statistical Sciences 2857A/B");
    module->addCourseGroup(statsGroup);

    // 1.5 courses from additional options
    auto additionalGroup = std::make_shared<SelectionCourseGroup>(
        "Additional Courses (1.5 courses - select three)", 1.5, 3);
    additionalGroup->addCourse("Computer Science 3XXX");  // Represents any 3000+ level CS course
    additionalGroup->addCourse("Data Science 3000A/B");
    additionalGroup->addCourse("Business 4XXX");  // Represents any 4000 level Business course
    module->addCourseGroup(additionalGroup);

    return module;
}

std::shared_ptr<Module> ModuleFactory::createHonorsBio() {
    auto module = std::make_shared<HonorsModule>("honors_bio", "Honors Specialization in Bioinformatics (10.0 courses)");
    
    // 6.0 courses - required core
    auto coreGroup = std::make_shared<CourseGroup>("Required Core Courses (6.0 courses - all required)", 6.0);
    std::vector<std::string> coreCourses = {
        "Computer Science 2208A/B", "Computer Science 2209A/B",
        "Computer Science 2210A/B", "Computer Science 2211A/B",
        "Computer Science 2212A/B/Y", "Computer Science 3307A/B/Y",
        "Computer Science 3331A/B", "Computer Science 3340A/B",
        "Computer Science 3350A/B"
    };
    for (const auto& course : coreCourses) {
        coreGroup->addCourse(course);
    }
    module->addCourseGroup(coreGroup);

    // 2.0 courses - biology core
    auto bioGroup = std::make_shared<CourseGroup>("Required Biology Courses (2.0 courses - all required)", 2.0);
    bioGroup->addCourse("Biology 2290F/G");
    bioGroup->addCourse("Biochemistry 2280A");
    module->addCourseGroup(bioGroup);

    // 1.0 course from stats options
    auto statsGroup = std::make_shared<SelectionCourseGroup>(
        "Statistics Requirement (1.0 course - select two)", 1.0, 2);
    statsGroup->addCourse("Statistical Sciences 2244A/B");
    statsGroup->addCourse("Biology 2244A/B");
    statsGroup->addCourse("Statistical Sciences 2857A/B");
    module->addCourseGroup(statsGroup);

    // 1.0 course from additional options
    auto additionalGroup = std::make_shared<SelectionCourseGroup>(
        "Additional Courses (1.0 course - select two)", 1.0, 2);
    additionalGroup->addCourse("Biology 3XXX");     // Any 3000+ level Biology course
    additionalGroup->addCourse("Biochemistry 3XXX"); // Any 3000+ level Biochemistry course
    additionalGroup->addCourse("Computer Science 3XXX"); // Any 3000+ level CS course
    additionalGroup->addCourse("Data Science 3000A/B");
    module->addCourseGroup(additionalGroup);

    return module;
}

std::shared_ptr<Module> ModuleFactory::createSpecCS() {
    auto module = std::make_shared<SpecializationModule>("spec_cs", "Specialization in Computer Science (8.0 courses)");
    
    // 5.0 courses - required core
    auto coreGroup = std::make_shared<CourseGroup>("Required Core Courses (5.0 courses - all required)", 5.0);
    std::vector<std::string> coreCourses = {
        "Computer Science 2208A/B", "Computer Science 2209A/B",
        "Computer Science 2210A/B", "Computer Science 2211A/B",
        "Computer Science 2212A/B/Y", "Computer Science 3305A/B",
        "Computer Science 3307A/B/Y", "Computer Science 3331A/B",
        "Computer Science 3340A/B", "Computer Science 3350A/B"
    };
    for (const auto& course : coreCourses) {
        coreGroup->addCourse(course);
    }
    module->addCourseGroup(coreGroup);

    // 2.0 courses - required additional
    auto additionalRequired = std::make_shared<CourseGroup>("Required Additional Courses (2.0 courses - all required)", 2.0);
    additionalRequired->addCourse("Computer Science 3342A/B");
    additionalRequired->addCourse("Computer Science 3349A/B");
    additionalRequired->addCourse("Computer Science 3357A/B");
    module->addCourseGroup(additionalRequired);

    // 1.0 course from electives
    auto electiveGroup = std::make_shared<SelectionCourseGroup>(
        "Additional Courses (1.0 course - select two)", 1.0, 2);
    electiveGroup->addCourse("Computer Science 3XXX"); // Any 3000+ level CS course
    electiveGroup->addCourse("Data Science 3000A/B");
    module->addCourseGroup(electiveGroup);

    return module;
}

std::shared_ptr<Module> ModuleFactory::createMajorCS() {
    auto module = std::make_shared<Module>("major_cs", "Major in Computer Science (6.0 courses)", 60.0, 50.0);
    
    // 4.5 courses - required core
    auto coreGroup = std::make_shared<CourseGroup>("Required Core Courses (4.5 courses - all required)", 4.5);
    std::vector<std::string> coreCourses = {
        "Computer Science 2208A/B", "Computer Science 2209A/B",
        "Computer Science 2210A/B", "Computer Science 2211A/B",
        "Computer Science 2212A/B/Y", "Computer Science 3307A/B/Y",
        "Computer Science 3331A/B", "Computer Science 3340A/B"
    };
    for (const auto& course : coreCourses) {
        coreGroup->addCourse(course);
    }
    module->addCourseGroup(coreGroup);

    // 1.5 courses from electives
    auto electiveGroup = std::make_shared<SelectionCourseGroup>(
        "Additional Courses (1.5 courses - select three)", 1.5, 3);
    electiveGroup->addCourse("Computer Science 3XXX");  // Any 3000+ level CS course
    module->addCourseGroup(electiveGroup);

    return module;
}

std::shared_ptr<Module> ModuleFactory::createMinorGame() {
    auto module = std::make_shared<MinorModule>("minor_game", "Minor in Game Development (4.0 courses)");
    
    // 2.5 courses - required core
    auto coreGroup = std::make_shared<CourseGroup>("Required Core Courses (2.5 courses - all required)", 2.5);
    std::vector<std::string> coreCourses = {
        "Computer Science 2210A/B",
        "Computer Science 2212A/B/Y",
        "Computer Science 3307A/B/Y",
        "Computer Science 3340A/B",
        "Computer Science 4470Y"
    };
    for (const auto& course : coreCourses) {
        coreGroup->addCourse(course);
    }
    module->addCourseGroup(coreGroup);

    // 1.5 courses from electives
    auto electiveGroup = std::make_shared<SelectionCourseGroup>(
        "Additional Courses (1.5 courses - select three)", 1.5, 3);
    electiveGroup->addCourse("Computer Science 3XXX");  // Any 3000+ level CS course
    module->addCourseGroup(electiveGroup);

    return module;
}

std::shared_ptr<Module> ModuleFactory::createMinorSE() {
    auto module = std::make_shared<MinorModule>("minor_se", "Minor in Software Engineering (4.0 courses)");
    
    // 2.5 courses - required core
    auto coreGroup = std::make_shared<CourseGroup>("Required Core Courses (2.5 courses - all required)", 2.5);
    std::vector<std::string> coreCourses = {
        "Computer Science 2209A/B",
        "Computer Science 2211A/B",
        "Computer Science 3305A/B",
        "Computer Science 3342A/B",
        "Computer Science 4470Y"
    };
    for (const auto& course : coreCourses) {
        coreGroup->addCourse(course);
    }
    module->addCourseGroup(coreGroup);

    // 1.5 courses from electives
    auto electiveGroup = std::make_shared<SelectionCourseGroup>(
        "Additional Courses (1.5 courses - select three)", 1.5, 3);
    electiveGroup->addCourse("Computer Science 3XXX");  // Any 3000+ level CS course
    module->addCourseGroup(electiveGroup);

    return module;
}

// StandardProgressCalculator Implementation
json StandardProgressCalculator::calculateProgress(
    const Module& module, const std::vector<std::string>& completedCourses) {
    
    json progress;
    progress["type"] = "progressCalculated";
    
    int totalRequired = 0;
    int completed = 0;
    
    // Calculate progress based on course groups
    auto requirements = module.getRequirements();
    for (const auto& group : requirements["courseGroups"]) {
        int groupRequired = static_cast<int>(group["requiredCredits"].get<double>() * 2);
        totalRequired += groupRequired;
        
        for (const auto& course : group["courses"]) {
            if (std::find(completedCourses.begin(), completedCourses.end(), course) != completedCourses.end()) {
                completed++;
            }
        }
    }
    
    double percentage = totalRequired > 0 ? (completed * 100.0) / totalRequired : 0;
    progress["progress"]["completed"] = completed;
    progress["progress"]["remaining"] = totalRequired - completed;
    progress["progress"]["percentage"] = static_cast<int>(std::round(percentage));
    
    return progress;
} 

// GraduationProgress Implementation
GraduationProgress::GraduationProgress() 
    : progressCalculator(std::make_unique<StandardProgressCalculator>()) {
    initializeModules();
}

void GraduationProgress::initializeModules() {
    // Initialize all modules using the factory
    std::vector<std::string> moduleIds = {
        "honors_cs", "honors_is", "honors_bio", 
        "spec_cs", "major_cs", "minor_game", "minor_se"
    };
    
    for (const auto& id : moduleIds) {
        auto module = ModuleFactory::createModule(id);
        if (module) {
            modules[id] = module;
        }
    }
}

std::string GraduationProgress::getModuleRequirements(const std::string& moduleId) {
    try {
        auto it = modules.find(moduleId);
        if (it == modules.end()) {
            throw std::runtime_error("Module not found");
        }

        json response;
        response["type"] = "moduleRequirements";
        response["requirements"] = it->second->getRequirements();
        return response.dump();
    } catch (const std::exception& e) {
        std::cerr << "Error getting module requirements: " << e.what() << std::endl;
        return "{\"type\":\"error\",\"message\":\"Failed to get module requirements\"}";
    }
}

std::string GraduationProgress::calculateProgress(
    const std::string& moduleId, const std::vector<std::string>& completedCourses) {
    try {
        auto it = modules.find(moduleId);
        if (it == modules.end()) {
            throw std::runtime_error("Module not found");
        }

        json progress = progressCalculator->calculateProgress(*it->second, completedCourses);
        saveProgress(moduleId, completedCourses);
        return progress.dump();
    } catch (const std::exception& e) {
        std::cerr << "Error calculating progress: " << e.what() << std::endl;
        return "{\"type\":\"error\",\"message\":\"Failed to calculate progress\"}";
    }
}

void GraduationProgress::saveProgress(
    const std::string& moduleId, const std::vector<std::string>& completedCourses) {
    try {
        std::filesystem::create_directories(std::filesystem::path(PROGRESS_PATH).parent_path());
        
        std::ofstream file(PROGRESS_PATH);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open progress file for writing");
        }

        file << "MODULE:" << moduleId << "\n";
        file << "COMPLETED_COURSES:\n";
        for (const auto& course : completedCourses) {
            file << course << "\n";
        }
        file.close();
    } catch (const std::exception& e) {
        std::cerr << "Error saving progress: " << e.what() << std::endl;
    }
}

std::vector<std::string> GraduationProgress::loadProgress(std::string& moduleId) {
    std::vector<std::string> completedCourses;
    try {
        if (!std::filesystem::exists(PROGRESS_PATH)) {
            return completedCourses;
        }

        std::ifstream file(PROGRESS_PATH);
        if (!file.is_open()) {
            return completedCourses;
        }

        std::string line;
        bool readingCourses = false;
        
        while (std::getline(file, line)) {
            if (line.substr(0, 7) == "MODULE:") {
                moduleId = line.substr(7);
            }
            else if (line == "COMPLETED_COURSES:") {
                readingCourses = true;
            }
            else if (readingCourses && !line.empty()) {
                completedCourses.push_back(line);
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error loading progress: " << e.what() << std::endl;
    }
    return completedCourses;
} 