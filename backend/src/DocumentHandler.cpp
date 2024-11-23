#include "../include/DocumentHandler.h"
#include <fstream>
#include <sstream>
#include <iostream>

DocumentHandler::DocumentHandler() {
    // Ensure storage directory exists
    std::filesystem::create_directories(STORAGE_PATH);
}

std::string DocumentHandler::listDocuments() {
    json documents = json::array();
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(STORAGE_PATH)) {
            if (entry.path().extension() == ".pdf") {
                Document doc;
                doc.filename = entry.path().filename().string();
                doc.title = doc.filename.substr(0, doc.filename.find_last_of('.'));
                doc.id = std::to_string(std::hash<std::string>{}(doc.filename));
                
                documents.push_back({
                    {"id", doc.id},
                    {"title", doc.title},
                    {"filename", doc.filename}
                });
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error listing documents: " << e.what() << std::endl;
    }
    
    return documents.dump();
}

std::vector<char> DocumentHandler::getDocument(const std::string& filename) {
    std::string filepath = STORAGE_PATH + sanitizeFilename(filename);
    std::vector<char> buffer;
    
    if (!isValidPDF(filepath)) {
        return buffer;
    }
    
    std::ifstream file(filepath, std::ios::binary);
    if (file) {
        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        buffer.resize(fileSize);
        
        file.seekg(0);
        file.read(buffer.data(), fileSize);
    }
    
    return buffer;
}

std::string DocumentHandler::sanitizeFilename(const std::string& filename) {
    std::string clean = filename;
    clean.erase(std::remove(clean.begin(), clean.end(), '/'), clean.end());
    clean.erase(std::remove(clean.begin(), clean.end(), '\\'), clean.end());
    return clean;
}

bool DocumentHandler::isValidPDF(const std::string& filepath) {
    return std::filesystem::exists(filepath) && 
           filepath.substr(filepath.find_last_of('.')) == ".pdf";
}