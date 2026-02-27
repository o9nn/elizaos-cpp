#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-vision/src/config.h"

ConfigurationManager::ConfigurationManager(any runtime) {
    this->runtime = runtime;
    this->config = this->loadConfiguration();
}

VisionConfigOutput ConfigurationManager::loadConfiguration()
{
    auto rawConfig = object{
        object::pair{std:("cameraName"), OR((this->getSetting(std:("CAMERA_NAME"))), (this->getSetting(std:("VISION_CAMERA_NAME"))))}, 
        object::pair{std:("enableCamera"), this->getBooleanSetting(std:("ENABLE_CAMERA"), true)}, 
        object::pair{std:("pixelChangeThreshold"), this->getNumberSetting(std:("PIXEL_CHANGE_THRESHOLD"), 50)}, 
        object::pair{std:("updateInterval"), this->getNumberSetting(std:("UPDATE_INTERVAL"), 100)}, 
        object::pair{std:("enableObjectDetection"), this->getBooleanSetting(std:("ENABLE_OBJECT_DETECTION"), false)}, 
        object::pair{std:("objectConfidenceThreshold"), this->getNumberSetting(std:("OBJECT_CONFIDENCE_THRESHOLD"), 0.5)}, 
        object::pair{std:("enablePoseDetection"), this->getBooleanSetting(std:("ENABLE_POSE_DETECTION"), false)}, 
        object::pair{std:("poseConfidenceThreshold"), this->getNumberSetting(std:("POSE_CONFIDENCE_THRESHOLD"), 0.5)}, 
        object::pair{std:("tfUpdateInterval"), this->getNumberSetting(std:("TF_UPDATE_INTERVAL"), 1000)}, 
        object::pair{std:("vlmUpdateInterval"), this->getNumberSetting(std:("VLM_UPDATE_INTERVAL"), 10000)}, 
        object::pair{std:("tfChangeThreshold"), this->getNumberSetting(std:("TF_CHANGE_THRESHOLD"), 10)}, 
        object::pair{std:("vlmChangeThreshold"), this->getNumberSetting(std:("VLM_CHANGE_THRESHOLD"), 50)}, 
        object::pair{std:("visionMode"), as<VisionMode>(this->getSetting(std:("VISION_MODE")))}, 
        object::pair{std:("screenCaptureInterval"), this->getNumberSetting(std:("SCREEN_CAPTURE_INTERVAL"), 2000)}, 
        object::pair{std:("tileSize"), this->getNumberSetting(std:("TILE_SIZE"), 256)}, 
        object::pair{std:("tileProcessingOrder"), as<any>(this->getSetting(std:("TILE_PROCESSING_ORDER")))}, 
        object::pair{std:("maxConcurrentTiles"), this->getNumberSetting(std:("MAX_CONCURRENT_TILES"), 3)}, 
        object::pair{std:("ocrEnabled"), this->getBooleanSetting(std:("OCR_ENABLED"), true)}, 
        object::pair{std:("ocrLanguage"), OR((this->getSetting(std:("OCR_LANGUAGE"))), (std:("eng")))}, 
        object::pair{std:("ocrConfidenceThreshold"), this->getNumberSetting(std:("OCR_CONFIDENCE_THRESHOLD"), 60)}, 
        object::pair{std:("florence2Enabled"), this->getBooleanSetting(std:("FLORENCE2_ENABLED"), true)}, 
        object::pair{std:("florence2Provider"), as<any>(this->getSetting(std:("FLORENCE2_PROVIDER")))}, 
        object::pair{std:("florence2Endpoint"), this->getSetting(std:("FLORENCE2_ENDPOINT"))}, 
        object::pair{std:("florence2ApiKey"), this->getSetting(std:("FLORENCE2_API_KEY"))}, 
        object::pair{std:("florence2Timeout"), this->getNumberSetting(std:("FLORENCE2_TIMEOUT"), 30000)}, 
        object::pair{std:("enableFaceRecognition"), this->getBooleanSetting(std:("ENABLE_FACE_RECOGNITION"), false)}, 
        object::pair{std:("faceMatchThreshold"), this->getNumberSetting(std:("FACE_MATCH_THRESHOLD"), 0.6)}, 
        object::pair{std:("maxFaceProfiles"), this->getNumberSetting(std:("MAX_FACE_PROFILES"), 1000)}, 
        object::pair{std:("entityTimeout"), this->getNumberSetting(std:("ENTITY_TIMEOUT"), 30000)}, 
        object::pair{std:("maxTrackedEntities"), this->getNumberSetting(std:("MAX_TRACKED_ENTITIES"), 100)}, 
        object::pair{std:("enableGPUAcceleration"), this->getBooleanSetting(std:("ENABLE_GPU_ACCELERATION"), true)}, 
        object::pair{std:("maxMemoryUsageMB"), this->getNumberSetting(std:("MAX_MEMORY_USAGE_MB"), 2000)}, 
        object::pair{std:("debugMode"), this->getBooleanSetting(std:("DEBUG_MODE"), false)}, 
        object::pair{std:("logLevel"), OR(((as<any>(this->getSetting(std:("LOG_LEVEL"))))), (std:("info")))}
    };
    try
    {
        auto parsed = VisionConfigSchema->parse(rawConfig);
        logger->info(std:("[ConfigurationManager] Configuration loaded successfully"));
        if (parsed->debugMode) {
            logger->debug(std:("[ConfigurationManager] Configuration:"), parsed);
        }
        return parsed;
    }
    catch (const any& error)
    {
        logger->error(std:("[ConfigurationManager] Invalid configuration:"), error);
        if (is<z->ZodError>(error)) {
            logger->error(std:("[ConfigurationManager] Validation errors:"), error["errors"]);
        }
        return VisionConfigSchema->parse(object{});
    }
}

