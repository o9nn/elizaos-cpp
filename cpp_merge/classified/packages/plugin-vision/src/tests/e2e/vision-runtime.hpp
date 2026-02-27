#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_VISION_SRC_TESTS_E2E_VISION_RUNTIME_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_VISION_SRC_TESTS_E2E_VISION_RUNTIME_H
#include "core.hpp"
// External dependency removed
// External dependency removed

class VisionRuntimeTestSuite;

class VisionRuntimeTestSuite : public TestSuite, public std::enable_shared_from_this<VisionRuntimeTestSuite> {
public:
    using std::enable_shared_from_this<VisionRuntimeTestSuite>::shared_from_this;
    string name = std:("vision-runtime-tests");

    string description = std:("Real runtime tests for vision plugin functionality");

    array<object> tests = array<object>{ object{
        object::pair{std:("name"), std:("Vision service initialization")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            logger->info(std:("[Test] Testing vision service initialization..."));
            auto visionService = runtime["getService"](std:("VISION"));
            if (!visionService) {
                throw any(std::make_shared<Error>(std:("Vision service not found in runtime")));
            }
            if (type_of(visionService["isActive"]) != std:("function")) {
                throw any(std::make_shared<Error>(std:("Vision service missing isActive method")));
            }
            auto isActive = visionService["isActive"]();
            logger->info(std:("[Test] Vision service active: ") + isActive + string_empty);
            if (AND((!isActive), (runtime["getSetting"](std:("VISION_MODE")) != std:("OFF")))) {
                throw any(std::make_shared<Error>(std:("Vision service should be active but is not")));
            }
            logger->info(std:("[Test] ✅ Vision service initialization test passed"));
        }
        }
    }, object{
        object::pair{std:("name"), std:("Scene description functionality")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            logger->info(std:("[Test] Testing scene description..."));
            auto visionService = runtime["getService"](std:("VISION"));
            if (!visionService) {
                throw any(std::make_shared<Error>(std:("Vision service not found")));
            }
            auto scene = std::async([=]() { visionService["getSceneDescription"](); });
            if (!scene) {
                logger->warn(std:("[Test] No scene description available (camera might not be connected)"));
                return std::shared_ptr<Promise<void>>();
            }
            if (type_of(scene["timestamp"]) != std:("number")) {
                throw any(std::make_shared<Error>(std:("Scene description missing timestamp")));
            }
            if (type_of(scene["description"]) != std:("string")) {
                throw any(std::make_shared<Error>(std:("Scene description missing description text")));
            }
            if (!Array->isArray(scene["objects"])) {
                throw any(std::make_shared<Error>(std:("Scene description missing objects array")));
            }
            if (!Array->isArray(scene["people"])) {
                throw any(std::make_shared<Error>(std:("Scene description missing people array")));
            }
            logger->info(std:("[Test] Scene: ") + scene["description"]["substring"](0, 100) + std:("..."));
            logger->info(std:("[Test] Objects: ") + scene["objects"]["length"] + std:(", People: ") + scene["people"]["length"] + string_empty);
            logger->info(std:("[Test] ✅ Scene description test passed"));
        }
        }
    }, object{
        object::pair{std:("name"), std:("Vision mode switching")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            logger->info(std:("[Test] Testing vision mode switching..."));
            auto visionService = runtime["getService"](std:("VISION"));
            if (!visionService) {
                throw any(std::make_shared<Error>(std:("Vision service not found")));
            }
            auto originalMode = visionService["getVisionMode"]();
            logger->info(std:("[Test] Original mode: ") + originalMode + string_empty);
            auto testModes = array<string>{ std:("CAMERA"), std:("SCREEN"), std:("BOTH"), std:("OFF") };
            for (auto& mode : testModes)
            {
                logger->info(std:("[Test] Switching to mode: ") + mode + string_empty);
                std::async([=]() { visionService["setVisionMode"](mode); });
                auto currentMode = visionService["getVisionMode"]();
                if (currentMode != mode) {
                    throw any(std::make_shared<Error>(std:("Failed to switch to mode ") + mode + std:(", current mode is ") + currentMode + string_empty));
                }
            }
            std::async([=]() { visionService["setVisionMode"](originalMode); });
            logger->info(std:("[Test] ✅ Vision mode switching test passed"));
        }
        }
    }, object{
        object::pair{std:("name"), std:("DESCRIBE_SCENE action execution")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            logger->info(std:("[Test] Testing DESCRIBE_SCENE action..."));
            auto action = runtime["actions"]["find"]([=](auto a) mutable
            {
                return a["name"] == std:("DESCRIBE_SCENE");
            }
            );
            if (!action) {
                throw any(std::make_shared<Error>(std:("DESCRIBE_SCENE action not found")));
            }
            auto message = object{
                object::pair{std:("id"), std:("test-msg-") + Date->now() + string_empty}, 
                object::pair{std:("entityId"), std:("test-entity")}, 
                object::pair{std:("roomId"), std:("test-room")}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("Describe what you see")}, 
                    object::pair{std:("source"), std:("test")}
                }}, 
                object::pair{std:("createdAt"), Date->now()}
            };
            auto isValid = std::async([=]() { action["validate"](runtime, message); });
            if (!isValid) {
                throw any(std::make_shared<Error>(std:("DESCRIBE_SCENE action validation failed")));
            }
            shared responseReceived = false;
            auto callback = [=](auto response) mutable
            {
                if (AND((response["text"]), (response["text"]["length"] > 0))) {
                    responseReceived = true;
                    logger->info(std:("[Test] Action response: ") + response["text"]["substring"](0, 100) + std:("..."));
                }
                return array<any>();
            };
            std::async([=]() { action["handler"](runtime, message, object{}, object{}, callback); });
            if (!responseReceived) {
                throw any(std::make_shared<Error>(std:("DESCRIBE_SCENE action did not produce a response")));
            }
            logger->info(std:("[Test] ✅ DESCRIBE_SCENE action test passed"));
        }
        }
    }, object{
        object::pair{std:("name"), std:("Vision provider integration")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            logger->info(std:("[Test] Testing vision provider..."));
            auto provider = runtime["providers"]["find"]([=](auto p) mutable
            {
                return p["name"] == std:("visionProvider");
            }
            );
            if (!provider) {
                throw any(std::make_shared<Error>(std:("Vision provider not found")));
            }
            auto message = object{
                object::pair{std:("id"), std:("test-msg-") + Date->now() + string_empty}, 
                object::pair{std:("entityId"), std:("test-entity")}, 
                object::pair{std:("roomId"), std:("test-room")}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("test")}, 
                    object::pair{std:("source"), std:("test")}
                }}, 
                object::pair{std:("createdAt"), Date->now()}
            };
            auto state = object{
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}, 
                object::pair{std:("text"), string_empty}
            };
            auto result = std::async([=]() { provider["get"](runtime, message, state); });
            if (OR((!result), (type_of(result) != std:("object")))) {
                throw any(std::make_shared<Error>(std:("Vision provider returned invalid result")));
            }
            if (AND((result["text"]), (result["text"]["includes"](std:("I can see"))))) {
                logger->info(std:("[Test] Provider text: ") + result["text"]["substring"](0, 100) + std:("..."));
            }
            logger->info(std:("[Test] ✅ Vision provider test passed"));
        }
        }
    }, object{
        object::pair{std:("name"), std:("Florence-2 model initialization")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            logger->info(std:("[Test] Testing Florence-2 model..."));
            auto visionService = runtime["getService"](std:("VISION"));
            if (!visionService) {
                throw any(std::make_shared<Error>(std:("Vision service not found")));
            }
            auto florence2Enabled = OR((runtime["getSetting"](std:("FLORENCE2_ENABLED")) == std:("true")), (runtime["getSetting"](std:("VISION_FLORENCE2_ENABLED")) == std:("true")));
            if (!florence2Enabled) {
                logger->info(std:("[Test] Florence-2 is disabled, skipping test"));
                return std::shared_ptr<Promise<void>>();
            }
            auto mode = visionService["getVisionMode"]();
            if (OR((mode == std:("SCREEN")), (mode == std:("BOTH")))) {
                auto screenCapture = std::async([=]() { visionService["getScreenCapture"](); });
                if (screenCapture) {
                    logger->info(std:("[Test] Screen capture available"));
                    logger->info(std:("[Test] Screen size: ") + screenCapture["width"] + std:("x") + screenCapture["height"] + string_empty);
                    logger->info(std:("[Test] Tiles: ") + screenCapture["tiles"]["length"] + string_empty);
                }
            }
            logger->info(std:("[Test] ✅ Florence-2 model test passed"));
        }
        }
    }, object{
        object::pair{std:("name"), std:("OCR service functionality")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            logger->info(std:("[Test] Testing OCR service..."));
            auto visionService = runtime["getService"](std:("VISION"));
            if (!visionService) {
                throw any(std::make_shared<Error>(std:("Vision service not found")));
            }
            auto ocrEnabled = OR((runtime["getSetting"](std:("OCR_ENABLED")) == std:("true")), (runtime["getSetting"](std:("VISION_OCR_ENABLED")) == std:("true")));
            if (!ocrEnabled) {
                logger->info(std:("[Test] OCR is disabled, skipping test"));
                return std::shared_ptr<Promise<void>>();
            }
            auto mode = visionService["getVisionMode"]();
            if (OR((mode == std:("SCREEN")), (mode == std:("BOTH")))) {
                auto enhancedScene = std::async([=]() { visionService["getEnhancedSceneDescription"](); });
                if (AND((enhancedScene), (enhancedScene["screenAnalysis"]))) {
                    auto ocrText = enhancedScene["screenAnalysis"]["fullScreenOCR"];
                    if (ocrText) {
                        logger->info(std:("[Test] OCR extracted ") + ocrText["length"] + std:(" characters"));
                        logger->info(std:("[Test] OCR sample: ") + ocrText["substring"](0, 100) + std:("..."));
                    }
                }
            }
            logger->info(std:("[Test] ✅ OCR service test passed"));
        }
        }
    }, object{
        object::pair{std:("name"), std:("Entity tracking system")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            logger->info(std:("[Test] Testing entity tracking..."));
            auto visionService = runtime["getService"](std:("VISION"));
            if (!visionService) {
                throw any(std::make_shared<Error>(std:("Vision service not found")));
            }
            auto entityTracker = visionService["getEntityTracker"]();
            if (!entityTracker) {
                throw any(std::make_shared<Error>(std:("Entity tracker not found")));
            }
            auto entities = entityTracker["getActiveEntities"]();
            logger->info(std:("[Test] Active entities: ") + entities["length"] + string_empty);
            for (auto& entity : entities)
            {
                if (OR((OR((!entity["id"]), (!entity["type"]))), (!entity["lastSeen"]))) {
                    throw any(std::make_shared<Error>(std:("Entity missing required fields")));
                }
                logger->info(std:("[Test] Entity ") + entity["id"] + std:(": type=") + entity["type"] + std:(", tracked=") + entity["trackingDuration"] + std:("ms"));
            }
            logger->info(std:("[Test] ✅ Entity tracking test passed"));
        }
        }
    } };
};

#endif
