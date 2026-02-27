#include "config.hpp"
#include <string>

ConfigurationManager::ConfigurationManager(any runtime) {
    this->runtime = runtime;
    this->config = this->loadConfiguration();
}

VisionConfigOutput ConfigurationManager::loadConfiguration()
{
    auto rawConfig = object{
        object::pair{std::string("cameraName"), OR((this->getSetting(std::string("CAMERA_NAME"))), (this->getSetting(std::string("VISION_CAMERA_NAME"))))}, 
        object::pair{std::string("enableCamera"), this->getBooleanSetting(std::string("ENABLE_CAMERA"), true)}, 
        object::pair{std::string("pixelChangeThreshold"), this->getNumberSetting(std::string("PIXEL_CHANGE_THRESHOLD"), 50)}, 
        object::pair{std::string("updateInterval"), this->getNumberSetting(std::string("UPDATE_INTERVAL"), 100)}, 
        object::pair{std::string("enableObjectDetection"), this->getBooleanSetting(std::string("ENABLE_OBJECT_DETECTION"), false)}, 
        object::pair{std::string("objectConfidenceThreshold"), this->getNumberSetting(std::string("OBJECT_CONFIDENCE_THRESHOLD"), 0.5)}, 
        object::pair{std::string("enablePoseDetection"), this->getBooleanSetting(std::string("ENABLE_POSE_DETECTION"), false)}, 
        object::pair{std::string("poseConfidenceThreshold"), this->getNumberSetting(std::string("POSE_CONFIDENCE_THRESHOLD"), 0.5)}, 
        object::pair{std::string("tfUpdateInterval"), this->getNumberSetting(std::string("TF_UPDATE_INTERVAL"), 1000)}, 
        object::pair{std::string("vlmUpdateInterval"), this->getNumberSetting(std::string("VLM_UPDATE_INTERVAL"), 10000)}, 
        object::pair{std::string("tfChangeThreshold"), this->getNumberSetting(std::string("TF_CHANGE_THRESHOLD"), 10)}, 
        object::pair{std::string("vlmChangeThreshold"), this->getNumberSetting(std::string("VLM_CHANGE_THRESHOLD"), 50)}, 
        object::pair{std::string("visionMode"), as<VisionMode>(this->getSetting(std::string("VISION_MODE")))}, 
        object::pair{std::string("screenCaptureInterval"), this->getNumberSetting(std::string("SCREEN_CAPTURE_INTERVAL"), 2000)}, 
        object::pair{std::string("tileSize"), this->getNumberSetting(std::string("TILE_SIZE"), 256)}, 
        object::pair{std::string("tileProcessingOrder"), as<any>(this->getSetting(std::string("TILE_PROCESSING_ORDER")))}, 
        object::pair{std::string("maxConcurrentTiles"), this->getNumberSetting(std::string("MAX_CONCURRENT_TILES"), 3)}, 
        object::pair{std::string("ocrEnabled"), this->getBooleanSetting(std::string("OCR_ENABLED"), true)}, 
        object::pair{std::string("ocrLanguage"), OR((this->getSetting(std::string("OCR_LANGUAGE"))), (std::string("eng")))}, 
        object::pair{std::string("ocrConfidenceThreshold"), this->getNumberSetting(std::string("OCR_CONFIDENCE_THRESHOLD"), 60)}, 
        object::pair{std::string("florence2Enabled"), this->getBooleanSetting(std::string("FLORENCE2_ENABLED"), true)}, 
        object::pair{std::string("florence2Provider"), as<any>(this->getSetting(std::string("FLORENCE2_PROVIDER")))}, 
        object::pair{std::string("florence2Endpoint"), this->getSetting(std::string("FLORENCE2_ENDPOINT"))}, 
        object::pair{std::string("florence2ApiKey"), this->getSetting(std::string("FLORENCE2_API_KEY"))}, 
        object::pair{std::string("florence2Timeout"), this->getNumberSetting(std::string("FLORENCE2_TIMEOUT"), 30000)}, 
        object::pair{std::string("enableFaceRecognition"), this->getBooleanSetting(std::string("ENABLE_FACE_RECOGNITION"), false)}, 
        object::pair{std::string("faceMatchThreshold"), this->getNumberSetting(std::string("FACE_MATCH_THRESHOLD"), 0.6)}, 
        object::pair{std::string("maxFaceProfiles"), this->getNumberSetting(std::string("MAX_FACE_PROFILES"), 1000)}, 
        object::pair{std::string("entityTimeout"), this->getNumberSetting(std::string("ENTITY_TIMEOUT"), 30000)}, 
        object::pair{std::string("maxTrackedEntities"), this->getNumberSetting(std::string("MAX_TRACKED_ENTITIES"), 100)}, 
        object::pair{std::string("enableGPUAcceleration"), this->getBooleanSetting(std::string("ENABLE_GPU_ACCELERATION"), true)}, 
        object::pair{std::string("maxMemoryUsageMB"), this->getNumberSetting(std::string("MAX_MEMORY_USAGE_MB"), 2000)}, 
        object::pair{std::string("debugMode"), this->getBooleanSetting(std::string("DEBUG_MODE"), false)}, 
        object::pair{std::string("logLevel"), OR(((as<any>(this->getSetting(std::string("LOG_LEVEL"))))), (std::string("info")))}
    };
    try
    {
        auto parsed = VisionConfigSchema->parse(rawConfig);
        logger->info(std::string("[ConfigurationManager] Configuration loaded successfully"));
        if (parsed->debugMode) {
            logger->debug(std::string("[ConfigurationManager] Configuration:"), parsed);
        }
        return parsed;
    }
    catch (const any& error)
    {
        logger->error(std::string("[ConfigurationManager] Invalid configuration:"), error);
        if (is<z->ZodError>(error)) {
            logger->error(std::string("[ConfigurationManager] Validation errors:"), error["errors"]);
        }
        return VisionConfigSchema->parse(object{});
    }
}

