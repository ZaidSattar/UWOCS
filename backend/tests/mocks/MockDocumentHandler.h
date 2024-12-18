#pragma once
#include <gmock/gmock.h>
#include "../../include/DocumentHandler.h"

class MockDocumentHandler : public DocumentHandler {
public:
    MOCK_METHOD(std::string, listDocuments, (), (override));
    MOCK_METHOD(std::vector<uint8_t>, getDocument, (const std::string& filename), (override));
    MOCK_METHOD(bool, documentExists, (const std::string& filename), (override));
}; 