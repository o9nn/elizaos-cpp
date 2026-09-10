#include "elizaos/characterfile.hpp"
#include <fstream>
#include <sstream>
#include <atomic>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <iomanip>
#include <cctype>
#include <condition_variable>
#include <limits>
#include <system_error>
#include <thread>
#include <utility>

namespace elizaos {

namespace {

std::string jsonEscape(const std::string& input) {
    std::ostringstream escaped;
    for (unsigned char c : input) {
        switch (c) {
            case '"': escaped << "\\\""; break;
            case '\\': escaped << "\\\\"; break;
            case '\b': escaped << "\\b"; break;
            case '\f': escaped << "\\f"; break;
            case '\n': escaped << "\\n"; break;
            case '\r': escaped << "\\r"; break;
            case '\t': escaped << "\\t"; break;
            default:
                if (c < 0x20) {
                    escaped << "\\u" << std::hex << std::setw(4) << std::setfill('0')
                            << static_cast<int>(c) << std::dec << std::setfill(' ');
                } else {
                    escaped << static_cast<char>(c);
                }
        }
    }
    return escaped.str();
}

std::string jsonUnescape(const std::string& input) {
    std::string out;
    out.reserve(input.size());
    for (size_t i = 0; i < input.size(); ++i) {
        if (input[i] != '\\') {
            out.push_back(input[i]);
            continue;
        }
        if (++i >= input.size()) throw std::runtime_error("Incomplete JSON escape");
        const char escaped = input[i];
        switch (escaped) {
            case '"': out.push_back('"'); break;
            case '\\': out.push_back('\\'); break;
            case '/': out.push_back('/'); break;
            case 'b': out.push_back('\b'); break;
            case 'f': out.push_back('\f'); break;
            case 'n': out.push_back('\n'); break;
            case 'r': out.push_back('\r'); break;
            case 't': out.push_back('\t'); break;
            case 'u':
                if (i + 4 >= input.size()) throw std::runtime_error("Incomplete unicode escape");
                out.append("\\u");
                for (int digit = 0; digit < 4; ++digit) {
                    const char hexadecimal = input[++i];
                    if (!std::isxdigit(static_cast<unsigned char>(hexadecimal))) {
                        throw std::runtime_error("Invalid unicode escape");
                    }
                    out.push_back(hexadecimal);
                }
                break;
            default:
                throw std::runtime_error("Invalid JSON escape");
        }
    }
    return out;
}

void skipWhitespace(const std::string& input, size_t& position) {
    while (position < input.size() &&
           std::isspace(static_cast<unsigned char>(input[position]))) {
        ++position;
    }
}

std::string parseJsonStringLiteral(const std::string& input, size_t& position) {
    if (position >= input.size() || input[position] != '"') {
        throw std::runtime_error("Expected JSON string literal");
    }
    ++position;
    std::string raw;
    while (position < input.size()) {
        const unsigned char character = static_cast<unsigned char>(input[position++]);
        if (character == '"') return jsonUnescape(raw);
        if (character < 0x20) throw std::runtime_error("Unescaped control character in JSON string");
        if (character == '\\') {
            if (position >= input.size()) throw std::runtime_error("Incomplete JSON escape");
            raw.push_back('\\');
            raw.push_back(input[position++]);
        } else {
            raw.push_back(static_cast<char>(character));
        }
    }
    throw std::runtime_error("Unterminated JSON string literal");
}

bool isJsonNumber(const std::string& value) {
    static const std::regex numberPattern(
        R"(^-?(0|[1-9][0-9]*)(\.[0-9]+)?([eE][+-]?[0-9]+)?$)");
    return std::regex_match(value, numberPattern);
}

void consumeJsonValue(const std::string& input, size_t& position);

void consumeJsonArray(const std::string& input, size_t& position) {
    if (position >= input.size() || input[position] != '[') {
        throw std::runtime_error("Expected JSON array");
    }
    ++position;
    skipWhitespace(input, position);
    if (position < input.size() && input[position] == ']') {
        ++position;
        return;
    }
    while (true) {
        consumeJsonValue(input, position);
        skipWhitespace(input, position);
        if (position >= input.size()) throw std::runtime_error("Unterminated JSON array");
        if (input[position] == ']') {
            ++position;
            return;
        }
        if (input[position] != ',') throw std::runtime_error("Expected ',' or ']' in JSON array");
        ++position;
        skipWhitespace(input, position);
        if (position >= input.size() || input[position] == ']') {
            throw std::runtime_error("Trailing comma in JSON array");
        }
    }
}

JsonValue parseJsonObject(const std::string& input, size_t& position) {
    if (position >= input.size() || input[position] != '{') {
        throw std::runtime_error("Expected JSON object");
    }
    ++position;
    JsonValue json;
    skipWhitespace(input, position);
    if (position < input.size() && input[position] == '}') {
        ++position;
        return json;
    }

    while (true) {
        skipWhitespace(input, position);
        const std::string key = parseJsonStringLiteral(input, position);
        if (json.find(key) != json.end()) throw std::runtime_error("Duplicate JSON object key");
        skipWhitespace(input, position);
        if (position >= input.size() || input[position] != ':') {
            throw std::runtime_error("Expected ':' after JSON object key");
        }
        ++position;
        skipWhitespace(input, position);
        if (position >= input.size()) throw std::runtime_error("Missing JSON value");

        if (input[position] == '{') {
            json[key] = parseJsonObject(input, position);
        } else if (input[position] == '[') {
            const size_t arrayStart = position;
            consumeJsonArray(input, position);
            json[key] = input.substr(arrayStart, position - arrayStart);
        } else if (input[position] == '"') {
            json[key] = parseJsonStringLiteral(input, position);
        } else {
            const size_t scalarStart = position;
            while (position < input.size() && input[position] != ',' &&
                   input[position] != '}' &&
                   !std::isspace(static_cast<unsigned char>(input[position]))) {
                ++position;
            }
            const std::string scalar = input.substr(scalarStart, position - scalarStart);
            if (scalar != "true" && scalar != "false" && scalar != "null" &&
                !isJsonNumber(scalar)) {
                throw std::runtime_error("Invalid JSON scalar");
            }
            json[key] = scalar;
        }

        skipWhitespace(input, position);
        if (position >= input.size()) throw std::runtime_error("Unterminated JSON object");
        if (input[position] == '}') {
            ++position;
            return json;
        }
        if (input[position] != ',') throw std::runtime_error("Expected ',' or '}' in JSON object");
        ++position;
        skipWhitespace(input, position);
        if (position >= input.size() || input[position] == '}') {
            throw std::runtime_error("Trailing comma in JSON object");
        }
    }
}

void consumeJsonValue(const std::string& input, size_t& position) {
    skipWhitespace(input, position);
    if (position >= input.size()) throw std::runtime_error("Missing JSON value");
    if (input[position] == '{') {
        static_cast<void>(parseJsonObject(input, position));
    } else if (input[position] == '[') {
        consumeJsonArray(input, position);
    } else if (input[position] == '"') {
        static_cast<void>(parseJsonStringLiteral(input, position));
    } else {
        const size_t scalarStart = position;
        while (position < input.size() && input[position] != ',' &&
               input[position] != ']' && input[position] != '}' &&
               !std::isspace(static_cast<unsigned char>(input[position]))) {
            ++position;
        }
        const std::string scalar = input.substr(scalarStart, position - scalarStart);
        if (scalar != "true" && scalar != "false" && scalar != "null" &&
            !isJsonNumber(scalar)) {
            throw std::runtime_error("Invalid JSON scalar");
        }
    }
}

JsonValue parseJsonObjectFlat(const std::string& jsonString) {
    size_t position = 0;
    skipWhitespace(jsonString, position);
    JsonValue json = parseJsonObject(jsonString, position);
    skipWhitespace(jsonString, position);
    if (position != jsonString.size()) throw std::runtime_error("Trailing content after JSON object");
    return json;
}

} // namespace

// Local helper to parse version strings without depending on plugin_specification
static PluginVersion parseVersionString(const std::string& versionStr) {
    PluginVersion version;
    std::istringstream iss(versionStr);
    std::string part;

    if (std::getline(iss, part, '.')) {
        try { version.major = std::stoi(part); } catch (...) {}
    }
    if (std::getline(iss, part, '.')) {
        try { version.minor = std::stoi(part); } catch (...) {}
    }
    if (std::getline(iss, part)) {
        // Handle potential prerelease suffix
        size_t dashPos = part.find('-');
        if (dashPos != std::string::npos) {
            try { version.patch = std::stoi(part.substr(0, dashPos)); } catch (...) {}
            version.prerelease = part.substr(dashPos + 1);
        } else {
            try { version.patch = std::stoi(part); } catch (...) {}
        }
    }

    return version;
}

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
            {
                std::lock_guard<std::mutex> lock(mutex_);
                ++filesError_;
            }
            return std::nullopt;
        }
        
        auto result = loadFromJson(content);
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (result) {
                ++filesLoaded_;
            } else {
                ++filesError_;
            }
        }
        if (result) {
            logger_->log("Successfully loaded character: " + result->name, "characterfile", "loader", LogLevel::SUCCESS);
        }
        
        return result;
    } catch (const std::exception& e) {
        logger_->log("Exception loading character file: " + std::string(e.what()), "characterfile", "loader", LogLevel::ERROR);
        {
            std::lock_guard<std::mutex> lock(mutex_);
            ++filesError_;
        }
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
        bool strictValidation = true;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            strictValidation = strictValidation_;
        }
        if (!validation.isValid && strictValidation) {
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
            {
                std::lock_guard<std::mutex> lock(mutex_);
                ++filesSaved_;
            }
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
    // Convert to formatted JSON with proper string escaping so exported profiles
    // can be parsed back into the same public identity fields.
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"name\": \"" << jsonEscape(character.name) << "\",\n";
    oss << "  \"description\": \"" << jsonEscape(character.description) << "\",\n";
    oss << "  \"id\": \"" << jsonEscape(character.id) << "\",\n";
    oss << "  \"version\": \"" << jsonEscape(character.version) << "\",\n";
    oss << "  \"creator\": \"" << jsonEscape(character.creator) << "\",\n";
    oss << "  \"created_at\": \"" << std::to_string(std::chrono::system_clock::to_time_t(character.created_at)) << "\",\n";
    oss << "  \"updated_at\": \"" << std::to_string(std::chrono::system_clock::to_time_t(character.updated_at)) << "\",\n";
    oss << "  \"personality\": {\n";
    oss << "    \"openness\": " << character.personality.openness << ",\n";
    oss << "    \"conscientiousness\": " << character.personality.conscientiousness << ",\n";
    oss << "    \"curiosity\": " << character.personality.curiosity << ",\n";
    oss << "    \"empathy\": " << character.personality.empathy << "\n";
    oss << "  },\n";
    oss << "  \"communicationStyle\": {\n";
    oss << "    \"tone\": \"" << jsonEscape(character.communicationStyle.tone) << "\",\n";
    oss << "    \"vocabulary\": \"" << jsonEscape(character.communicationStyle.vocabulary) << "\",\n";
    oss << "    \"verbosity\": " << character.communicationStyle.verbosity << ",\n";
    oss << "    \"formality\": " << character.communicationStyle.formality << ",\n";
    oss << "    \"emotionality\": " << character.communicationStyle.emotionality << "\n";
    oss << "  }\n";
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
    if (!validateRequiredFields(json)) {
        result.addError("Required fields missing or empty");
    }
    if (!validateFieldTypes(json)) {
        result.addError("Invalid field types");
    }

    JsonValue schema;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        schema = validationSchema_;
    }
    if (!schema.empty() && !validateSchema(json, schema)) {
        result.addError("Custom schema validation failed");
    }
    result.isValid = result.errors.empty();
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
        metadata.version = parseVersionString(getString(json, "version", "1.0.0"));

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
    std::lock_guard<std::mutex> lock(mutex_);
    stats["filesLoaded"] = std::string(std::to_string(filesLoaded_));
    stats["filesError"] = std::string(std::to_string(filesError_));
    stats["filesSaved"] = std::string(std::to_string(filesSaved_));
    stats["successRate"] = std::string(std::to_string(
        filesLoaded_ + filesError_ > 0 ?
        static_cast<double>(filesLoaded_) / (filesLoaded_ + filesError_) : 0.0));
    return stats;
}

