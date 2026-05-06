#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// Extend the core service types with vision service
  struct ServiceTypeRegistry {
    'VISION' VISION;
};

// Export service type constant

// Vision-specific types
struct CameraInfo {
    std::string id;
    std::string name;
    bool connected;
};

struct SceneDescription {
    double timestamp;
    std::string description;
    std::vector<DetectedObject> objects;
    std::vector<PersonInfo> people;
    bool sceneChanged;
    double changePercentage;
    std::optional<std::string> audioTranscription;
};

struct DetectedObject {
    std::string id;
    std::string type;
    double confidence;
    BoundingBox boundingBox;
};

struct PersonInfo {
    std::string id;
    std::variant<'sitting', 'standing', 'lying', 'unknown'> pose;
    std::variant<'camera', 'away', 'left', 'right', 'unknown'> facing;
    double confidence;
    BoundingBox boundingBox;
    std::string part;
    double score;
};

struct BoundingBox {
    double x;
    double y;
    double width;
    double height;
};

struct VisionFrame {
    double timestamp;
    double width;
    double height;
    Buffer data;
    std::variant<'rgb', 'rgba', 'jpeg', 'png'> format;
};

// Vision modes
enum VisionMode {
}

// Screen capture types
struct ScreenCapture {
    double timestamp;
    double width;
    double height;
    Buffer data;
    std::vector<ScreenTile> tiles;
};

struct ScreenTile {
    std::string id;
    double row;
    double col;
    double x;
    double y;
    double width;
    double height;
    std::optional<Buffer> data;
    std::optional<TileAnalysis> analysis;
};

struct TileAnalysis {
    double timestamp;
    std::optional<Florence2Result> florence2;
    std::optional<OCRResult> ocr;
    std::optional<std::vector<DetectedObject>> objects;
    std::optional<std::string> text;
    std::optional<std::string> summary;
};

struct Florence2Result {
    std::optional<std::string> caption;
    std::string label;
    BoundingBox bbox;
    double confidence;
    std::string description;
    BoundingBox bbox;
    std::optional<std::vector<std::string>> tags;
};

struct OCRResult {
    std::string text;
    std::string text;
    BoundingBox bbox;
    double confidence;
    std::string text;
    BoundingBox bbox;
    double confidence;
    std::string fullText;
};

// Enhanced scene description with screen data

// Update VisionConfig
struct VisionConfig {
    std::optional<std::string> cameraName;
    std::optional<double> pixelChangeThreshold;
    std::optional<double> updateInterval;
    std::optional<bool> enablePoseDetection;
    std::optional<bool> enableObjectDetection;
    std::optional<double> tfUpdateInterval;
    std::optional<double> vlmUpdateInterval;
    std::optional<double> tfChangeThreshold;
    std::optional<double> vlmChangeThreshold;
    std::optional<VisionMode> visionMode;
    std::optional<double> screenCaptureInterval;
    std::optional<double> tileSize;
    std::optional<std::variant<'sequential', 'priority', 'random'>> tileProcessingOrder;
    std::optional<bool> ocrEnabled;
    std::optional<bool> florence2Enabled;
    double x;
    double y;
    double width;
    double height;
    std::optional<double> displayIndex;
    std::optional<bool> captureAllDisplays;
    std::optional<double> targetScreenFPS;
    double x;
    double y;
    double width;
    double height;
};

// Entity tracking types
struct TrackedEntity {
    std::string id;
    std::variant<'person', 'object', 'pet'> entityType;
    double firstSeen;
    double lastSeen;
    BoundingBox lastPosition;
    std::vector<EntityAppearance> appearances;
    EntityAttributes attributes;
    std::optional<std::string> worldId;
    std::optional<std::string> roomId;
};

struct EntityAppearance {
    double timestamp;
    BoundingBox boundingBox;
    double confidence;
    std::optional<std::vector<double>> embedding;
    std::string part;
    double score;
};

struct EntityAttributes {
    std::optional<std::string> name;
    std::optional<std::vector<double>> faceEmbedding;
    std::optional<std::string> faceId;
    std::optional<std::vector<std::string>> clothing;
    std::optional<std::string> hairColor;
    std::optional<std::vector<std::string>> accessories;
    std::optional<std::string> objectType;
    std::optional<std::string> color;
    std::optional<std::variant<'small', 'medium', 'large'>> size;
    std::optional<std::string> description;
    std::optional<std::vector<std::string>> tags;
};

struct FaceLibrary {
};

struct FaceProfile {
    std::string id;
    std::optional<std::string> name;
    std::vector<std::vector<double>> embeddings;
    double firstSeen;
    double lastSeen;
    double seenCount;
    std::optional<std::string> age;
    std::optional<std::string> gender;
    std::optional<std::string> emotion;
};

struct WorldState {
    std::string worldId;
    double lastUpdate;
    std::vector<std::string> activeEntities;
    std::string entityId;
    double leftAt;
    BoundingBox lastPosition;
};


} // namespace elizaos