any ConfigurationManager::getSetting(string key)
{
    auto visionKey = std::string("VISION_") + key + string_empty;
    auto value = OR((this->runtime["getSetting"](visionKey)), (this->runtime["getSetting"](key)));
    return OR((value), (std::nullopt));
}

boolean ConfigurationManager::getBooleanSetting(string key, boolean defaultValue)
{
    auto value = this->getSetting(key);
    if (value == std::nullopt) {
        return defaultValue;
    }
    return value->toLowerCase() == std::string("true");
}

double ConfigurationManager::getNumberSetting(string key, double defaultValue)
{
    auto value = this->getSetting(key);
    if (value == std::nullopt) {
        return defaultValue;
    }
    auto parsed = Number(value);
    return (isNaN(parsed)) ? defaultValue : parsed;
}

VisionConfigOutput ConfigurationManager::get()
{
    return utils::assign(object{
    }, this->config);
}

void ConfigurationManager::update(Partial<VisionConfigInput> updates)
{
    try
    {
        auto newConfig = utils::assign(object{
        }, this->config, updates);
        auto parsed = VisionConfigSchema->parse(newConfig);
        this->config = parsed;
        logger->info(std::string("[ConfigurationManager] Configuration updated"));
    }
    catch (const any& error)
    {
        logger->error(std::string("[ConfigurationManager] Failed to update configuration:"), error);
        throw any(error);
    }
}

