#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-VISION_SRC_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-VISION_SRC_TYPES_H
#include "core.h"

namespace @elizaos/core {
    class ServiceTypeRegistry;
}
class CameraInfo;
class SceneDescription;
class DetectedObject;
class PersonInfo;
class BoundingBox;
class VisionFrame;
enum struct VisionMode;
class ScreenCapture;
class ScreenTile;
class TileAnalysis;
class Florence2Result;
class OCRResult;
class EnhancedSceneDescription;
class VisionConfig;
class TrackedEntity;
class EntityAppearance;
class EntityAttributes;
class FaceLibrary;
class FaceProfile;
class WorldState;

namespace std::string("@elizaos/core") {
    class ServiceTypeRegistry : public object, public std::enable_shared_from_this<ServiceTypeRegistry> {
    public:
        using std::enable_shared_from_this<ServiceTypeRegistry>::shared_from_this;
        string VISION;
    };

}
extern object VisionServiceType, satisfies, Partial;
class CameraInfo : public object, public std::enable_shared_from_this<CameraInfo> {
public:
    using std::enable_shared_from_this<CameraInfo>::shared_from_this;
    string id;

    string name;

    boolean connected;
};

class SceneDescription : public object, public std::enable_shared_from_this<SceneDescription> {
public:
    using std::enable_shared_from_this<SceneDescription>::shared_from_this;
    double timestamp;

    string description;

    array<std::shared_ptr<DetectedObject>> objects;

    array<std::shared_ptr<PersonInfo>> people;

    boolean sceneChanged;

    double changePercentage;

    string audioTranscription;
};

class DetectedObject : public object, public std::enable_shared_from_this<DetectedObject> {
public:
    using std::enable_shared_from_this<DetectedObject>::shared_from_this;
    string id;

    string type;

    double confidence;

    std::shared_ptr<BoundingBox> boundingBox;
};

class PersonInfo : public object, public std::enable_shared_from_this<PersonInfo> {
public:
    using std::enable_shared_from_this<PersonInfo>::shared_from_this;
    string id;

    any pose;

    any facing;

    double confidence;

    std::shared_ptr<BoundingBox> boundingBox;

    array<object> keypoints;
};

class BoundingBox : public object, public std::enable_shared_from_this<BoundingBox> {
public:
    using std::enable_shared_from_this<BoundingBox>::shared_from_this;
    double x;

    double y;

    double width;

    double height;
};

class VisionFrame : public object, public std::enable_shared_from_this<VisionFrame> {
public:
    using std::enable_shared_from_this<VisionFrame>::shared_from_this;
    double timestamp;

    double width;

    double height;

    std::shared_ptr<Buffer> data;

    any format;
};

enum struct VisionMode {
    OFF = std::string("OFF"), CAMERA = std::string("CAMERA"), SCREEN = std::string("SCREEN"), BOTH = std::string("BOTH")
};
class ScreenCapture : public object, public std::enable_shared_from_this<ScreenCapture> {
public:
    using std::enable_shared_from_this<ScreenCapture>::shared_from_this;
    double timestamp;

    double width;

    double height;

    std::shared_ptr<Buffer> data;

    array<std::shared_ptr<ScreenTile>> tiles;
};

class ScreenTile : public object, public std::enable_shared_from_this<ScreenTile> {
public:
    using std::enable_shared_from_this<ScreenTile>::shared_from_this;
    string id;

    double row;

    double col;

    double x;

    double y;

    double width;

    double height;

    std::shared_ptr<Buffer> data;

    std::shared_ptr<TileAnalysis> analysis;
};

class TileAnalysis : public object, public std::enable_shared_from_this<TileAnalysis> {
public:
    using std::enable_shared_from_this<TileAnalysis>::shared_from_this;
    double timestamp;

    std::shared_ptr<Florence2Result> florence2;

    std::shared_ptr<OCRResult> ocr;

