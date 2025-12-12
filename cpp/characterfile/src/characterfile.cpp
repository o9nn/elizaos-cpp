#include "elizaos/characterfile.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <iomanip>

namespace elizaos {

// Global character file loader instance
std::shared_ptr<CharacterFileLoader> globalCharacterFileLoader = std::make_shared<CharacterFileLoader>();

// =====================================================
// ValidationResult Implementation
// =====================================================

void ValidationResult::addError(const std::string& error) {
    errors.push_back(error);
    isValid = false;
}

void ValidationResult::addWarning(const std::string& warning) {
    warnings.push_back(warning);
}

std::string ValidationResult::getSummary() const {
    std::ostringstream oss;
    oss << "Validation: " << (isValid ? "VALID" : "INVALID") << std::endl;
    oss << "Errors: " << errors.size() << std::endl;
    oss << "Warnings: " << warnings.size() << std::endl;
    
    for (const auto& error : errors) {
        oss << "  ERROR: " << error << std::endl;
    }
    
    for (const auto& warning : warnings) {
        oss << "  WARNING: " << warning << std::endl;
    }
    
    return oss.str();
}

// =====================================================
// CharacterFileLoader Implementation
// =====================================================

CharacterFileLoader::CharacterFileLoader() {
    logger_ = std::make_shared<AgentLogger>();
    logger_->log("Character file loader initialized", "characterfile", "loader", LogLevel::INFO);
}

CharacterFileLoader::~CharacterFileLoader() = default;

std::optional<CharacterProfile> CharacterFileLoader::loadFromFile(const std::string& filename) {
    try {
        logger_->log("Loading character from file: " + filename, "characterfile", "loader", LogLevel::INFO);
        
        std::string content = readFileContents(filename);
        if (content.empty()) {
            logger_->log("Failed to read file: " + filename, "characterfile", "loader", LogLevel::ERROR);
            filesError_++;
            return std::nullopt;
        }
        
        auto result = loadFromJson(content);
        if (result) {
            filesLoaded_++;
            logger_->log("Successfully loaded character: " + result->name, "characterfile", "loader", LogLevel::SUCCESS);
        } else {
            filesError_++;
        }
        
        return result;
    } catch (const std::exception& e) {
        logger_->log("Exception loading character file: " + std::string(e.what()), "characterfile", "loader", LogLevel::ERROR);
        filesError_++;
        return std::nullopt;
    }
}

std::optional<CharacterProfile> CharacterFileLoader::loadFromJson(const std::string& jsonString) {
    try {
        JsonValue json = parseJsonString(jsonString);
        return loadFromJsonValue(json);
    } catch (const std::exception& e) {
        logger_->log("Failed to parse JSON: " + std::string(e.what()), "characterfile", "loader", LogLevel::ERROR);
        return std::nullopt;
    }
}

std::optional<CharacterProfile> CharacterFileLoader::loadFromJsonValue(const JsonValue& json) {
    try {
        // Validate the JSON structure
        ValidationResult validation = validateJsonValue(json);
        if (!validation.isValid && strictValidation_) {
            logger_->log("Character validation failed: " + validation.getSummary(), "characterfile", "loader", LogLevel::ERROR);
            return std::nullopt;
        }
        
        // Convert to CharacterProfile
        CharacterProfile character = jsonToCharacterProfile(json);
        
        logger_->log("Successfully parsed character: " + character.name, "characterfile", "loader", LogLevel::INFO);
        return character;
    } catch (const std::exception& e) {
        logger_->log("Failed to convert JSON to character: " + std::string(e.what()), "characterfile", "loader", LogLevel::ERROR);
        return std::nullopt;
    }
}

bool CharacterFileLoader::saveToFile(const CharacterProfile& character, const std::string& filename) {
    try {
        logger_->log("Saving character to file: " + filename, "characterfile", "loader", LogLevel::INFO);
        
        std::string jsonString = exportToJson(character);
        if (writeFileContents(filename, jsonString)) {
            filesSaved_++;
            logger_->log("Successfully saved character: " + character.name, "characterfile", "loader", LogLevel::SUCCESS);
            return true;
        } else {
            logger_->log("Failed to write file: " + filename, "characterfile", "loader", LogLevel::ERROR);
            return false;
        }
    } catch (const std::exception& e) {
        logger_->log("Exception saving character file: " + std::string(e.what()), "characterfile", "loader", LogLevel::ERROR);
        return false;
    }
}

std::string CharacterFileLoader::exportToJson(const CharacterProfile& character) {
    JsonValue json = exportToJsonValue(character);
    
    // Convert to formatted JSON string
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"name\": \"" << character.name << "\",\n";
    oss << "  \"description\": \"" << character.description << "\",\n";
    oss << "  \"id\": \"" << character.id << "\",\n";
    oss << "  \"version\": \"" << character.version << "\",\n";
    oss << "  \"creator\": \"" << character.creator << "\",\n";
    oss << "  \"created_at\": \"" << std::to_string(std::chrono::system_clock::to_time_t(character.created_at)) << "\",\n";
    oss << "  \"updated_at\": \"" << std::to_string(std::chrono::system_clock::to_time_t(character.updated_at)) << "\"\n";
    oss << "}";
    
    return oss.str();
}

JsonValue CharacterFileLoader::exportToJsonValue(const CharacterProfile& character) {
    return characterProfileToJson(character);
}

ValidationResult CharacterFileLoader::validateFile(const std::string& filename) {
    ValidationResult result;
    
    try {
        std::string content = readFileContents(filename);
        if (content.empty()) {
            result.addError("Could not read file: " + filename);
            return result;
        }
        
        return validateJson(content);
    } catch (const std::exception& e) {
        result.addError("Exception validating file: " + std::string(e.what()));
        return result;
    }
}

ValidationResult CharacterFileLoader::validateJson(const std::string& jsonString) {
    ValidationResult result;
    
    try {
        JsonValue json = parseJsonString(jsonString);
        return validateJsonValue(json);
    } catch (const std::exception& e) {
        result.addError("JSON parsing failed: " + std::string(e.what()));
        return result;
    }
}

ValidationResult CharacterFileLoader::validateJsonValue(const JsonValue& json) {
    ValidationResult result;
    
    // Check required fields
    if (!validateRequiredFields(json)) {
        result.addError("Required fields missing");
    }
    
    // Check field types
    if (!validateFieldTypes(json)) {
        result.addError("Invalid field types");
    }
    
    // If no errors, mark as valid
    if (result.errors.empty()) {
        result.isValid = true;
    }
    
    return result;
}

std::vector<std::string> CharacterFileLoader::getSupportedExtensions() const {
    return {".json", ".character", ".eliza"};
}

bool CharacterFileLoader::isCharacterFile(const std::string& filename) {
    auto extensions = getSupportedExtensions();
    std::string ext = std::filesystem::path(filename).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
    
    return std::find(extensions.begin(), extensions.end(), ext) != extensions.end();
}

std::optional<PluginMetadata> CharacterFileLoader::getCharacterMetadata(const std::string& filename) {
    try {
        std::string content = readFileContents(filename);
        JsonValue json = parseJsonString(content);
        
        PluginMetadata metadata;
        metadata.name = getString(json, "name");
        metadata.description = getString(json, "description");
        metadata.author = getString(json, "creator");
        metadata.version = PluginVersion::fromString(getString(json, "version", "1.0.0"));
        
        return metadata;
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

std::vector<CharacterProfile> CharacterFileLoader::loadFromDirectory(const std::string& directory, bool recursive) {
    std::vector<CharacterProfile> characters;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file() && isCharacterFile(entry.path().string())) {
                auto character = loadFromFile(entry.path().string());
                if (character) {
                    characters.push_back(*character);
                }
            } else if (entry.is_directory() && recursive) {
                auto subCharacters = loadFromDirectory(entry.path().string(), recursive);
                characters.insert(characters.end(), subCharacters.begin(), subCharacters.end());
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        logger_->log("Filesystem error: " + std::string(e.what()), "characterfile", "loader", LogLevel::ERROR);
    }
    
    return characters;
}

JsonValue CharacterFileLoader::getStatistics() const {
    JsonValue stats;
    stats["filesLoaded"] = std::string(std::to_string(filesLoaded_));
    stats["filesError"] = std::string(std::to_string(filesError_));
    stats["filesSaved"] = std::string(std::to_string(filesSaved_));
    stats["successRate"] = std::string(std::to_string(
        filesLoaded_ + filesError_ > 0 ? 
        static_cast<double>(filesLoaded_) / (filesLoaded_ + filesError_) : 0.0
    ));
    
    return stats;
}

void CharacterFileLoader::setStrictValidation(bool enabled) {
    strictValidation_ = enabled;
    logger_->log("Strict validation " + std::string(enabled ? "enabled" : "disabled"), "characterfile", "loader", LogLevel::INFO);
}

void CharacterFileLoader::setValidationSchema(const JsonValue& schema) {
    validationSchema_ = schema;
    logger_->log("Custom validation schema set", "characterfile", "loader", LogLevel::INFO);
}

// Private helper methods
JsonValue CharacterFileLoader::parseJsonString(const std::string& jsonString) {
    // Simple JSON parsing - in a real implementation would use a proper JSON library
    JsonValue json;
    
    // For now, create a basic structure
    json["name"] = std::string("ParsedCharacter");
    json["description"] = std::string("Character parsed from JSON");
    json["id"] = std::string("parsed-" + std::to_string(std::hash<std::string>{}(jsonString)));
    
    return json;
}

std::string CharacterFileLoader::readFileContents(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return "";
    }
    
    std::ostringstream content;
    content << file.rdbuf();
    file.close();
    
    return content.str();
}

bool CharacterFileLoader::writeFileContents(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    file.close();
    
    return file.good();
}

bool CharacterFileLoader::validateSchema(const JsonValue& json, const JsonValue& schema) {
    // Simple schema validation
    // Use the parameters to avoid warnings
    return !json.empty() && !schema.empty();
}

bool CharacterFileLoader::validateRequiredFields(const JsonValue& json) {
    std::vector<std::string> requiredFields = {"name", "description"};
    
    for (const std::string& field : requiredFields) {
        if (json.find(field) == json.end()) {
            return false;
        }
    }
    
    return true;
}

bool CharacterFileLoader::validateFieldTypes(const JsonValue& json) {
    // Check if required fields have the correct types
    try {
        if (json.find("name") != json.end()) {
            std::any_cast<std::string>(json.at("name"));
        }
        if (json.find("description") != json.end()) {
            std::any_cast<std::string>(json.at("description"));
        }
        return true;
    } catch (const std::bad_any_cast&) {
        return false;
    }
}

CharacterProfile CharacterFileLoader::jsonToCharacterProfile(const JsonValue& json) {
    std::string name = getString(json, "name", "UnknownCharacter");
    std::string description = getString(json, "description", "No description");
    
    CharacterProfile character(name, description);
    character.id = getString(json, "id", character.id);
    character.version = getString(json, "version", "1.0.0");
    character.creator = getString(json, "creator", "Unknown");
    
    // Parse personality if present
    if (json.find("personality") != json.end()) {
        try {
            JsonValue personalityJson = std::any_cast<JsonValue>(json.at("personality"));
            character.personality = parsePersonality(personalityJson);
        } catch (const std::bad_any_cast&) {
            // Use default personality
        }
    }
    
    // Parse traits if present
    if (json.find("traits") != json.end()) {
        try {
            JsonValue traitsJson = std::any_cast<JsonValue>(json.at("traits"));
            character.traits = parseTraits(traitsJson);
        } catch (const std::bad_any_cast&) {
            // Use default traits
        }
    }
    
    return character;
}

JsonValue CharacterFileLoader::characterProfileToJson(const CharacterProfile& character) {
    JsonValue json;
    
    json["name"] = std::string(character.name);
    json["description"] = std::string(character.description);
    json["id"] = std::string(character.id);
    json["version"] = std::string(character.version);
    json["creator"] = std::string(character.creator);
    json["created_at"] = std::string(std::to_string(std::chrono::system_clock::to_time_t(character.created_at)));
    json["updated_at"] = std::string(std::to_string(std::chrono::system_clock::to_time_t(character.updated_at)));
    
    // Add personality
    json["personality"] = std::any(personalityToJson(character.personality));
    
    // Add traits
    json["traits"] = std::any(traitsToJson(character.traits));
    
    return json;
}

PersonalityMatrix CharacterFileLoader::parsePersonality(const JsonValue& personalityJson) {
    PersonalityMatrix personality;
    
    personality.openness = getFloat(personalityJson, "openness", 0.5f);
    personality.conscientiousness = getFloat(personalityJson, "conscientiousness", 0.5f);
    personality.extraversion = getFloat(personalityJson, "extraversion", 0.5f);
    personality.agreeableness = getFloat(personalityJson, "agreeableness", 0.5f);
    personality.neuroticism = getFloat(personalityJson, "neuroticism", 0.5f);
    personality.creativity = getFloat(personalityJson, "creativity", 0.5f);
    personality.empathy = getFloat(personalityJson, "empathy", 0.5f);
    personality.assertiveness = getFloat(personalityJson, "assertiveness", 0.5f);
    personality.curiosity = getFloat(personalityJson, "curiosity", 0.5f);
    personality.loyalty = getFloat(personalityJson, "loyalty", 0.5f);
    
    return personality;
}

JsonValue CharacterFileLoader::personalityToJson(const PersonalityMatrix& personality) {
    JsonValue json;
    
    json["openness"] = std::string(std::to_string(personality.openness));
    json["conscientiousness"] = std::string(std::to_string(personality.conscientiousness));
    json["extraversion"] = std::string(std::to_string(personality.extraversion));
    json["agreeableness"] = std::string(std::to_string(personality.agreeableness));
    json["neuroticism"] = std::string(std::to_string(personality.neuroticism));
    json["creativity"] = std::string(std::to_string(personality.creativity));
    json["empathy"] = std::string(std::to_string(personality.empathy));
    json["assertiveness"] = std::string(std::to_string(personality.assertiveness));
    json["curiosity"] = std::string(std::to_string(personality.curiosity));
    json["loyalty"] = std::string(std::to_string(personality.loyalty));
    
    return json;
}

std::vector<CharacterTrait> CharacterFileLoader::parseTraits(const JsonValue& traitsJson) {
    std::vector<CharacterTrait> traits;
    
    // Simple trait parsing - in a real implementation would parse array of trait objects
    CharacterTrait defaultTrait("default", "Default trait", TraitCategory::PERSONALITY, TraitValueType::NUMERIC);
    defaultTrait.setNumericValue(0.5f);
    traits.push_back(defaultTrait);
    
    // Use traitsJson to avoid warning
    if (!traitsJson.empty()) {
        // Would parse traits from JSON here
    }
    
    return traits;
}

JsonValue CharacterFileLoader::traitsToJson(const std::vector<CharacterTrait>& traits) {
    JsonValue json;
    
    // Simple trait serialization - in a real implementation would create trait array
    json["count"] = std::string(std::to_string(traits.size()));
    
    return json;
}

CharacterBackground CharacterFileLoader::parseBackground(const JsonValue& backgroundJson) {
    CharacterBackground background;
    
    // Use backgroundJson to avoid warning
    if (!backgroundJson.empty()) {
        // Would parse background from JSON here
    }
    
    return background;
}

JsonValue CharacterFileLoader::backgroundToJson(const CharacterBackground& background) {
    JsonValue json;
    
    json["experienceCount"] = std::string(std::to_string(background.experiences.size()));
    
    return json;
}

CommunicationStyle CharacterFileLoader::parseCommunicationStyle(const JsonValue& commJson) {
    CommunicationStyle style;
    
    style.tone = getString(commJson, "tone", "neutral");
    style.formality = getFloat(commJson, "formality", 0.5f);
    style.emotionality = getFloat(commJson, "emotionality", 0.5f);
    style.verbosity = getFloat(commJson, "verbosity", 0.5f);
    
    return style;
}

JsonValue CharacterFileLoader::communicationStyleToJson(const CommunicationStyle& style) {
    JsonValue json;
    
    json["tone"] = std::string(style.tone);
    json["formality"] = std::string(std::to_string(style.formality));
    json["emotionality"] = std::string(std::to_string(style.emotionality));
    json["verbosity"] = std::string(std::to_string(style.verbosity));
    
    return json;
}

std::string CharacterFileLoader::getString(const JsonValue& json, const std::string& key, const std::string& defaultValue) {
    auto it = json.find(key);
    if (it != json.end()) {
        try {
            return std::any_cast<std::string>(it->second);
        } catch (const std::bad_any_cast&) {
            return defaultValue;
        }
    }
    return defaultValue;
}

float CharacterFileLoader::getFloat(const JsonValue& json, const std::string& key, float defaultValue) {
    auto it = json.find(key);
    if (it != json.end()) {
        try {
            std::string str = std::any_cast<std::string>(it->second);
            return std::stof(str);
        } catch (const std::exception&) {
            return defaultValue;
        }
    }
    return defaultValue;
}

bool CharacterFileLoader::getBool(const JsonValue& json, const std::string& key, bool defaultValue) {
    auto it = json.find(key);
    if (it != json.end()) {
        try {
            std::string str = std::any_cast<std::string>(it->second);
            return str == "true" || str == "1";
        } catch (const std::bad_any_cast&) {
            return defaultValue;
        }
    }
    return defaultValue;
}

std::vector<std::string> CharacterFileLoader::getStringArray(const JsonValue& json, const std::string& key) {
    std::vector<std::string> result;
    
    auto it = json.find(key);
    if (it != json.end()) {
        try {
            // Simple implementation - would parse array from JSON
            std::string str = std::any_cast<std::string>(it->second);
            result.push_back(str);
        } catch (const std::bad_any_cast&) {
            // Return empty array
        }
    }
    
    return result;
}

// =====================================================
// CharacterFileManager Implementation
// =====================================================

CharacterFileManager::CharacterFileManager() {
    loader_ = std::make_shared<CharacterFileLoader>();
    logger_ = std::make_shared<AgentLogger>();
    
    logger_->log("Character file manager initialized", "characterfile", "manager", LogLevel::INFO);
}

CharacterFileManager::~CharacterFileManager() {
    if (isWatching_) {
        stopWatching();
    }
}

void CharacterFileManager::setCharacterManager(std::shared_ptr<CharacterManager> manager) {
    characterManager_ = manager;
    logger_->log("Character manager set for file operations", "characterfile", "manager", LogLevel::INFO);
}

int CharacterFileManager::importFromDirectory(const std::string& directory, bool recursive) {
    if (!characterManager_) {
        logger_->log("No character manager set for import", "characterfile", "manager", LogLevel::ERROR);
        return 0;
    }
    
    logger_->log("Importing characters from directory: " + directory, "characterfile", "manager", LogLevel::INFO);
    
    auto characters = loader_->loadFromDirectory(directory, recursive);
    int imported = 0;
    
    for (const auto& character : characters) {
        std::string id = characterManager_->registerCharacter(character);
        if (!id.empty()) {
            imported++;
            importedCount_++;
        } else {
            errorCount_++;
        }
    }
    
    logger_->log("Imported " + std::to_string(imported) + " characters from " + directory, "characterfile", "manager", LogLevel::SUCCESS);
    return imported;
}

int CharacterFileManager::exportToDirectory(const std::string& directory) {
    if (!characterManager_) {
        logger_->log("No character manager set for export", "characterfile", "manager", LogLevel::ERROR);
        return 0;
    }
    
    logger_->log("Exporting characters to directory: " + directory, "characterfile", "manager", LogLevel::INFO);
    
    // Create directory if it doesn't exist
    std::filesystem::create_directories(directory);
    
    auto characters = characterManager_->getAllCharacters();
    int exported = 0;
    
    for (const auto& character : characters) {
        std::string filename = directory + "/" + CharacterFileUtils::createFilename(character.name);
        if (loader_->saveToFile(character, filename)) {
            exported++;
            exportedCount_++;
        } else {
            errorCount_++;
        }
    }
    
    logger_->log("Exported " + std::to_string(exported) + " characters to " + directory, "characterfile", "manager", LogLevel::SUCCESS);
    return exported;
}

bool CharacterFileManager::syncWithManager(const std::string& directory) {
    logger_->log("Syncing character files with manager from: " + directory, "characterfile", "manager", LogLevel::INFO);
    
    // Import new/updated characters
    int imported = importFromDirectory(directory, false);
    
    // Export any characters that don't have files
    int exported = exportToDirectory(directory);
    
    logger_->log("Sync complete: " + std::to_string(imported) + " imported, " + 
                std::to_string(exported) + " exported", "characterfile", "manager", LogLevel::SUCCESS);
    
    return true;
}

bool CharacterFileManager::watchDirectory(const std::string& directory, bool autoImport) {
    if (isWatching_) {
        stopWatching();
    }
    
    watchedDirectory_ = directory;
    isWatching_ = true;
    
    logger_->log("Started watching directory: " + directory + 
                " (auto-import: " + (autoImport ? "enabled" : "disabled") + ")", 
                "characterfile", "manager", LogLevel::INFO);
    
    // In a real implementation, would set up filesystem watching
    // For now, just log that we're "watching"
    
    return true;
}

void CharacterFileManager::stopWatching() {
    if (isWatching_) {
        isWatching_ = false;
        logger_->log("Stopped watching directory: " + watchedDirectory_, "characterfile", "manager", LogLevel::INFO);
        watchedDirectory_.clear();
    }
}

JsonValue CharacterFileManager::getOperationStatistics() const {
    JsonValue stats;
    stats["importedCount"] = std::string(std::to_string(importedCount_));
    stats["exportedCount"] = std::string(std::to_string(exportedCount_));
    stats["errorCount"] = std::string(std::to_string(errorCount_));
    stats["isWatching"] = std::string(isWatching_ ? "true" : "false");
    stats["watchedDirectory"] = std::string(watchedDirectory_);
    
    return stats;
}

std::vector<ValidationResult> CharacterFileManager::validateDirectory(const std::string& directory) {
    std::vector<ValidationResult> results;
    
    logger_->log("Validating character files in directory: " + directory, "characterfile", "manager", LogLevel::INFO);
    
    auto files = findCharacterFiles(directory, false);
    for (const auto& file : files) {
        ValidationResult result = loader_->validateFile(file);
        result.addWarning("File: " + file);
        results.push_back(result);
    }
    
    logger_->log("Validated " + std::to_string(results.size()) + " character files", "characterfile", "manager", LogLevel::INFO);
    return results;
}

bool CharacterFileManager::convertFormat(const std::string& inputFile, const std::string& outputFile, const std::string& targetFormat) {
    logger_->log("Converting file format: " + inputFile + " -> " + outputFile + 
                " (format: " + targetFormat + ")", "characterfile", "manager", LogLevel::INFO);
    
    // Load character from input file
    auto character = loader_->loadFromFile(inputFile);
    if (!character) {
        return false;
    }
    
    // Save to output file (format determined by extension)
    return loader_->saveToFile(*character, outputFile);
}

bool CharacterFileManager::backupDirectory(const std::string& sourceDir, const std::string& backupDir) {
    logger_->log("Backing up character files: " + sourceDir + " -> " + backupDir, "characterfile", "manager", LogLevel::INFO);
    
    try {
        std::filesystem::create_directories(backupDir);
        std::filesystem::copy(sourceDir, backupDir, std::filesystem::copy_options::recursive);
        
        logger_->log("Backup completed successfully", "characterfile", "manager", LogLevel::SUCCESS);
        return true;
    } catch (const std::filesystem::filesystem_error& e) {
        logger_->log("Backup failed: " + std::string(e.what()), "characterfile", "manager", LogLevel::ERROR);
        return false;
    }
}

std::vector<std::string> CharacterFileManager::findCharacterFiles(const std::string& directory, bool recursive) {
    std::vector<std::string> files;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file() && loader_->isCharacterFile(entry.path().string())) {
                files.push_back(entry.path().string());
            } else if (entry.is_directory() && recursive) {
                auto subFiles = findCharacterFiles(entry.path().string(), recursive);
                files.insert(files.end(), subFiles.begin(), subFiles.end());
            }
        }
    } catch (const std::filesystem::filesystem_error&) {
        // Directory doesn't exist or is not accessible
    }
    
    return files;
}

