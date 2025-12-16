#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

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
    std::optional<string; // Latest audio transcription> audioTranscription;
};

struct DetectedObject {
    std::string id;
    std::string type;
    double confidence;
    BoundingBox boundingBox;
};

struct PersonInfo {
    std::string id;
    'sitting' | 'standing' | 'lying' | 'unknown' pose;
    'camera' | 'away' | 'left' | 'right' | 'unknown' facing;
    double confidence;
    BoundingBox boundingBox;
    std::optional<Array<{> keypoints;
    std::string part;
    { x: number; y: number } position;
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
    'rgb' | 'rgba' | 'jpeg' | 'png' format;
};

// Vision modes
enum VisionMode {
  OFF = 'OFF',
  CAMERA = 'CAMERA',
  SCREEN = 'SCREEN',
  BOTH = 'BOTH',
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
    std::optional<Array<{> objects;
    std::string label;
    BoundingBox bbox;
    double confidence;
    std::optional<Array<{> regions;
    std::string description;
    BoundingBox bbox;
    std::optional<std::vector<std::string>> tags;
};

struct OCRResult {
    std::string text;
    Array<{ blocks;
    std::string text;
    BoundingBox bbox;
    double confidence;
    std::optional<Array<{> words;
    std::string text;
    BoundingBox bbox;
    double confidence;
    std::string fullText;
};

// Enhanced scene description with screen data
interface EnhancedSceneDescription extends SceneDescription {
  screenCapture?: ScreenCapture;
  screenAnalysis?: {
    fullScreenOCR?: string;
    activeTile?: TileAnalysis;
    gridSummary?: string;
    focusedApp?: string;
    uiElements?: Array<{
      type: string;
      text: string;
      position: BoundingBox;
    }>;
  };
}

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
    std::optional<number; // ms between screen captures> screenCaptureInterval;
    std::optional<number; // Size of tiles (e.g., 256 for 256x256)> tileSize;
    std::optional<'sequential' | 'priority' | 'random'> tileProcessingOrder;
    std::optional<bool> ocrEnabled;
    std::optional<bool> florence2Enabled;
    std::optional<{> screenRegion;
    double x;
    double y;
    double width;
    double height;
    std::optional<number; // Specific display to capture> displayIndex;
    std::optional<boolean; // Cycle through all displays> captureAllDisplays;
    std::optional<number; // Target FPS for screen capture> targetScreenFPS;
    std::optional<Array<{> textRegions;
    double x;
    double y;
    double width;
    double height;
};

// Entity tracking types
struct TrackedEntity {
    std::string id;
    'person' | 'object' | 'pet' entityType;
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
    std::optional<number[]; // Face embedding for person recognition> embedding;
    std::optional<Array<{> keypoints;
    std::string part;
    { x: number; y: number } position;
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
    std::optional<'small' | 'medium' | 'large'> size;
    std::optional<std::string> description;
    std::optional<std::vector<std::string>> tags;
};

struct FaceLibrary {
    std::unordered_map<std::string, FaceProfile> faces;
    std::unordered_map<std::string, std::vector<std::vector<double>>> embeddings;
};

struct FaceProfile {
    std::string id;
    std::optional<std::string> name;
    number[][]; // Multiple embeddings for better recognition embeddings;
    double firstSeen;
    double lastSeen;
    double seenCount;
    std::optional<{> attributes;
    std::optional<std::string> age;
    std::optional<std::string> gender;
    std::optional<std::string> emotion;
};

struct WorldState {
    std::string worldId;
    std::unordered_map<std::string, TrackedEntity> entities;
    double lastUpdate;
    string[]; // Currently visible activeEntities;
    Array<{ recentlyLeft;
    std::string entityId;
    double leftAt;
    BoundingBox lastPosition;
};


} // namespace elizaos
