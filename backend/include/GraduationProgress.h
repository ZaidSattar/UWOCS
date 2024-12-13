#ifndef GRADUATION_PROGRESS_H
#define GRADUATION_PROGRESS_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Base CourseGroup class
class CourseGroup {
public:
    CourseGroup(const std::string& title, double credits);
    virtual ~CourseGroup() = default;
    
    void addCourse(const std::string& course);
    virtual bool isCompleted(const std::vector<std::string>& completedCourses) const;
    virtual json toJson() const;

protected:
    std::string title;
    std::vector<std::string> courses;
    double requiredCredits;
};

// Specialized CourseGroup for selection-based groups
class SelectionCourseGroup : public CourseGroup {
public:
    SelectionCourseGroup(const std::string& title, double credits, int maxSelectable);
    bool isCompleted(const std::vector<std::string>& completedCourses) const override;

private:
    int maxSelectable;
};

// Base Module class
class Module {
public:
    Module(const std::string& id, const std::string& title, double minGpa, double minGrade);
    virtual ~Module() = default;
    
    void addCourseGroup(std::shared_ptr<CourseGroup> group);
    virtual json getRequirements() const;
    virtual bool meetsRequirements(const std::vector<std::string>& completedCourses, double gpa) const;

protected:
    std::string id;
    std::string title;
    double minGpaRequired;
    double minCourseGrade;
    std::vector<std::shared_ptr<CourseGroup>> courseGroups;
};

// Specialized Module classes
class HonorsModule : public Module {
public:
    HonorsModule(const std::string& id, const std::string& title);
    bool meetsRequirements(const std::vector<std::string>& completedCourses, double gpa) const override;
};

class SpecializationModule : public Module {
public:
    SpecializationModule(const std::string& id, const std::string& title);
};

class MinorModule : public Module {
public:
    MinorModule(const std::string& id, const std::string& title);
};

// Module Factory
class ModuleFactory {
public:
    static std::shared_ptr<Module> createModule(const std::string& moduleId);

private:
    static std::shared_ptr<Module> createHonorsCS();
    static std::shared_ptr<Module> createHonorsIS();
    static std::shared_ptr<Module> createHonorsBio();
    static std::shared_ptr<Module> createSpecCS();
    static std::shared_ptr<Module> createMajorCS();
    static std::shared_ptr<Module> createMinorGame();
    static std::shared_ptr<Module> createMinorSE();
};

// Progress Calculator
class ProgressCalculator {
public:
    virtual ~ProgressCalculator() = default;
    virtual json calculateProgress(const Module& module, const std::vector<std::string>& completedCourses) = 0;
};

class StandardProgressCalculator : public ProgressCalculator {
public:
    json calculateProgress(const Module& module, const std::vector<std::string>& completedCourses) override;
};

// Main GraduationProgress class
class GraduationProgress {
public:
    GraduationProgress();
    std::string getModuleRequirements(const std::string& moduleId);
    std::string calculateProgress(const std::string& moduleId, const std::vector<std::string>& completedCourses);
    std::vector<std::string> loadProgress(std::string& moduleId);

private:
    std::map<std::string, std::shared_ptr<Module>> modules;
    std::unique_ptr<ProgressCalculator> progressCalculator;
    void saveProgress(const std::string& moduleId, const std::vector<std::string>& completedCourses);
    void initializeModules();
};

#endif // GRADUATION_PROGRESS_H 