std::shared_ptr<VisionConfig> ConfigurationManager::toLegacyFormat()
{
    return object{
        object::pair{std::string("cameraName"), this->config->cameraName}, 
        object::pair{std::string("pixelChangeThreshold"), this->config->pixelChangeThreshold}, 
        object::pair{std::string("updateInterval"), this->config->updateInterval}, 
        object::pair{std::string("enableObjectDetection"), this->config->enableObjectDetection}, 
        object::pair{std::string("enablePoseDetection"), this->config->enablePoseDetection}, 
        object::pair{std::string("tfUpdateInterval"), this->config->tfUpdateInterval}, 
        object::pair{std::string("vlmUpdateInterval"), this->config->vlmUpdateInterval}, 
        object::pair{std::string("tfChangeThreshold"), this->config->tfChangeThreshold}, 
        object::pair{std::string("vlmChangeThreshold"), this->config->vlmChangeThreshold}, 
        object::pair{std::string("visionMode"), as<VisionMode>(this->config->visionMode)}, 
        object::pair{std::string("screenCaptureInterval"), this->config->screenCaptureInterval}, 
        object::pair{std::string("tileSize"), this->config->tileSize}, 
        object::pair{std::string("tileProcessingOrder"), this->config->tileProcessingOrder}, 
        object::pair{std::string("ocrEnabled"), this->config->ocrEnabled}, 
        object::pair{std::string("florence2Enabled"), this->config->florence2Enabled}
    };
}

Partial<VisionConfigInput> ConfigurationManager::getPreset(string name)
{
    auto presets = object{
        object::pair{std::string("high-performance"), object{
            object::pair{std::string("updateInterval"), 50}, 
            object::pair{std::string("tfUpdateInterval"), 500}, 
            object::pair{std::string("vlmUpdateInterval"), 5000}, 
            object::pair{std::string("enableGPUAcceleration"), true}, 
            object::pair{std::string("maxConcurrentTiles"), 5}
        }}, 
        object::pair{std::string("low-resource"), object{
            object::pair{std::string("updateInterval"), 200}, 
            object::pair{std::string("tfUpdateInterval"), 2000}, 
            object::pair{std::string("vlmUpdateInterval"), 20000}, 
            object::pair{std::string("enableObjectDetection"), false}, 
            object::pair{std::string("enablePoseDetection"), false}, 
            object::pair{std::string("maxMemoryUsageMB"), 500}, 
            object::pair{std::string("maxConcurrentTiles"), 1}
        }}, 
        object::pair{std::string("security-monitoring"), object{
            object::pair{std::string("enableObjectDetection"), true}, 
            object::pair{std::string("enablePoseDetection"), true}, 
            object::pair{std::string("enableFaceRecognition"), true}, 
            object::pair{std::string("updateInterval"), 100}, 
            object::pair{std::string("entityTimeout"), 60000}
        }}, 
        object::pair{std::string("screen-reader"), object{
            object::pair{std::string("visionMode"), std::string("SCREEN")}, 
            object::pair{std::string("ocrEnabled"), true}, 
            object::pair{std::string("florence2Enabled"), true}, 
            object::pair{std::string("screenCaptureInterval"), 1000}, 
            object::pair{std::string("tileProcessingOrder"), std::string("priority")}
        }}
    };
    return OR((const_(presets)[name]), (object{}));
}