    array<std::shared_ptr<DetectedObject>> objects;

    string text;

    string summary;
};

class Florence2Result : public object, public std::enable_shared_from_this<Florence2Result> {
public:
    using std::enable_shared_from_this<Florence2Result>::shared_from_this;
    string caption;

    array<object> objects;

    array<object> regions;

    array<string> tags;
};

class OCRResult : public object, public std::enable_shared_from_this<OCRResult> {
public:
    using std::enable_shared_from_this<OCRResult>::shared_from_this;
    string text;

    array<object> blocks;

    string fullText;
};

class EnhancedSceneDescription : public SceneDescription, public std::enable_shared_from_this<EnhancedSceneDescription> {
public:
    using std::enable_shared_from_this<EnhancedSceneDescription>::shared_from_this;
    std::shared_ptr<ScreenCapture> screenCapture;

    object screenAnalysis;
};

class VisionConfig : public object, public std::enable_shared_from_this<VisionConfig> {
public:
    using std::enable_shared_from_this<VisionConfig>::shared_from_this;
    string cameraName;

    double pixelChangeThreshold;

    double updateInterval;

    boolean enablePoseDetection;

    boolean enableObjectDetection;

    double tfUpdateInterval;

    double vlmUpdateInterval;

    double tfChangeThreshold;

    double vlmChangeThreshold;

    VisionMode visionMode;

    double screenCaptureInterval;

    double tileSize;

    any tileProcessingOrder;

    boolean ocrEnabled;

    boolean florence2Enabled;

    object screenRegion;

    double displayIndex;

    boolean captureAllDisplays;

    double targetScreenFPS;

    array<object> textRegions;
};

class TrackedEntity : public object, public std::enable_shared_from_this<TrackedEntity> {
public:
    using std::enable_shared_from_this<TrackedEntity>::shared_from_this;
    string id;

    any entityType;

    double firstSeen;

    double lastSeen;

    std::shared_ptr<BoundingBox> lastPosition;

    array<std::shared_ptr<EntityAppearance>> appearances;

    std::shared_ptr<EntityAttributes> attributes;

    string worldId;

    string roomId;
};

class EntityAppearance : public object, public std::enable_shared_from_this<EntityAppearance> {
public:
    using std::enable_shared_from_this<EntityAppearance>::shared_from_this;
    double timestamp;

    std::shared_ptr<BoundingBox> boundingBox;

    double confidence;

    array<double> embedding;

    array<object> keypoints;
};

class EntityAttributes : public object, public std::enable_shared_from_this<EntityAttributes> {
public:
    using std::enable_shared_from_this<EntityAttributes>::shared_from_this;
    string name;

    array<double> faceEmbedding;

    string faceId;

    array<string> clothing;

    string hairColor;

    array<string> accessories;

    string objectType;

    string color;

    any size;

    string description;

    array<string> tags;
};

class FaceLibrary : public object, public std::enable_shared_from_this<FaceLibrary> {
public:
    using std::enable_shared_from_this<FaceLibrary>::shared_from_this;
    std::shared_ptr<Map<string, std::shared_ptr<FaceProfile>>> faces;

    std::shared_ptr<Map<string, array<array<double>>>> embeddings;
};

class FaceProfile : public object, public std::enable_shared_from_this<FaceProfile> {
public:
    using std::enable_shared_from_this<FaceProfile>::shared_from_this;
    string id;

    string name;

    array<array<double>> embeddings;

    double firstSeen;

    double lastSeen;

    double seenCount;

    object attributes;
};

class WorldState : public object, public std::enable_shared_from_this<WorldState> {
public:
    using std::enable_shared_from_this<WorldState>::shared_from_this;
    string worldId;

    std::shared_ptr<Map<string, std::shared_ptr<TrackedEntity>>> entities;

    double lastUpdate;

    array<string> activeEntities;

    array<object> recentlyLeft;
};

namespace @elizaos/core {
}
#endif
