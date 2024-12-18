#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../include/DocumentHandler.h"
#include "mocks/MockDocumentHandler.h"

using ::testing::Return;
using ::testing::_;
using ::testing::AtLeast;

class DocumentHandlerTest : public ::testing::Test {
protected:
    MockDocumentHandler mockHandler;
};

TEST_F(DocumentHandlerTest, ListDocumentsTest) {
    json expectedResponse = json::array({
        {{"filename", "test1.pdf"}, {"title", "Test Document 1"}},
        {{"filename", "test2.pdf"}, {"title", "Test Document 2"}}
    });
    
    EXPECT_CALL(mockHandler, listDocuments())
        .Times(1)
        .WillOnce(Return(expectedResponse.dump()));
    
    std::string response = mockHandler.listDocuments();
    json result = json::parse(response);
    
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result[0]["filename"], "test1.pdf");
}

TEST_F(DocumentHandlerTest, GetDocumentTest) {
    std::vector<uint8_t> expectedData = {'T', 'E', 'S', 'T'};
    
    EXPECT_CALL(mockHandler, documentExists("test.pdf"))
        .Times(1)
        .WillOnce(Return(true));
        
    EXPECT_CALL(mockHandler, getDocument("test.pdf"))
        .Times(1)
        .WillOnce(Return(expectedData));
    
    EXPECT_TRUE(mockHandler.documentExists("test.pdf"));
    auto result = mockHandler.getDocument("test.pdf");
    EXPECT_EQ(result, expectedData);
}

TEST_F(DocumentHandlerTest, InvalidDocumentTest) {
    // Test with invalid filename characters
    auto invalidData = mockHandler.getDocument("../invalid/path.pdf");
    EXPECT_TRUE(invalidData.empty());
    
    // Test with empty filename
    auto emptyNameData = mockHandler.getDocument("");
    EXPECT_TRUE(emptyNameData.empty());
} 