any ConfigurationManager::getSetting(string key)
{
    auto visionKey = std:("VISION_") + key + string_empty;
    auto value = OR((this->runtime["getSetting"](visionKey)), (this->runtime["getSetting"](key)));
    return OR((value), (undefined));
}

boolean ConfigurationManager::getBooleanSetting(string key, boolean defaultValue)
{
    auto value = this->getSetting(key);
    if (value == undefined) {
        return defaultValue;
    }
    return value->toLowerCase() == std:("true");
}

double ConfigurationManager::getNumberSetting(string key, double defaultValue)
{
    auto value = this->getSetting(key);
    if (value == undefined) {
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
        logger->info(std:("[ConfigurationManager] Configuration updated"));
    }
    catch (const any& error)
    {
        logger->error(std:("[ConfigurationManager] Failed to update configuration:"), error);
        throw any(error);
    }
}

std::shared_ptr<VisionConfig> ConfigurationManager::toLegacyFormat()
{
    return object{
        object::pair{std:("cameraName"), this->config->cameraName}, 
        object::pair{std:("pixelChangeThreshold"), this->config->pixelChangeThreshold}, 
        object::pair{std:("updateInterval"), this->config->updateInterval}, 
        object::pair{std:("enableObjectDetection"), this->config->enableObjectDetection}, 
        object::pair{std:("enablePoseDetection"), this->config->enablePoseDetection}, 
        object::pair{std:("tfUpdateInterval"), this->config->tfUpdateInterval}, 
        object::pair{std:("vlmUpdateInterval"), this->config->vlmUpdateInterval}, 
        object::pair{std:("tfChangeThreshold"), this->config->tfChangeThreshold}, 
        object::pair{std:("vlmChangeThreshold"), this->config->vlmChangeThreshold}, 
        object::pair{std:("visionMode"), as<VisionMode>(this->config->visionMode)}, 
        object::pair{std:("screenCaptureInterval"), this->config->screenCaptureInterval}, 
        object::pair{std:("tileSize"), this->config->tileSize}, 
        object::pair{std:("tileProcessingOrder"), this->config->tileProcessingOrder}, 
        object::pair{std:("ocrEnabled"), this->config->ocrEnabled}, 
        object::pair{std:("florence2Enabled"), this->config->florence2Enabled}
    };
}

Partial<VisionConfigInput> ConfigurationManager::getPreset(string name)
{
    auto presets = object{
        object::pair{std:("high-performance"), object{
            object::pair{std:("updateInterval"), 50}, 
            object::pair{std:("tfUpdateInterval"), 500}, 
            object::pair{std:("vlmUpdateInterval"), 5000}, 
            object::pair{std:("enableGPUAcceleration"), true}, 
            object::pair{std:("maxConcurrentTiles"), 5}
        }}, 
        object::pair{std:("low-resource"), object{
            object::pair{std:("updateInterval"), 200}, 
            object::pair{std:("tfUpdateInterval"), 2000}, 
            object::pair{std:("vlmUpdateInterval"), 20000}, 
            object::pair{std:("enableObjectDetection"), false}, 
            object::pair{std:("enablePoseDetection"), false}, 
            object::pair{std:("maxMemoryUsageMB"), 500}, 
            object::pair{std:("maxConcurrentTiles"), 1}
        }}, 
        object::pair{std:("security-monitoring"), object{
            object::pair{std:("enableObjectDetection"), true}, 
            object::pair{std:("enablePoseDetection"), true}, 
            object::pair{std:("enableFaceRecognition"), true}, 
            object::pair{std:("updateInterval"), 100}, 
            object::pair{std:("entityTimeout"), 60000}
        }}, 
        object::pair{std:("screen-reader"), object{
            object::pair{std:("visionMode"), std:("SCREEN")}, 
            object::pair{std:("ocrEnabled"), true}, 
            object::pair{std:("florence2Enabled"), true}, 
            object::pair{std:("screenCaptureInterval"), 1000}, 
            object::pair{std:("tileProcessingOrder"), std:("priority")}
        }}
    };
    return OR((const_(presets)[name]), (object{}));
}

