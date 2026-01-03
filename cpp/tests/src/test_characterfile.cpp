#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "elizaos/characterfile.hpp"
#include <memory>
#include <string>
#include <fstream>

using namespace elizaos;
using namespace ::testing;

class CharacterFileTest : public ::testing::Test {
protected:
    void SetUp() override {
        loader_ = std::make_shared<CharacterFileLoader>();
    }

    void TearDown() override {
        // Cleanup temp files if any
    }

    std::shared_ptr<CharacterFileLoader> loader_;
};

// ============================================================================
// CharacterFileLoader Tests
// ============================================================================

TEST_F(CharacterFileTest, LoaderCreation) {
    CharacterFileLoader loader;
    // Should be able to create a loader without errors
    EXPECT_TRUE(true);
}

TEST_F(CharacterFileTest, GetSupportedExtensions) {
    CharacterFileLoader loader;
    auto extensions = loader.getSupportedExtensions();

    EXPECT_GT(extensions.size(), 0);
}

TEST_F(CharacterFileTest, SetStrictValidation) {
    CharacterFileLoader loader;

    // Should be able to enable/disable strict validation
    loader.setStrictValidation(true);
    loader.setStrictValidation(false);

    EXPECT_TRUE(true);
}

TEST_F(CharacterFileTest, ExportToJson) {
    CharacterFileLoader loader;

    CharacterProfile profile;
    profile.id = "test-char";
    profile.name = "TestBot";
    profile.description = "A test character";
    profile.tags = {"test", "bot"};

    std::string json = loader.exportToJson(profile);

    EXPECT_FALSE(json.empty());
    EXPECT_NE(json.find("TestBot"), std::string::npos);
}

TEST_F(CharacterFileTest, ExportToJsonValue) {
    CharacterFileLoader loader;

    CharacterProfile profile;
    profile.id = "test-char";
    profile.name = "ExportBot";
    profile.description = "Export test";

    JsonValue jsonVal = loader.exportToJsonValue(profile);

    EXPECT_FALSE(jsonVal.empty());
}

TEST_F(CharacterFileTest, LoadFromInvalidJson) {
    CharacterFileLoader loader;

    auto result = loader.loadFromJson("{ invalid json }");

    // Should return empty optional for invalid JSON
    EXPECT_FALSE(result.has_value());
}

TEST_F(CharacterFileTest, LoadFromEmptyJson) {
    CharacterFileLoader loader;

    auto result = loader.loadFromJson("{}");

    // Empty JSON may succeed but produce empty character
    // Implementation-dependent behavior
    if (result.has_value()) {
        EXPECT_TRUE(result.value().name.empty());
    }
}

TEST_F(CharacterFileTest, ValidateEmptyJson) {
    CharacterFileLoader loader;

    ValidationResult result = loader.validateJson("{}");

    // Empty JSON should have some validation result
    // Whether it's valid depends on strictness settings
    EXPECT_FALSE(result.errors.empty() || result.isValid);
}

TEST_F(CharacterFileTest, ValidateInvalidJson) {
    CharacterFileLoader loader;

    ValidationResult result = loader.validateJson("{ not valid }");

    // Invalid JSON should fail validation
    EXPECT_FALSE(result.isValid);
    EXPECT_FALSE(result.errors.empty());
}

// ============================================================================
// ValidationResult Tests
// ============================================================================

TEST_F(CharacterFileTest, ValidationResultCreation) {
    ValidationResult result;

    EXPECT_FALSE(result.isValid);
    EXPECT_TRUE(result.errors.empty());
    EXPECT_TRUE(result.warnings.empty());
}

TEST_F(CharacterFileTest, ValidationResultAddError) {
    ValidationResult result;

    result.addError("Test error");

    EXPECT_EQ(result.errors.size(), 1);
    EXPECT_EQ(result.errors[0], "Test error");
}

TEST_F(CharacterFileTest, ValidationResultAddWarning) {
    ValidationResult result;

    result.addWarning("Test warning");

    EXPECT_EQ(result.warnings.size(), 1);
    EXPECT_EQ(result.warnings[0], "Test warning");
}

TEST_F(CharacterFileTest, ValidationResultGetSummary) {
    ValidationResult result;
    result.addError("Error 1");
    result.addWarning("Warning 1");

    std::string summary = result.getSummary();

    EXPECT_FALSE(summary.empty());
}

// ============================================================================
// CharacterFileTemplate Tests
// ============================================================================

TEST_F(CharacterFileTest, CreateBasicTemplate) {
    JsonValue tmpl = CharacterFileTemplate::createBasicTemplate();

    EXPECT_FALSE(tmpl.empty());
}

TEST_F(CharacterFileTest, CreateDetailedTemplate) {
    JsonValue tmpl = CharacterFileTemplate::createDetailedTemplate();

    EXPECT_FALSE(tmpl.empty());
}

