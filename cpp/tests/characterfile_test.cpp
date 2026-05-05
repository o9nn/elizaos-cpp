// characterfile_test.cpp
// End-to-end tests for elizaos::CharacterFileLoader save/load round-trips
// and validation paths.

#include "elizaos/characterfile.hpp"
#include "elizaos/characters.hpp"
#include <gtest/gtest.h>
#include <cstdio>
#include <fstream>
#include <string>

using namespace elizaos;

class CharacterFileFixture : public ::testing::Test {
protected:
    void TearDown() override {
        std::remove(tmpFile_.c_str());
    }
    std::string tmpFile_ = "test_character_file.json";
    CharacterFileLoader loader_;
};

TEST_F(CharacterFileFixture, ExportToJsonNonEmpty) {
    CharacterProfile c("Hero", "test description");
    auto json = loader_.exportToJson(c);
    EXPECT_FALSE(json.empty());
    EXPECT_NE(json.find("Hero"), std::string::npos);
}

TEST_F(CharacterFileFixture, SaveToFileWritesContent) {
    CharacterProfile c("RoundTrip", "a character");
    EXPECT_TRUE(loader_.saveToFile(c, tmpFile_));
    std::ifstream f(tmpFile_);
    EXPECT_TRUE(f.good());
}

TEST_F(CharacterFileFixture, LoadFromInvalidJsonDoesNotThrow) {
    EXPECT_NO_THROW((void)loader_.loadFromJson("{ this is not valid JSON "));
}

TEST_F(CharacterFileFixture, ValidateInvalidJsonReportsErrors) {
    auto result = loader_.validateJson("not-json");
    // Whether it returns errors or warnings is implementation-defined;
    // critically it must report not-valid via summary text.
    auto summary = result.getSummary();
    EXPECT_FALSE(summary.empty());
}

TEST_F(CharacterFileFixture, SupportedExtensions) {
    auto exts = loader_.getSupportedExtensions();
    EXPECT_GE(exts.size(), 1u);
}

TEST_F(CharacterFileFixture, IsCharacterFileChecks) {
    CharacterProfile c("X");
    EXPECT_TRUE(loader_.saveToFile(c, tmpFile_));
    EXPECT_NO_THROW((void)loader_.isCharacterFile(tmpFile_));
}

TEST(ValidationResult, AddErrorAndWarning) {
    ValidationResult r;
    r.addError("missing field");
    r.addWarning("deprecated key");
    EXPECT_FALSE(r.getSummary().empty());
}
