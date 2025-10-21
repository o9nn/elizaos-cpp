#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <optional>
#include <fstream>
#include <sstream>
#include "core.hpp"
#include "characters.hpp"
#include "agentlogger.hpp"
#include "plugin_specification.hpp"

namespace elizaos {

// JsonValue type definition (compatible with other modules)
using JsonValue = std::unordered_map<std::string, std::any>;

/**
 * Character file format specification
 */
struct CharacterFileFormat {
    std::string version = "1.0";
    std::string formatType = "eliza-character";
    std::string encoding = "utf-8";
};

/**
 * Character file validation result
 */
struct ValidationResult {
    bool isValid = false;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    void addError(const std::string& error);
    void addWarning(const std::string& warning);
    std::string getSummary() const;
};

/**
 * Character file loader and parser
 */
class CharacterFileLoader {
public:
    CharacterFileLoader();
    ~CharacterFileLoader();
    
    /**
     * Load character from JSON file
     */
    std::optional<CharacterProfile> loadFromFile(const std::string& filename);
    
    /**
     * Load character from JSON string
     */
    std::optional<CharacterProfile> loadFromJson(const std::string& jsonString);
    
    /**
     * Load character from JsonValue object
     */
    std::optional<CharacterProfile> loadFromJsonValue(const JsonValue& json);
    
    /**
     * Save character to JSON file
     */
    bool saveToFile(const CharacterProfile& character, const std::string& filename);
    
    /**
     * Export character to JSON string
     */
    std::string exportToJson(const CharacterProfile& character);
    
    /**
     * Export character to JsonValue object
     */
    JsonValue exportToJsonValue(const CharacterProfile& character);
    
    /**
     * Validate character file format
     */
    ValidationResult validateFile(const std::string& filename);
    
    /**
     * Validate character JSON string
     */
    ValidationResult validateJson(const std::string& jsonString);
    
    /**
     * Validate character JsonValue object
     */
    ValidationResult validateJsonValue(const JsonValue& json);
    
    /**
     * Get supported file extensions
     */
    std::vector<std::string> getSupportedExtensions() const;
    
    /**
     * Check if file is a character file
     */
    bool isCharacterFile(const std::string& filename);
    
    /**
     * Get character metadata without full loading
     */
    std::optional<PluginMetadata> getCharacterMetadata(const std::string& filename);
    
    /**
     * Batch load characters from directory
     */
    std::vector<CharacterProfile> loadFromDirectory(const std::string& directory, bool recursive = false);
    
    /**
     * Get loader statistics
     */
    JsonValue getStatistics() const;
    
    /**
     * Enable/disable strict validation
     */
    void setStrictValidation(bool enabled);
    
    /**
     * Set custom schema for validation
     */
    void setValidationSchema(const JsonValue& schema);
    
private:
    std::shared_ptr<AgentLogger> logger_;
    bool strictValidation_ = true;
    JsonValue validationSchema_;
    
    // Statistics
    size_t filesLoaded_ = 0;
    size_t filesError_ = 0;
    size_t filesSaved_ = 0;
    
public:
    // Helper methods (made public for template access)
    JsonValue parseJsonString(const std::string& jsonString);
    std::string readFileContents(const std::string& filename);
    bool writeFileContents(const std::string& filename, const std::string& content);

private:
    
    // Validation helpers
    bool validateSchema(const JsonValue& json, const JsonValue& schema);
    bool validateRequiredFields(const JsonValue& json);
    bool validateFieldTypes(const JsonValue& json);
    
    // Conversion helpers
    CharacterProfile jsonToCharacterProfile(const JsonValue& json);
    JsonValue characterProfileToJson(const CharacterProfile& character);
    
    // Personality conversion
    PersonalityMatrix parsePersonality(const JsonValue& personalityJson);
    JsonValue personalityToJson(const PersonalityMatrix& personality);
    
    // Trait conversion
    std::vector<CharacterTrait> parseTraits(const JsonValue& traitsJson);
    JsonValue traitsToJson(const std::vector<CharacterTrait>& traits);
    
    // Background conversion
    CharacterBackground parseBackground(const JsonValue& backgroundJson);
    JsonValue backgroundToJson(const CharacterBackground& background);
    
    // Communication style conversion
    CommunicationStyle parseCommunicationStyle(const JsonValue& commJson);
    JsonValue communicationStyleToJson(const CommunicationStyle& style);
    