void CharacterFileLoader::setStrictValidation(bool enabled) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        strictValidation_ = enabled;
    }
    logger_->log("Strict validation " + std::string(enabled ? "enabled" : "disabled"), "characterfile", "loader", LogLevel::INFO);
}

void CharacterFileLoader::setValidationSchema(const JsonValue& schema) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        validationSchema_ = schema;
    }
    logger_->log("Custom validation schema set", "characterfile", "loader", LogLevel::INFO);
}

// Private helper methods
JsonValue CharacterFileLoader::parseJsonString(const std::string& jsonString) {
    JsonValue json = parseJsonObjectFlat(jsonString);
    if (json.empty()) {
        throw std::runtime_error("JSON object did not contain any fields");
    }
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
    for (const auto& expected : schema) {
        const auto actual = json.find(expected.first);
        if (actual == json.end() || actual->second.type() != expected.second.type()) {
            return false;
        }
    }
    return true;
}

bool CharacterFileLoader::validateRequiredFields(const JsonValue& json) {
    for (const std::string& field : {std::string("name"), std::string("description")}) {
        if (json.find(field) == json.end()) return false;
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

    if (json.find("background") != json.end()) {
        try {
            JsonValue backgroundJson = std::any_cast<JsonValue>(json.at("background"));
            character.background = parseBackground(backgroundJson);
        } catch (const std::bad_any_cast&) {
            // Use default background
        }
    }

    if (json.find("communicationStyle") != json.end()) {
        try {
            JsonValue communicationJson = std::any_cast<JsonValue>(json.at("communicationStyle"));
            character.communicationStyle = parseCommunicationStyle(communicationJson);
        } catch (const std::bad_any_cast&) {
            // Use default communication style
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
    
    if (traitsJson.empty()) {
        return traits;
    }

    CharacterTrait trait(
        getString(traitsJson, "name", "imported-trait"),
        getString(traitsJson, "description", "Trait imported from character file"),
        TraitCategory::PERSONALITY,
        TraitValueType::NUMERIC);
    trait.setNumericValue(getFloat(traitsJson, "value", 0.5f));
    traits.push_back(trait);
    return traits;
}

JsonValue CharacterFileLoader::traitsToJson(const std::vector<CharacterTrait>& traits) {
    JsonValue json;
    
    json["count"] = std::string(std::to_string(traits.size()));
    if (!traits.empty()) {
        json["name"] = std::string(traits.front().name);
        json["description"] = std::string(traits.front().description);
        json["value"] = std::string(std::to_string(traits.front().getNumericValue()));
    }
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
    style.vocabulary = getString(commJson, "vocabulary", "standard");
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

namespace detail {

struct CharacterFileManagerState {
    struct Fingerprint {
        std::uintmax_t size = 0;
        std::uint64_t contentHash = 0;
        std::filesystem::file_time_type lastWrite{};

        bool operator==(const Fingerprint& other) const {
            return size == other.size && contentHash == other.contentHash &&
                   lastWrite == other.lastWrite;
        }

        bool operator!=(const Fingerprint& other) const {
            return !(*this == other);
        }
    };

    struct PendingChange {
        CharacterFileChangeType type = CharacterFileChangeType::Modified;
        std::optional<Fingerprint> fingerprint;
        std::chrono::steady_clock::time_point lastObserved;
    };

    std::shared_ptr<CharacterFileLoader> loader = std::make_shared<CharacterFileLoader>();
    std::shared_ptr<CharacterManager> characterManager;
    std::shared_ptr<AgentLogger> logger = std::make_shared<AgentLogger>();
    mutable std::mutex mutex;
    std::condition_variable condition;
    std::thread worker;
    bool watching = false;
    bool stopRequested = false;
    bool autoImport = true;
    std::string watchedDirectory;
    std::chrono::milliseconds interval{100};
    CharacterFileManager::ChangeCallback callback;
    std::unordered_map<std::string, Fingerprint> fingerprints;
    std::unordered_map<std::string, PendingChange> pendingChanges;
    std::unordered_map<std::string, std::string> importedIds;
    size_t importedCount = 0;
    size_t exportedCount = 0;
    size_t errorCount = 0;
};

} // namespace detail

namespace {

using CharacterFileFingerprint = detail::CharacterFileManagerState::Fingerprint;
using CharacterFileSnapshot = std::unordered_map<std::string, CharacterFileFingerprint>;

std::uint64_t hashFileContents(const std::filesystem::path& path, std::error_code& error) {
    std::ifstream input(path, std::ios::binary);
    if (!input) {
        error = std::make_error_code(std::errc::io_error);
        return 0;
    }

    std::uint64_t hash = 1469598103934665603ULL;
    char buffer[4096];
    while (input) {
        input.read(buffer, static_cast<std::streamsize>(sizeof(buffer)));
        const std::streamsize count = input.gcount();
        for (std::streamsize i = 0; i < count; ++i) {
            hash ^= static_cast<unsigned char>(buffer[i]);
            hash *= 1099511628211ULL;
        }
    }
    if (!input.eof()) {
        error = std::make_error_code(std::errc::io_error);
        return 0;
    }
    error.clear();
    return hash;
}

bool hasCharacterExtension(const std::filesystem::path& path) {
    std::string extension = path.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(),
        [](unsigned char character) { return static_cast<char>(std::tolower(character)); });
    return extension == ".json" || extension == ".character" || extension == ".eliza";
}

CharacterFileSnapshot takeCharacterFileSnapshot(const std::filesystem::path& directory,
                                                size_t& scanErrors) {
    CharacterFileSnapshot snapshot;
    scanErrors = 0;
    std::error_code iteratorError;
    std::filesystem::directory_iterator iterator(
        directory, std::filesystem::directory_options::skip_permission_denied, iteratorError);
    if (iteratorError) {
        ++scanErrors;
        return snapshot;
    }

    const std::filesystem::directory_iterator end;
    while (iterator != end) {
        const auto entry = *iterator;
        std::error_code statusError;
        const bool regular = entry.is_regular_file(statusError);
        if (statusError) {
            ++scanErrors;
        } else if (regular && hasCharacterExtension(entry.path())) {
            std::error_code metadataError;
            CharacterFileFingerprint fingerprint;
            fingerprint.size = entry.file_size(metadataError);
            if (!metadataError) {
                fingerprint.lastWrite = entry.last_write_time(metadataError);
            }
            if (!metadataError) {
                fingerprint.contentHash = hashFileContents(entry.path(), metadataError);
            }
            if (metadataError) {
                ++scanErrors;
            } else {
                snapshot[entry.path().lexically_normal().string()] = fingerprint;
            }
        }

        iterator.increment(iteratorError);
        if (iteratorError) {
            ++scanErrors;
            iteratorError.clear();
        }
    }
    return snapshot;
}

void logCharacterFileMessage(const std::shared_ptr<AgentLogger>& logger,
                             const std::string& message,
                             LogLevel level) noexcept {
    if (!logger) return;
    try {
        logger->log(message, "characterfile", "manager", level);
    } catch (...) {
        // Logging must never terminate the watcher.
    }
}

void autoImportCharacterFile(const std::shared_ptr<detail::CharacterFileManagerState>& state,
                             const CharacterFileChangeEvent& event) {
    std::shared_ptr<CharacterManager> manager;
    bool autoImport = false;
    {
        std::lock_guard<std::mutex> lock(state->mutex);
        autoImport = state->autoImport;
        manager = state->characterManager;
    }
    if (!autoImport || !manager) return;

    if (event.type == CharacterFileChangeType::Deleted) {
        std::string characterId;
        {
            std::lock_guard<std::mutex> lock(state->mutex);
            const auto found = state->importedIds.find(event.path);
            if (found != state->importedIds.end()) {
                characterId = found->second;
                state->importedIds.erase(found);
            }
        }
        if (!characterId.empty()) {
            try {
                if (!manager->unregisterCharacter(characterId)) {
                    std::lock_guard<std::mutex> lock(state->mutex);
                    ++state->errorCount;
                }
            } catch (...) {
                std::lock_guard<std::mutex> lock(state->mutex);
                ++state->errorCount;
            }
        }
        return;
    }

    const auto character = state->loader->loadFromFile(event.path);
    if (!character) {
        std::lock_guard<std::mutex> lock(state->mutex);
        ++state->errorCount;
        return;
    }

    std::string previousId;
    {
        std::lock_guard<std::mutex> lock(state->mutex);
        const auto previous = state->importedIds.find(event.path);
        if (previous != state->importedIds.end()) previousId = previous->second;
    }

    try {
        const std::string id = manager->registerCharacter(*character);
        if (id.empty()) {
            std::lock_guard<std::mutex> lock(state->mutex);
            ++state->errorCount;
            return;
        }
        bool removalFailed = false;
        if (!previousId.empty() && previousId != id) {
            removalFailed = !manager->unregisterCharacter(previousId);
        }
        std::lock_guard<std::mutex> lock(state->mutex);
        ++state->importedCount;
        if (removalFailed) ++state->errorCount;
        state->importedIds[event.path] = id;
    } catch (...) {
        std::lock_guard<std::mutex> lock(state->mutex);
        ++state->errorCount;
    }
}

void dispatchCharacterFileEvent(const std::shared_ptr<detail::CharacterFileManagerState>& state,
                                const CharacterFileChangeEvent& event) {
    autoImportCharacterFile(state, event);

    CharacterFileManager::ChangeCallback callback;
    {
        std::lock_guard<std::mutex> lock(state->mutex);
        if (state->stopRequested) return;
        callback = state->callback;
    }
    if (!callback) return;

    try {
        callback(event);
    } catch (const std::exception& exception) {
        logCharacterFileMessage(state->logger,
            "Character file change callback threw: " + std::string(exception.what()),
            LogLevel::ERROR);
    } catch (...) {
        logCharacterFileMessage(state->logger,
            "Character file change callback threw an unknown exception",
            LogLevel::ERROR);
    }
}

void runCharacterFileWatcher(const std::shared_ptr<detail::CharacterFileManagerState>& state) {
    while (true) {
        std::chrono::milliseconds interval{100};
        {
            std::unique_lock<std::mutex> lock(state->mutex);
            interval = state->interval;
            if (state->condition.wait_for(lock, interval,
                    [&state] { return state->stopRequested; })) {
                break;
            }
        }

        size_t scanErrors = 0;
        CharacterFileSnapshot current = takeCharacterFileSnapshot(state->watchedDirectory, scanErrors);
        std::vector<CharacterFileChangeEvent> events;
        {
            std::lock_guard<std::mutex> lock(state->mutex);
            if (state->stopRequested) break;
            state->errorCount += scanErrors;

            const auto now = std::chrono::steady_clock::now();
            for (const auto& entry : current) {
                const auto previous = state->fingerprints.find(entry.first);
                if (previous == state->fingerprints.end() || previous->second != entry.second) {
                    const auto pending = state->pendingChanges.find(entry.first);
                    const CharacterFileChangeType detectedType =
                        previous == state->fingerprints.end()
                            ? CharacterFileChangeType::Created
                            : CharacterFileChangeType::Modified;
                    CharacterFileChangeType pendingType = detectedType;
                    if (pending != state->pendingChanges.end() &&
                        pending->second.type == CharacterFileChangeType::Created) {
                        pendingType = CharacterFileChangeType::Created;
                    }
                    state->pendingChanges[entry.first] =
                        {pendingType, entry.second, now};
                }
            }
            for (const auto& previous : state->fingerprints) {
                if (current.find(previous.first) == current.end()) {
                    const auto pending = state->pendingChanges.find(previous.first);
                    if (pending != state->pendingChanges.end() &&
                        pending->second.type == CharacterFileChangeType::Created) {
                        state->pendingChanges.erase(pending);
                    } else {
                        state->pendingChanges[previous.first] =
                            {CharacterFileChangeType::Deleted, std::nullopt, now};
                    }
                }
            }
            state->fingerprints = current;

            for (auto pending = state->pendingChanges.begin();
                 pending != state->pendingChanges.end();) {
                bool stable = false;
                if (pending->second.type == CharacterFileChangeType::Deleted) {
                    stable = current.find(pending->first) == current.end();
                } else {
                    const auto latest = current.find(pending->first);
                    stable = latest != current.end() && pending->second.fingerprint &&
                             latest->second == *pending->second.fingerprint;
                }
                if (stable && now - pending->second.lastObserved >= interval) {
                    events.push_back({pending->second.type, pending->first});
                    pending = state->pendingChanges.erase(pending);
                } else {
                    ++pending;
                }
            }
        }

        std::sort(events.begin(), events.end(), [](const CharacterFileChangeEvent& lhs,
                                                   const CharacterFileChangeEvent& rhs) {
            if (lhs.path != rhs.path) return lhs.path < rhs.path;
            return static_cast<int>(lhs.type) < static_cast<int>(rhs.type);
        });
        for (const auto& event : events) {
            {
                std::lock_guard<std::mutex> lock(state->mutex);
                if (state->stopRequested) break;
            }
            dispatchCharacterFileEvent(state, event);
        }
    }

    std::lock_guard<std::mutex> lock(state->mutex);
    state->watching = false;
}

} // namespace

CharacterFileManager::CharacterFileManager()
    : state_(std::make_shared<detail::CharacterFileManagerState>()) {
    logCharacterFileMessage(state_->logger, "Character file manager initialized", LogLevel::INFO);
}

CharacterFileManager::~CharacterFileManager() {
    stopWatching();
}

void CharacterFileManager::setCharacterManager(std::shared_ptr<CharacterManager> manager) {
    auto state = state_;
    {
        std::lock_guard<std::mutex> lock(state->mutex);
        state->characterManager = std::move(manager);
    }
    logCharacterFileMessage(state->logger, "Character manager set for file operations", LogLevel::INFO);
}

int CharacterFileManager::importFromDirectory(const std::string& directory, bool recursive) {
    auto state = state_;
    std::shared_ptr<CharacterManager> manager;
    {
        std::lock_guard<std::mutex> lock(state->mutex);
        manager = state->characterManager;
    }
    if (!manager) {
        logCharacterFileMessage(state->logger, "No character manager set for import", LogLevel::ERROR);
        return 0;
    }

    logCharacterFileMessage(state->logger, "Importing characters from directory: " + directory, LogLevel::INFO);
    const auto characters = state->loader->loadFromDirectory(directory, recursive);
    int imported = 0;
    size_t errors = 0;
    for (const auto& character : characters) {
        try {
            const std::string id = manager->registerCharacter(character);
            if (!id.empty()) {
                ++imported;
            } else {
                ++errors;
            }
        } catch (...) {
            ++errors;
        }
    }
    {
        std::lock_guard<std::mutex> lock(state->mutex);
        state->importedCount += static_cast<size_t>(imported);
        state->errorCount += errors;
    }
    logCharacterFileMessage(state->logger,
        "Imported " + std::to_string(imported) + " characters from " + directory,
        LogLevel::SUCCESS);
    return imported;
}

int CharacterFileManager::exportToDirectory(const std::string& directory) {
    auto state = state_;
    std::shared_ptr<CharacterManager> manager;
    {
        std::lock_guard<std::mutex> lock(state->mutex);
        manager = state->characterManager;
    }
    if (!manager) {
        logCharacterFileMessage(state->logger, "No character manager set for export", LogLevel::ERROR);
        return 0;
    }

    logCharacterFileMessage(state->logger, "Exporting characters to directory: " + directory, LogLevel::INFO);
    std::error_code directoryError;
    std::filesystem::create_directories(directory, directoryError);
    if (directoryError) {
        std::lock_guard<std::mutex> lock(state->mutex);
        ++state->errorCount;
        return 0;
    }

    const auto characters = manager->getAllCharacters();
    int exported = 0;
    size_t errors = 0;
    for (const auto& character : characters) {
        const std::filesystem::path filename =
            std::filesystem::path(directory) / CharacterFileUtils::createFilename(character.name);
        if (state->loader->saveToFile(character, filename.string())) {
            ++exported;
        } else {
            ++errors;
        }
    }
    {
        std::lock_guard<std::mutex> lock(state->mutex);
        state->exportedCount += static_cast<size_t>(exported);
        state->errorCount += errors;
    }
    logCharacterFileMessage(state->logger,
        "Exported " + std::to_string(exported) + " characters to " + directory,
        LogLevel::SUCCESS);
    return exported;
}

bool CharacterFileManager::syncWithManager(const std::string& directory) {
    std::shared_ptr<CharacterManager> manager;
    {
        std::lock_guard<std::mutex> lock(state_->mutex);
        manager = state_->characterManager;
    }
    if (!manager) return false;

    logCharacterFileMessage(state_->logger,
        "Syncing character files with manager from: " + directory, LogLevel::INFO);
    const int imported = importFromDirectory(directory, false);
    const int exported = exportToDirectory(directory);
    logCharacterFileMessage(state_->logger,
        "Sync complete: " + std::to_string(imported) + " imported, " +
            std::to_string(exported) + " exported",
        LogLevel::SUCCESS);
    return true;
}

bool CharacterFileManager::watchDirectory(const std::string& directory, bool autoImport) {
    if (directory.empty()) return false;

    std::error_code pathError;
    std::filesystem::path path(directory);
    if (!std::filesystem::exists(path, pathError) || pathError ||
        !std::filesystem::is_directory(path, pathError) || pathError) {
        return false;
    }
    const std::filesystem::path canonical = std::filesystem::weakly_canonical(path, pathError);
    if (pathError) return false;

    stopWatching();

    auto state = state_;
    size_t scanErrors = 0;
    CharacterFileSnapshot baseline = takeCharacterFileSnapshot(canonical, scanErrors);
    {
        std::lock_guard<std::mutex> lock(state->mutex);
        if (state->interval.count() <= 0) return false;
        state->watchedDirectory = canonical.string();
        state->autoImport = autoImport;
        state->fingerprints = std::move(baseline);
        state->pendingChanges.clear();
        state->errorCount += scanErrors;
        state->stopRequested = false;
        state->watching = true;
    }

    try {
        std::lock_guard<std::mutex> lock(state->mutex);
        state->worker = std::thread([state] {
            try {
                runCharacterFileWatcher(state);
            } catch (const std::exception& exception) {
                logCharacterFileMessage(state->logger,
                    "Character file watcher failed: " + std::string(exception.what()),
                    LogLevel::ERROR);
                std::lock_guard<std::mutex> stateLock(state->mutex);
                state->watching = false;
                ++state->errorCount;
            } catch (...) {
                logCharacterFileMessage(state->logger,
                    "Character file watcher failed with an unknown exception",
                    LogLevel::ERROR);
                std::lock_guard<std::mutex> stateLock(state->mutex);
                state->watching = false;
                ++state->errorCount;
            }
        });
    } catch (...) {
        std::lock_guard<std::mutex> lock(state->mutex);
        state->watching = false;
        state->stopRequested = true;
        state->watchedDirectory.clear();
        state->fingerprints.clear();
        state->pendingChanges.clear();
        ++state->errorCount;
        return false;
    }

    logCharacterFileMessage(state->logger,
        "Started watching directory: " + canonical.string() +
            " (auto-import: " + (autoImport ? "enabled" : "disabled") + ")",
        LogLevel::INFO);
    return true;
}

bool CharacterFileManager::setWatchInterval(std::chrono::milliseconds interval) {
    if (interval.count() <= 0) return false;
    auto state = state_;
    {
        std::lock_guard<std::mutex> lock(state->mutex);
        state->interval = interval;
    }
    state->condition.notify_all();
    return true;
}

void CharacterFileManager::setChangeCallback(ChangeCallback callback) {
    std::lock_guard<std::mutex> lock(state_->mutex);
    state_->callback = std::move(callback);
}

void CharacterFileManager::stopWatching() {
    auto state = state_;
    std::thread worker;
    std::string directory;
    {
        std::lock_guard<std::mutex> lock(state->mutex);
        state->stopRequested = true;
        state->watching = false;
        directory = state->watchedDirectory;
        if (state->worker.joinable()) {
            if (state->worker.get_id() == std::this_thread::get_id()) {
                state->condition.notify_all();
                return;
            }
            worker = std::move(state->worker);
        } else {
            state->watchedDirectory.clear();
            state->fingerprints.clear();
            state->pendingChanges.clear();
        }
    }
    state->condition.notify_all();
    if (worker.joinable()) worker.join();
    {
        std::lock_guard<std::mutex> lock(state->mutex);
        state->watchedDirectory.clear();
        state->fingerprints.clear();
        state->pendingChanges.clear();
    }
    if (!directory.empty()) {
        logCharacterFileMessage(state->logger,
            "Stopped watching directory: " + directory, LogLevel::INFO);
    }
}

JsonValue CharacterFileManager::getOperationStatistics() const {
    JsonValue stats;
    std::lock_guard<std::mutex> lock(state_->mutex);
    stats["importedCount"] = std::string(std::to_string(state_->importedCount));
    stats["exportedCount"] = std::string(std::to_string(state_->exportedCount));
    stats["errorCount"] = std::string(std::to_string(state_->errorCount));
    stats["isWatching"] = std::string(state_->watching ? "true" : "false");
    stats["watchedDirectory"] = state_->watchedDirectory;
    return stats;
}

std::vector<ValidationResult> CharacterFileManager::validateDirectory(const std::string& directory) {
    std::vector<ValidationResult> results;
    logCharacterFileMessage(state_->logger,
        "Validating character files in directory: " + directory, LogLevel::INFO);
    const auto files = findCharacterFiles(directory, false);
    for (const auto& file : files) {
        ValidationResult result = state_->loader->validateFile(file);
        result.addWarning("File: " + file);
        results.push_back(std::move(result));
    }
    logCharacterFileMessage(state_->logger,
        "Validated " + std::to_string(results.size()) + " character files", LogLevel::INFO);
    return results;
}

bool CharacterFileManager::convertFormat(const std::string& inputFile,
                                         const std::string& outputFile,
                                         const std::string& targetFormat) {
    if (targetFormat != "json" && targetFormat != ".json" &&
        targetFormat != "character" && targetFormat != ".character" &&
        targetFormat != "eliza" && targetFormat != ".eliza") {
        return false;
    }
    logCharacterFileMessage(state_->logger,
        "Converting file format: " + inputFile + " -> " + outputFile +
            " (format: " + targetFormat + ")",
        LogLevel::INFO);
    const auto character = state_->loader->loadFromFile(inputFile);
    return character && state_->loader->saveToFile(*character, outputFile);
}

bool CharacterFileManager::backupDirectory(const std::string& sourceDir,
                                           const std::string& backupDir) {
    if (sourceDir.empty() || backupDir.empty()) return false;
    std::error_code error;
    if (!std::filesystem::is_directory(sourceDir, error) || error) return false;
    std::filesystem::create_directories(backupDir, error);
    if (error) return false;
    std::filesystem::copy(sourceDir, backupDir,
        std::filesystem::copy_options::recursive |
            std::filesystem::copy_options::overwrite_existing,
        error);
    if (error) {
        logCharacterFileMessage(state_->logger,
            "Backup failed: " + error.message(), LogLevel::ERROR);
        return false;
    }
    logCharacterFileMessage(state_->logger, "Backup completed successfully", LogLevel::SUCCESS);
    return true;
}

std::vector<std::string> CharacterFileManager::findCharacterFiles(const std::string& directory,
                                                                  bool recursive) {
    std::vector<std::string> files;
    std::error_code error;
    if (!std::filesystem::is_directory(directory, error) || error) return files;

    const auto visit = [&files](const auto& entry) {
        std::error_code statusError;
        if (entry.is_regular_file(statusError) && !statusError &&
            hasCharacterExtension(entry.path())) {
            files.push_back(entry.path().lexically_normal().string());
        }
    };
    if (recursive) {
        std::filesystem::recursive_directory_iterator iterator(
            directory, std::filesystem::directory_options::skip_permission_denied, error);
        const std::filesystem::recursive_directory_iterator endIterator;
        while (!error && iterator != endIterator) {
            visit(*iterator);
            iterator.increment(error);
        }
    } else {
        std::filesystem::directory_iterator iterator(
            directory, std::filesystem::directory_options::skip_permission_denied, error);
        const std::filesystem::directory_iterator endIterator;
        while (!error && iterator != endIterator) {
            visit(*iterator);
            iterator.increment(error);
        }
    }
    std::sort(files.begin(), files.end());
    return files;
}

bool CharacterFileManager::isValidCharacterFile(const std::string& filename) {
    return state_->loader->isCharacterFile(filename);
}

std::string CharacterFileManager::getCharacterIdFromFile(const std::string& filename) {
    const auto character = state_->loader->loadFromFile(filename);
    return character ? character->id : std::string{};
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
    
    // Convert template to JSON std::string
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
    static std::atomic<std::uint64_t> sequence{0};
    const auto now = std::chrono::system_clock::now().time_since_epoch();
    const auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();

    std::ostringstream oss;
    oss << "char-" << nanos << "-"
        << sequence.fetch_add(1, std::memory_order_relaxed);
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
    const auto now = std::chrono::system_clock::now();
    const std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm localTime{};
#ifdef _WIN32
    if (localtime_s(&localTime, &time) != 0) return {};
#else
    if (localtime_r(&time, &localTime) == nullptr) return {};
#endif

    std::ostringstream oss;
    oss << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::chrono::system_clock::time_point parseTimestamp(const std::string& timestamp) {
    std::tm parsed{};
    parsed.tm_isdst = -1;
    std::istringstream input(timestamp);
    input >> std::get_time(&parsed, "%Y-%m-%d %H:%M:%S");
    if (input.fail()) return std::chrono::system_clock::now();

    const std::time_t time = std::mktime(&parsed);
    if (time == static_cast<std::time_t>(-1)) {
        return std::chrono::system_clock::now();
    }
    return std::chrono::system_clock::from_time_t(time);
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