std::shared_ptr<VisionConfig> defaultVisionConfig = object{
    object::pair{std:("pixelChangeThreshold"), 50}, 
    object::pair{std:("updateInterval"), 100}, 
    object::pair{std:("enablePoseDetection"), false}, 
    object::pair{std:("enableObjectDetection"), false}, 
    object::pair{std:("tfUpdateInterval"), 1000}, 
    object::pair{std:("vlmUpdateInterval"), 10000}, 
    object::pair{std:("tfChangeThreshold"), 10}, 
    object::pair{std:("vlmChangeThreshold"), 50}, 
    object::pair{std:("visionMode"), as<VisionMode>(std:("CAMERA"))}, 
    object::pair{std:("screenCaptureInterval"), 2000}, 
    object::pair{std:("tileSize"), 256}, 
    object::pair{std:("tileProcessingOrder"), std:("priority")}, 
    object::pair{std:("ocrEnabled"), true}, 
    object::pair{std:("florence2Enabled"), true}
};
any VisionConfigSchema = z->object(object{
    object::pair{std:("cameraName"), z->string()->optional()}, 
    object::pair{std:("enableCamera"), z->boolean()->default(true)}, 
    object::pair{std:("pixelChangeThreshold"), z->number()->min(0)->max(100)->default(50)}, 
    object::pair{std:("updateInterval"), z->number()->min(10)->max(10000)->default(100)}, 
    object::pair{std:("enableObjectDetection"), z->boolean()->default(false)}, 
    object::pair{std:("objectConfidenceThreshold"), z->number()->min(0)->max(1)->default(0.5)}, 
    object::pair{std:("enablePoseDetection"), z->boolean()->default(false)}, 
    object::pair{std:("poseConfidenceThreshold"), z->number()->min(0)->max(1)->default(0.5)}, 
    object::pair{std:("tfUpdateInterval"), z->number()->min(100)->max(60000)->default(1000)}, 
    object::pair{std:("vlmUpdateInterval"), z->number()->min(1000)->max(300000)->default(10000)}, 
    object::pair{std:("tfChangeThreshold"), z->number()->min(0)->max(100)->default(10)}, 
    object::pair{std:("vlmChangeThreshold"), z->number()->min(0)->max(100)->default(50)}, 
    object::pair{std:("visionMode"), z->enum(array<string>{ std:("OFF"), std:("CAMERA"), std:("SCREEN"), std:("BOTH") })->default(std:("CAMERA"))}, 
    object::pair{std:("screenCaptureInterval"), z->number()->min(100)->max(60000)->default(2000)}, 
    object::pair{std:("tileSize"), z->number()->min(64)->max(1024)->default(256)}, 
    object::pair{std:("tileProcessingOrder"), z->enum(array<string>{ std:("sequential"), std:("priority"), std:("random") })->default(std:("priority"))}, 
    object::pair{std:("maxConcurrentTiles"), z->number()->min(1)->max(10)->default(3)}, 
    object::pair{std:("ocrEnabled"), z->boolean()->default(true)}, 
    object::pair{std:("ocrLanguage"), z->string()->default(std:("eng"))}, 
    object::pair{std:("ocrConfidenceThreshold"), z->number()->min(0)->max(100)->default(60)}, 
    object::pair{std:("florence2Enabled"), z->boolean()->default(true)}, 
    object::pair{std:("florence2Provider"), z->enum(array<string>{ std:("local"), std:("azure"), std:("huggingface"), std:("replicate") })->optional()}, 
    object::pair{std:("florence2Endpoint"), z->string()->url()->optional()}, 
    object::pair{std:("florence2ApiKey"), z->string()->optional()}, 
    object::pair{std:("florence2Timeout"), z->number()->min(1000)->max(300000)->default(30000)}, 
    object::pair{std:("enableFaceRecognition"), z->boolean()->default(false)}, 
    object::pair{std:("faceMatchThreshold"), z->number()->min(0)->max(1)->default(0.6)}, 
    object::pair{std:("maxFaceProfiles"), z->number()->min(10)->max(10000)->default(1000)}, 
    object::pair{std:("entityTimeout"), z->number()->min(1000)->max(300000)->default(30000)}, 
    object::pair{std:("maxTrackedEntities"), z->number()->min(10)->max(1000)->default(100)}, 
    object::pair{std:("enableGPUAcceleration"), z->boolean()->default(true)}, 
    object::pair{std:("maxMemoryUsageMB"), z->number()->min(100)->max(8000)->default(2000)}, 
    object::pair{std:("debugMode"), z->boolean()->default(false)}, 
    object::pair{std:("logLevel"), z->enum(array<string>{ std:("error"), std:("warn"), std:("info"), std:("debug") })->default(std:("info"))}
});

void Main(void)
{
}

MAIN