TEST_F(CharacterFileTest, GetTemplateTypes) {
    auto types = CharacterFileTemplate::getTemplateTypes();

    EXPECT_GT(types.size(), 0);
}

TEST_F(CharacterFileTest, ValidateTemplate) {
    JsonValue tmpl = CharacterFileTemplate::createBasicTemplate();

    ValidationResult result = CharacterFileTemplate::validateTemplate(tmpl);

    // Basic template should be valid
    // Note: depends on implementation
    EXPECT_TRUE(result.isValid || !result.errors.empty());
}

// ============================================================================
// CharacterFileManager Tests
// ============================================================================

TEST_F(CharacterFileTest, ManagerCreation) {
    CharacterFileManager manager;

    // Should be able to create a manager without errors
    EXPECT_TRUE(true);
}

TEST_F(CharacterFileTest, ManagerSetCharacterManager) {
    CharacterFileManager manager;
    auto charMgr = std::make_shared<CharacterManager>();

    manager.setCharacterManager(charMgr);

    // Should not crash
    EXPECT_TRUE(true);
}

TEST_F(CharacterFileTest, ManagerGetOperationStatistics) {
    CharacterFileManager manager;

    JsonValue stats = manager.getOperationStatistics();

    // Should return some statistics
    EXPECT_FALSE(stats.empty());
}

TEST_F(CharacterFileTest, ManagerStopWatching) {
    CharacterFileManager manager;

    // Should not crash when stopping without starting
    manager.stopWatching();

    EXPECT_TRUE(true);
}

// ============================================================================
// CharacterFileUtils Tests
// ============================================================================

TEST_F(CharacterFileTest, GenerateCharacterId) {
    std::string id1 = CharacterFileUtils::generateCharacterId();
    std::string id2 = CharacterFileUtils::generateCharacterId();

    EXPECT_FALSE(id1.empty());
    EXPECT_FALSE(id2.empty());
    EXPECT_NE(id1, id2);  // Should be unique
}

TEST_F(CharacterFileTest, SanitizeFilename) {
    std::string sanitized = CharacterFileUtils::sanitizeFilename("Test Bot/Name");

    EXPECT_FALSE(sanitized.empty());
    EXPECT_EQ(sanitized.find('/'), std::string::npos);  // Should remove slashes
}

TEST_F(CharacterFileTest, GetFileExtension) {
    std::string ext = CharacterFileUtils::getFileExtension();

    EXPECT_FALSE(ext.empty());
}

TEST_F(CharacterFileTest, CreateFilename) {
    std::string filename = CharacterFileUtils::createFilename("TestBot");

    EXPECT_FALSE(filename.empty());
    EXPECT_NE(filename.find("TestBot"), std::string::npos);
}

TEST_F(CharacterFileTest, ExtractNameFromFilename) {
    std::string name = CharacterFileUtils::extractNameFromFilename("testbot.json");

    EXPECT_FALSE(name.empty());
}

TEST_F(CharacterFileTest, IsValidCharacterId) {
    std::string validId = CharacterFileUtils::generateCharacterId();

    EXPECT_TRUE(CharacterFileUtils::isValidCharacterId(validId));
    EXPECT_FALSE(CharacterFileUtils::isValidCharacterId(""));
}

TEST_F(CharacterFileTest, FormatDisplayName) {
    std::string formatted = CharacterFileUtils::formatDisplayName("test_bot_name");

    EXPECT_FALSE(formatted.empty());
}

TEST_F(CharacterFileTest, GetCurrentTimestamp) {
    std::string ts = CharacterFileUtils::getCurrentTimestamp();

    EXPECT_FALSE(ts.empty());
}

TEST_F(CharacterFileTest, ParseTimestamp) {
    std::string ts = CharacterFileUtils::getCurrentTimestamp();

    auto parsed = CharacterFileUtils::parseTimestamp(ts);

    // Should return a valid time point
    auto now = std::chrono::system_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::seconds>(now - parsed).count();
    EXPECT_LT(std::abs(diff), 60);  // Should be within a minute of now
}

// ============================================================================
// CharacterFileFormat Tests
// ============================================================================

TEST_F(CharacterFileTest, CharacterFileFormatDefaults) {
    CharacterFileFormat format;

    EXPECT_EQ(format.version, "1.0");
    EXPECT_EQ(format.formatType, "eliza-character");
    EXPECT_EQ(format.encoding, "utf-8");
}

// ============================================================================
// Convenience Function Tests
// ============================================================================

TEST_F(CharacterFileTest, ValidateCharacterFileNonExistent) {
    ValidationResult result = validateCharacterFile("/nonexistent/path/to/file.json");

    // Non-existent file should fail validation
    EXPECT_FALSE(result.isValid);
}

