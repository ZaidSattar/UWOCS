#ifndef DOCUMENT_HANDLER_H
#define DOCUMENT_HANDLER_H

#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

const std::string STORAGE_PATH = "../storage/documents/";

class Document {
public:
    explicit Document(const std::string& filename);
    json toJson() const;

private:
    std::string filename;
    std::string title;
    std::string id;
};

class FileValidator {
public:
    virtual ~FileValidator() = default;
    virtual bool isValid(const std::string& filepath) const = 0;
};

class PDFValidator : public FileValidator {
public:
    bool isValid(const std::string& filepath) const override;
};

class FilePathSanitizer {
public:
    virtual ~FilePathSanitizer() = default;
    virtual std::string sanitize(const std::string& filename) const;
};

class DocumentStorage {
public:
    explicit DocumentStorage(const std::string& storagePath);
    std::vector<Document> listDocuments() const;
    std::vector<char> readFile(const std::string& filepath) const;

private:
    std::string storagePath;
    void ensureStorageExists();
};

class DocumentHandler {
public:
    DocumentHandler();
    std::string listDocuments();
    std::vector<char> getDocument(const std::string& filename);

private:
    DocumentStorage storage;
    std::unique_ptr<FileValidator> validator;
    std::unique_ptr<FilePathSanitizer> sanitizer;
};

#endif // DOCUMENT_HANDLER_H