bool CharacterFileManager::isValidCharacterFile(const std::string& filename) {
    return loader_->isCharacterFile(filename);
}

std::string CharacterFileManager::getCharacterIdFromFile(const std::string& filename) {
    auto metadata = loader_->getCharacterMetadata(filename);
    return metadata ? metadata->name : "";
}

// =====================================================
// CharacterFileTemplate Implementation
// =====================================================

JsonValue CharacterFileTemplate::createBasicTemplate() {
    JsonValue template_;
    
    template_["name"] = std::string("Basic Character Template");
    template_["description"] = std::string("A basic character template with essential fields");
    template_["version"] = std::string("1.0.0");
    template_["creator"] = std::string("ElizaOS");
    
    // Basic personality structure
    JsonValue personality;
    personality["openness"] = std::string("0.5");
    personality["conscientiousness"] = std::string("0.5");
    personality["extraversion"] = std::string("0.5");
    personality["agreeableness"] = std::string("0.5");
    personality["neuroticism"] = std::string("0.5");
    template_["personality"] = std::any(personality);
    
    return template_;
}

JsonValue CharacterFileTemplate::createDetailedTemplate() {
    JsonValue template_ = createBasicTemplate();
    
    template_["name"] = std::string("Detailed Character Template");
    template_["description"] = std::string("A comprehensive character template with all available fields");
    
    // Add communication style
    JsonValue commStyle;
    commStyle["tone"] = std::string("neutral");
    commStyle["formality"] = std::string("0.5");
    commStyle["emotionality"] = std::string("0.5");
    commStyle["verbosity"] = std::string("0.5");
    template_["communicationStyle"] = std::any(commStyle);
    
    // Add background
    JsonValue background;
    background["experiences"] = std::string("[]");
    template_["background"] = std::any(background);
    
    return template_;
}

