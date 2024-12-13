#include "../include/DocumentHandler.h"
#include <fstream>
#include <sstream>
#include <iostream>

// Document class implementation
Document::Document(const std::string& filename)
    : filename(filename),
      title(filename.substr(0, filename.find_last_of('.'))),
      id(std::to_string(std::hash<std::string>{}(filename))) {}

json Document::toJson() const {
    return {
        {"id", id},
        {"title", title},
        {"filename", filename}
    };
}

// FileValidator implementation
bool PDFValidator::isValid(const std::string& filepath) const {
    return std::filesystem::exists(filepath) && 
           filepath.substr(filepath.find_last_of('.')) == ".pdf";
}

// FilePathSanitizer implementation
std::string FilePathSanitizer::sanitize(const std::string& filename) const {
    std::string clean = filename;
    clean.erase(std::remove(clean.begin(), clean.end(), '/'), clean.end());
    clean.erase(std::remove(clean.begin(), clean.end(), '\\'), clean.end());
    return clean;
}

// DocumentStorage implementation
DocumentStorage::DocumentStorage(const std::string& storagePath) 
    : storagePath(storagePath) {
    ensureStorageExists();
}

void DocumentStorage::ensureStorageExists() {
    std::filesystem::create_directories(storagePath);
}

std::vector<char> DocumentStorage::readFile(const std::string& filepath) const {
    std::vector<char> buffer;
    std::ifstream file(filepath, std::ios::binary);
    
    if (file) {
        file.seekg(0, std::ios::end);
        buffer.resize(file.tellg());
        file.seekg(0);
        file.read(buffer.data(), buffer.size());
    }
    
    return buffer;
}

std::vector<Document> DocumentStorage::listDocuments() const {
    std::vector<Document> documents;
    
    try {
        std::cout << "Searching for documents in: " << storagePath << std::endl;
        
        if (!std::filesystem::exists(storagePath)) {
            std::cout << "Storage directory does not exist!" << std::endl;
            return documents;
        }

        for (const auto& entry : std::filesystem::directory_iterator(storagePath)) {
            std::cout << "Found file: " << entry.path().string() << std::endl;
            
            if (entry.path().extension() == ".pdf") {
                std::cout << "Adding PDF: " << entry.path().filename().string() << std::endl;
                documents.emplace_back(entry.path().filename().string());
            }
        }
        
        std::cout << "Total documents found: " << documents.size() << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error listing documents: " << e.what() << std::endl;
    }
    
    return documents;
}

// DocumentHandler implementation
DocumentHandler::DocumentHandler() 
    : storage(STORAGE_PATH),
      validator(std::make_unique<PDFValidator>()),
      sanitizer(std::make_unique<FilePathSanitizer>()) 
{
    std::cout << "DocumentHandler initialized with path: " << STORAGE_PATH << std::endl;
    
    // Print current working directory
    std::cout << "Current working directory: " 
              << std::filesystem::current_path().string() << std::endl;
}

std::string DocumentHandler::listDocuments() {
    json documentsJson = json::array();
    
    try {
        auto documents = storage.listDocuments();
        for (const auto& doc : documents) {
            documentsJson.push_back(doc.toJson());
        }
    } catch (const std::exception& e) {
        std::cerr << "Error creating document list: " << e.what() << std::endl;
    }
    
    return documentsJson.dump();
}

std::vector<char> DocumentHandler::getDocument(const std::string& filename) {
    std::string sanitizedName = sanitizer->sanitize(filename);
    std::string filepath = STORAGE_PATH + sanitizedName;
    
    if (!validator->isValid(filepath)) {
        return std::vector<char>();
    }
    
    return storage.readFile(filepath);
}