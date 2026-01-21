#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-VISION_SRC_TESTS_E2E_VISION-RUNTIME_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-VISION_SRC_TESTS_E2E_VISION-RUNTIME_H
#include "core.h"
#include "@elizaos/core.h"
#include "@elizaos/core.h"

class VisionRuntimeTestSuite;

class VisionRuntimeTestSuite : public TestSuite, public std::enable_shared_from_this<VisionRuntimeTestSuite> {
public:
    using std::enable_shared_from_this<VisionRuntimeTestSuite>::shared_from_this;
    string name = std::string("vision-runtime-tests");

    string description = std::string("Real runtime tests for vision plugin functionality");

    array<object> tests = array<object>{ object{
        object::pair{std::string("name"), std::string("Vision service initialization")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            logger->info(std::string("[Test] Testing vision service initialization..."));
            auto visionService = runtime["getService"](std::string("VISION"));
            if (!visionService) {
                throw any(std::make_shared<Error>(std::string("Vision service not found in runtime")));
            }
            if (type_of(visionService["isActive"]) != std::string("function")) {
                throw any(std::make_shared<Error>(std::string("Vision service missing isActive method")));
            }
            auto isActive = visionService["isActive"]();
            logger->info(std::string("[Test] Vision service active: ") + isActive + string_empty);
            if (AND((!isActive), (runtime["getSetting"](std::string("VISION_MODE")) != std::string("OFF")))) {
                throw any(std::make_shared<Error>(std::string("Vision service should be active but is not")));
            }
            logger->info(std::string("[Test] ✅ Vision service initialization test passed"));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Scene description functionality")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            logger->info(std::string("[Test] Testing scene description..."));
            auto visionService = runtime["getService"](std::string("VISION"));
            if (!visionService) {
                throw any(std::make_shared<Error>(std::string("Vision service not found")));
            }
            auto scene = std::async([=]() { visionService["getSceneDescription"](); });
            if (!scene) {
                logger->warn(std::string("[Test] No scene description available (camera might not be connected)"));
                return std::shared_ptr<Promise<void>>();
            }
            if (type_of(scene["timestamp"]) != std::string("number")) {
                throw any(std::make_shared<Error>(std::string("Scene description missing timestamp")));
            }
            if (type_of(scene["description"]) != std::string("string")) {
                throw any(std::make_shared<Error>(std::string("Scene description missing description text")));
            }
            if (!Array->isArray(scene["objects"])) {
                throw any(std::make_shared<Error>(std::string("Scene description missing objects array")));
            }
            if (!Array->isArray(scene["people"])) {
                throw any(std::make_shared<Error>(std::string("Scene description missing people array")));
            }
            logger->info(std::string("[Test] Scene: ") + scene["description"]["substring"](0, 100) + std::string("..."));
            logger->info(std::string("[Test] Objects: ") + scene["objects"]["length"] + std::string(", People: ") + scene["people"]["length"] + string_empty);
            logger->info(std::string("[Test] ✅ Scene description test passed"));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Vision mode switching")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            logger->info(std::string("[Test] Testing vision mode switching..."));
            auto visionService = runtime["getService"](std::string("VISION"));
            if (!visionService) {
                throw any(std::make_shared<Error>(std::string("Vision service not found")));
            }
            auto originalMode = visionService["getVisionMode"]();
            logger->info(std::string("[Test] Original mode: ") + originalMode + string_empty);
            auto testModes = array<string>{ std::string("CAMERA"), std::string("SCREEN"), std::string("BOTH"), std::string("OFF") };
            for (auto& mode : testModes)
            {
                logger->info(std::string("[Test] Switching to mode: ") + mode + string_empty);
                std::async([=]() { visionService["setVisionMode"](mode); });
                auto currentMode = visionService["getVisionMode"]();
                if (currentMode != mode) {
                    throw any(std::make_shared<Error>(std::string("Failed to switch to mode ") + mode + std::string(", current mode is ") + currentMode + string_empty));
                }
            }
            std::async([=]() { visionService["setVisionMode"](originalMode); });
            logger->info(std::string("[Test] ✅ Vision mode switching test passed"));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("DESCRIBE_SCENE action execution")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            logger->info(std::string("[Test] Testing DESCRIBE_SCENE action..."));
            auto action = runtime["actions"]["find"]([=](auto a) mutable
            {
                return a["name"] == std::string("DESCRIBE_SCENE");
            }
            );
            if (!action) {
                throw any(std::make_shared<Error>(std::string("DESCRIBE_SCENE action not found")));
            }
            auto message = object{
                object::pair{std::string("id"), std::string("test-msg-") + Date->now() + string_empty}, 
                object::pair{std::string("entityId"), std::string("test-entity")}, 
                object::pair{std::string("roomId"), std::string("test-room")}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Describe what you see")}, 
                    object::pair{std::string("source"), std::string("test")}
                }}, 
                object::pair{std::string("createdAt"), Date->now()}
            };
            auto isValid = std::async([=]() { action["validate"](runtime, message); });
            if (!isValid) {
                throw any(std::make_shared<Error>(std::string("DESCRIBE_SCENE action validation failed")));
            }
            shared responseReceived = false;
            auto callback = [=](auto response) mutable
            {
                if (AND((response["text"]), (response["text"]["length"] > 0))) {
                    responseReceived = true;
                    logger->info(std::string("[Test] Action response: ") + response["text"]["substring"](0, 100) + std::string("..."));
                }
                return array<any>();
            };
            std::async([=]() { action["handler"](runtime, message, object{}, object{}, callback); });
            if (!responseReceived) {
                throw any(std::make_shared<Error>(std::string("DESCRIBE_SCENE action did not produce a response")));
            }
            logger->info(std::string("[Test] ✅ DESCRIBE_SCENE action test passed"));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Vision provider integration")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            logger->info(std::string("[Test] Testing vision provider..."));
            auto provider = runtime["providers"]["find"]([=](auto p) mutable
            {
                return p["name"] == std::string("visionProvider");
            }
            );
            if (!provider) {
                throw any(std::make_shared<Error>(std::string("Vision provider not found")));
            }
            auto message = object{
                object::pair{std::string("id"), std::string("test-msg-") + Date->now() + string_empty}, 
                object::pair{std::string("entityId"), std::string("test-entity")}, 
                object::pair{std::string("roomId"), std::string("test-room")}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("test")}, 
                    object::pair{std::string("source"), std::string("test")}
                }}, 
                object::pair{std::string("createdAt"), Date->now()}
            };
            auto state = object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}, 
                object::pair{std::string("text"), string_empty}
            };
            auto result = std::async([=]() { provider["get"](runtime, message, state); });
            if (OR((!result), (type_of(result) != std::string("object")))) {
                throw any(std::make_shared<Error>(std::string("Vision provider returned invalid result")));
            }
            if (AND((result["text"]), (result["text"]["includes"](std::string("I can see"))))) {
                logger->info(std::string("[Test] Provider text: ") + result["text"]["substring"](0, 100) + std::string("..."));
            }
            logger->info(std::string("[Test] ✅ Vision provider test passed"));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Florence-2 model initialization")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            logger->info(std::string("[Test] Testing Florence-2 model..."));
            auto visionService = runtime["getService"](std::string("VISION"));
            if (!visionService) {
                throw any(std::make_shared<Error>(std::string("Vision service not found")));
            }
            auto florence2Enabled = OR((runtime["getSetting"](std::string("FLORENCE2_ENABLED")) == std::string("true")), (runtime["getSetting"](std::string("VISION_FLORENCE2_ENABLED")) == std::string("true")));
            if (!florence2Enabled) {
                logger->info(std::string("[Test] Florence-2 is disabled, skipping test"));
                return std::shared_ptr<Promise<void>>();
            }
            auto mode = visionService["getVisionMode"]();
            if (OR((mode == std::string("SCREEN")), (mode == std::string("BOTH")))) {
                auto screenCapture = std::async([=]() { visionService["getScreenCapture"](); });
                if (screenCapture) {
                    logger->info(std::string("[Test] Screen capture available"));
                    logger->info(std::string("[Test] Screen size: ") + screenCapture["width"] + std::string("x") + screenCapture["height"] + string_empty);
                    logger->info(std::string("[Test] Tiles: ") + screenCapture["tiles"]["length"] + string_empty);
                }
            }
            logger->info(std::string("[Test] ✅ Florence-2 model test passed"));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("OCR service functionality")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            logger->info(std::string("[Test] Testing OCR service..."));
            auto visionService = runtime["getService"](std::string("VISION"));
            if (!visionService) {
                throw any(std::make_shared<Error>(std::string("Vision service not found")));
            }
            auto ocrEnabled = OR((runtime["getSetting"](std::string("OCR_ENABLED")) == std::string("true")), (runtime["getSetting"](std::string("VISION_OCR_ENABLED")) == std::string("true")));
            if (!ocrEnabled) {
                logger->info(std::string("[Test] OCR is disabled, skipping test"));
                return std::shared_ptr<Promise<void>>();
            }
            auto mode = visionService["getVisionMode"]();
            if (OR((mode == std::string("SCREEN")), (mode == std::string("BOTH")))) {
                auto enhancedScene = std::async([=]() { visionService["getEnhancedSceneDescription"](); });
                if (AND((enhancedScene), (enhancedScene["screenAnalysis"]))) {
                    auto ocrText = enhancedScene["screenAnalysis"]["fullScreenOCR"];
                    if (ocrText) {
                        logger->info(std::string("[Test] OCR extracted ") + ocrText["length"] + std::string(" characters"));
                        logger->info(std::string("[Test] OCR sample: ") + ocrText["substring"](0, 100) + std::string("..."));
                    }
                }
            }
            logger->info(std::string("[Test] ✅ OCR service test passed"));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Entity tracking system")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            logger->info(std::string("[Test] Testing entity tracking..."));
            auto visionService = runtime["getService"](std::string("VISION"));
            if (!visionService) {
                throw any(std::make_shared<Error>(std::string("Vision service not found")));
            }
            auto entityTracker = visionService["getEntityTracker"]();
            if (!entityTracker) {
                throw any(std::make_shared<Error>(std::string("Entity tracker not found")));
            }
            auto entities = entityTracker["getActiveEntities"]();
            logger->info(std::string("[Test] Active entities: ") + entities["length"] + string_empty);
            for (auto& entity : entities)
            {
                if (OR((OR((!entity["id"]), (!entity["type"]))), (!entity["lastSeen"]))) {
                    throw any(std::make_shared<Error>(std::string("Entity missing required fields")));
                }
                logger->info(std::string("[Test] Entity ") + entity["id"] + std::string(": type=") + entity["type"] + std::string(", tracked=") + entity["trackingDuration"] + std::string("ms"));
            }
            logger->info(std::string("[Test] ✅ Entity tracking test passed"));
        }
        }
    } };
};

#endif