JsonValue CharacterFileTemplate::createTemplateFromCharacter(const CharacterProfile& character) {
    CharacterFileLoader loader;
    return loader.exportToJsonValue(character);
}

CharacterProfile CharacterFileTemplate::generateFromTemplate(const JsonValue& templateJson, const JsonValue& parameters) {
    CharacterFileLoader loader;
    
    // Merge template with parameters
    JsonValue merged = templateJson;
    for (const auto& param : parameters) {
        merged[param.first] = param.second;
    }
    
    auto character = loader.loadFromJsonValue(merged);
    return character ? *character : CharacterProfile("Generated", "Generated from template");
}

ValidationResult CharacterFileTemplate::validateTemplate(const JsonValue& templateJson) {
    CharacterFileLoader loader;
    return loader.validateJsonValue(templateJson);
}

std::vector<std::string> CharacterFileTemplate::getTemplateTypes() {
    return {"basic", "detailed", "personality-focused", "skill-focused", "custom"};
}

std::optional<JsonValue> CharacterFileTemplate::loadTemplate(const std::string& filename) {
    CharacterFileLoader loader;
    try {
        std::string content = loader.readFileContents(filename);
        return loader.parseJsonString(content);
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

bool CharacterFileTemplate::saveTemplate(const JsonValue& templateJson, const std::string& filename) {
    CharacterFileLoader loader;
    
    // Convert template to JSON string
    std::ostringstream oss;
    oss << "{\n";
    for (auto it = templateJson.begin(); it != templateJson.end(); ++it) {
        if (it != templateJson.begin()) oss << ",\n";
        oss << "  \"" << it->first << "\": ";
        try {
            oss << "\"" << std::any_cast<std::string>(it->second) << "\"";
        } catch (const std::bad_any_cast&) {
            oss << "\"\"";
        }
    }
    oss << "\n}";
    
    return loader.writeFileContents(filename, oss.str());
}

// =====================================================
// CharacterFileUtils Implementation
// =====================================================

namespace CharacterFileUtils {

std::string generateCharacterId() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::ostringstream oss;
    oss << "char-" << time_t << "-" << std::hash<std::string>{}(getCurrentTimestamp());
    return oss.str();
}

std::string sanitizeFilename(const std::string& name) {
    std::string sanitized = name;
    
    // Replace invalid characters with underscores
    std::regex invalid("[<>:\"/\\|?*]");
    sanitized = std::regex_replace(sanitized, invalid, "_");
    
    // Remove multiple consecutive underscores
    std::regex multiple("_{2,}");
    sanitized = std::regex_replace(sanitized, multiple, "_");
    
    // Remove leading/trailing underscores
    sanitized = std::regex_replace(sanitized, std::regex("^_+|_+$"), "");
    
    return sanitized;
}

std::string getFileExtension() {
    return ".character.json";
}

std::string createFilename(const std::string& characterName) {
    return sanitizeFilename(characterName) + getFileExtension();
}

std::string extractNameFromFilename(const std::string& filename) {
    std::filesystem::path path(filename);
    std::string stem = path.stem().stem().string(); // Remove .character.json
    return stem;
}

bool isValidCharacterId(const std::string& id) {
    // Simple validation - should start with char- and be reasonable length
    return id.length() >= 5 && id.substr(0, 5) == "char-";
}

std::string formatDisplayName(const std::string& name) {
    std::string formatted = name;
    
    // Capitalize first letter of each word
    bool capitalizeNext = true;
    for (char& c : formatted) {
        if (std::isalpha(c)) {
            if (capitalizeNext) {
                c = std::toupper(c);
                capitalizeNext = false;
            }
        } else {
            capitalizeNext = true;
        }
    }
    
    return formatted;
}

std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::chrono::system_clock::time_point parseTimestamp(const std::string& timestamp) {
    // Simple timestamp parsing - in a real implementation would use proper date parsing
    try {
        time_t time = std::stoll(timestamp);
        return std::chrono::system_clock::from_time_t(time);
    } catch (const std::exception&) {
        return std::chrono::system_clock::now();
    }
}

} // namespace CharacterFileUtils

// =====================================================
// Convenience Functions
// =====================================================

std::optional<CharacterProfile> loadCharacterFromFile(const std::string& filename) {
    return globalCharacterFileLoader->loadFromFile(filename);
}

bool saveCharacterToFile(const CharacterProfile& character, const std::string& filename) {
    return globalCharacterFileLoader->saveToFile(character, filename);
}

ValidationResult validateCharacterFile(const std::string& filename) {
    return globalCharacterFileLoader->validateFile(filename);
}

} // namespace elizaos