    // Utility functions
    std::string getString(const JsonValue& json, const std::string& key, const std::string& defaultValue = "");
    float getFloat(const JsonValue& json, const std::string& key, float defaultValue = 0.0f);
    bool getBool(const JsonValue& json, const std::string& key, bool defaultValue = false);
    std::vector<std::string> getStringArray(const JsonValue& json, const std::string& key);
};

/**
 * Character file manager for bulk operations
 */
class CharacterFileManager {
public:
    CharacterFileManager();
    ~CharacterFileManager();
    
    /**
     * Set character manager for integration
     */
    void setCharacterManager(std::shared_ptr<CharacterManager> manager);
    
    /**
     * Import characters from directory into character manager
     */
    int importFromDirectory(const std::string& directory, bool recursive = false);
    
    /**
     * Export all characters from character manager to directory
     */
    int exportToDirectory(const std::string& directory);
    
    /**
     * Sync character files with character manager
     */
    bool syncWithManager(const std::string& directory);
    
    /**
     * Watch directory for character file changes
     */
    bool watchDirectory(const std::string& directory, bool autoImport = true);
    
    /**
     * Stop directory watching
     */
    void stopWatching();
    
    /**
     * Get import/export statistics
     */
    JsonValue getOperationStatistics() const;
    
    /**
     * Validate all character files in directory
     */
    std::vector<ValidationResult> validateDirectory(const std::string& directory);
    
    /**
     * Convert character files between formats
     */
    bool convertFormat(const std::string& inputFile, const std::string& outputFile, const std::string& targetFormat);
    
    /**
     * Backup character files
     */
    bool backupDirectory(const std::string& sourceDir, const std::string& backupDir);
    
private:
    std::shared_ptr<CharacterFileLoader> loader_;
    std::shared_ptr<CharacterManager> characterManager_;
    std::shared_ptr<AgentLogger> logger_;
    
    bool isWatching_ = false;
    std::string watchedDirectory_;
    
    // Operation statistics
    size_t importedCount_ = 0;
    size_t exportedCount_ = 0;
    size_t errorCount_ = 0;
    
    // Helper methods
    std::vector<std::string> findCharacterFiles(const std::string& directory, bool recursive);
    bool isValidCharacterFile(const std::string& filename);
    std::string getCharacterIdFromFile(const std::string& filename);
};

/**
 * Character file template system
 */
class CharacterFileTemplate {
public:
    /**
     * Create basic character template
     */
    static JsonValue createBasicTemplate();
    
    /**
     * Create detailed character template
     */
    static JsonValue createDetailedTemplate();
    
    /**
     * Create template from existing character
     */
    static JsonValue createTemplateFromCharacter(const CharacterProfile& character);
    
    /**
     * Generate character from template with parameters
     */
    static CharacterProfile generateFromTemplate(const JsonValue& templateJson, const JsonValue& parameters);
    
    /**
     * Validate template format
     */
    static ValidationResult validateTemplate(const JsonValue& templateJson);
    
    /**
     * Get available template types
     */
    static std::vector<std::string> getTemplateTypes();
    
    /**
     * Load template from file
     */
    static std::optional<JsonValue> loadTemplate(const std::string& filename);
    
    /**
     * Save template to file
     */
    static bool saveTemplate(const JsonValue& templateJson, const std::string& filename);
};

/**
 * Character file utilities
 */
namespace CharacterFileUtils {
    /**
     * Generate unique character ID
     */
    std::string generateCharacterId();
    
    /**
     * Sanitize character name for filename
     */
    std::string sanitizeFilename(const std::string& name);
    
    /**
     * Get character file extension
     */
    std::string getFileExtension();
    
    /**
     * Create character filename from name
     */
    std::string createFilename(const std::string& characterName);
    
    /**
     * Extract character name from filename
     */
    std::string extractNameFromFilename(const std::string& filename);
    
    /**
     * Validate character ID format
     */
    bool isValidCharacterId(const std::string& id);
    
    /**
     * Format character name for display
     */
    std::string formatDisplayName(const std::string& name);
    
    /**
     * Get current timestamp for character files
     */
    std::string getCurrentTimestamp();
    
    /**
     * Parse timestamp from character file
     */
    std::chrono::system_clock::time_point parseTimestamp(const std::string& timestamp);
}

// Global character file loader instance
extern std::shared_ptr<CharacterFileLoader> globalCharacterFileLoader;

// Convenience functions
std::optional<CharacterProfile> loadCharacterFromFile(const std::string& filename);
bool saveCharacterToFile(const CharacterProfile& character, const std::string& filename);
ValidationResult validateCharacterFile(const std::string& filename);

} // namespace elizaos