std::shared_ptr<VisionConfig> defaultVisionConfig = object{
    object::pair{std::string("pixelChangeThreshold"), 50}, 
    object::pair{std::string("updateInterval"), 100}, 
    object::pair{std::string("enablePoseDetection"), false}, 
    object::pair{std::string("enableObjectDetection"), false}, 
    object::pair{std::string("tfUpdateInterval"), 1000}, 
    object::pair{std::string("vlmUpdateInterval"), 10000}, 
    object::pair{std::string("tfChangeThreshold"), 10}, 
    object::pair{std::string("vlmChangeThreshold"), 50}, 
    object::pair{std::string("visionMode"), as<VisionMode>(std::string("CAMERA"))}, 
    object::pair{std::string("screenCaptureInterval"), 2000}, 
    object::pair{std::string("tileSize"), 256}, 
    object::pair{std::string("tileProcessingOrder"), std::string("priority")}, 
    object::pair{std::string("ocrEnabled"), true}, 
    object::pair{std::string("florence2Enabled"), true}
};
any VisionConfigSchema = z->object(object{
    object::pair{std::string("cameraName"), z->string()->optional()}, 
    object::pair{std::string("enableCamera"), z->boolean()->default(true)}, 
    object::pair{std::string("pixelChangeThreshold"), z->number()->min(0)->max(100)->default(50)}, 
    object::pair{std::string("updateInterval"), z->number()->min(10)->max(10000)->default(100)}, 
    object::pair{std::string("enableObjectDetection"), z->boolean()->default(false)}, 
    object::pair{std::string("objectConfidenceThreshold"), z->number()->min(0)->max(1)->default(0.5)}, 
    object::pair{std::string("enablePoseDetection"), z->boolean()->default(false)}, 
    object::pair{std::string("poseConfidenceThreshold"), z->number()->min(0)->max(1)->default(0.5)}, 
    object::pair{std::string("tfUpdateInterval"), z->number()->min(100)->max(60000)->default(1000)}, 
    object::pair{std::string("vlmUpdateInterval"), z->number()->min(1000)->max(300000)->default(10000)}, 
    object::pair{std::string("tfChangeThreshold"), z->number()->min(0)->max(100)->default(10)}, 
    object::pair{std::string("vlmChangeThreshold"), z->number()->min(0)->max(100)->default(50)}, 
    object::pair{std::string("visionMode"), z->enum(array<string>{ std::string("OFF"), std::string("CAMERA"), std::string("SCREEN"), std::string("BOTH") })->default(std::string("CAMERA"))}, 
    object::pair{std::string("screenCaptureInterval"), z->number()->min(100)->max(60000)->default(2000)}, 
    object::pair{std::string("tileSize"), z->number()->min(64)->max(1024)->default(256)}, 
    object::pair{std::string("tileProcessingOrder"), z->enum(array<string>{ std::string("sequential"), std::string("priority"), std::string("random") })->default(std::string("priority"))}, 
    object::pair{std::string("maxConcurrentTiles"), z->number()->min(1)->max(10)->default(3)}, 
    object::pair{std::string("ocrEnabled"), z->boolean()->default(true)}, 
    object::pair{std::string("ocrLanguage"), z->string()->default(std::string("eng"))}, 
    object::pair{std::string("ocrConfidenceThreshold"), z->number()->min(0)->max(100)->default(60)}, 
    object::pair{std::string("florence2Enabled"), z->boolean()->default(true)}, 
    object::pair{std::string("florence2Provider"), z->enum(array<string>{ std::string("local"), std::string("azure"), std::string("huggingface"), std::string("replicate") })->optional()}, 
    object::pair{std::string("florence2Endpoint"), z->string()->url()->optional()}, 
    object::pair{std::string("florence2ApiKey"), z->string()->optional()}, 
    object::pair{std::string("florence2Timeout"), z->number()->min(1000)->max(300000)->default(30000)}, 
    object::pair{std::string("enableFaceRecognition"), z->boolean()->default(false)}, 
    object::pair{std::string("faceMatchThreshold"), z->number()->min(0)->max(1)->default(0.6)}, 
    object::pair{std::string("maxFaceProfiles"), z->number()->min(10)->max(10000)->default(1000)}, 
    object::pair{std::string("entityTimeout"), z->number()->min(1000)->max(300000)->default(30000)}, 
    object::pair{std::string("maxTrackedEntities"), z->number()->min(10)->max(1000)->default(100)}, 
    object::pair{std::string("enableGPUAcceleration"), z->boolean()->default(true)}, 
    object::pair{std::string("maxMemoryUsageMB"), z->number()->min(100)->max(8000)->default(2000)}, 
    object::pair{std::string("debugMode"), z->boolean()->default(false)}, 
    object::pair{std::string("logLevel"), z->enum(array<string>{ std::string("error"), std::string("warn"), std::string("info"), std::string("debug") })->default(std::string("info"))}
});

void Main(void)
{
}

MAIN
