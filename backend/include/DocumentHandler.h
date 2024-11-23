#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct Document {
    std::string id;
    std::string title;
    std::string filename;
};

class DocumentHandler {
public:
    DocumentHandler();
    
    // Returns JSON string of all documents
    std::string listDocuments();
    
    // Returns file content as binary data
    std::vector<char> getDocument(const std::string& filename);
    
private:
    const std::string STORAGE_PATH = "../storage/documents/";
    
    std::string sanitizeFilename(const std::string& filename);
    bool isValidPDF(const std::string& filepath);
};