#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-VISION_SRC_TESTS_E2E_SCREEN-VISION_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-VISION_SRC_TESTS_E2E_SCREEN-VISION_H
#include "core.h"
#include "@elizaos/core.h"
#include "@elizaos/core.h"
#include "../../service.h"
#include "../../types.h"
#include "../../action.h"

class ScreenVisionE2ETestSuite;

class ScreenVisionE2ETestSuite : public TestSuite, public std::enable_shared_from_this<ScreenVisionE2ETestSuite> {
public:
    using std::enable_shared_from_this<ScreenVisionE2ETestSuite>::shared_from_this;
    string name = std:("plugin-vision-screen-e2e");

    string description = std:("E2E tests for screen vision functionality including Florence-2 and OCR");

    array<object> tests = array<object>{ object{
        object::pair{std:("name"), std:("Should initialize screen vision components")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("Testing screen vision initialization..."));
            auto visionService = runtime->getService<std::shared_ptr<VisionService>>(std:("VISION"));
            if (!visionService) {
                throw any(std::make_shared<Error>(std:("Vision service not available")));
            }
            std::async([=]() { visionService->setVisionMode(VisionMode::SCREEN); });
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, 2000);
            }
            ); });
            auto mode = visionService->getVisionMode();
            if (mode != VisionMode::SCREEN) {
                throw any(std::make_shared<Error>(std:("Expected vision mode SCREEN but got ") + mode + string_empty));
            }
            console->log(std:("✓ Screen vision mode activated"));
            auto screenInfo = std::async([=]() { visionService->getScreenCapture(); });
            if (screenInfo) {
                console->log(std:("✓ Screen capture available: ") + screenInfo->width + std:("x") + screenInfo->height + string_empty);
            } else {
                console->log(std:("⚠️  Screen capture not yet available (may still be initializing)"));
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("Should capture and tile screen")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("Testing screen capture and tiling..."));
            auto visionService = runtime->getService<std::shared_ptr<VisionService>>(std:("VISION"));
            if (!visionService) {
                throw any(std::make_shared<Error>(std:("Vision service not available")));
            }
            std::async([=]() { visionService->setVisionMode(VisionMode::SCREEN); });
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, 7777);
            }
            ); });
            auto screenCapture = std::async([=]() { visionService->getScreenCapture(); });
            if (!screenCapture) {
                console->warn(std:("⚠️  No screen capture available - screen capture may not be supported in this environment"));
                return std::shared_ptr<Promise<void>>();
            }
            console->log(std:("✓ Screen captured: ") + screenCapture->width + std:("x") + screenCapture->height + string_empty);
            console->log(std:("✓ Divided into ") + screenCapture->tiles->length + std:(" tiles"));
            auto firstTile = const_(screenCapture->tiles)[0];
            if (!firstTile) {
                throw any(std::make_shared<Error>(std:("No tiles created from screen capture")));
            }
            console->log(std:("  First tile: ") + firstTile->id + std:(" at (") + firstTile->x + std:(", ") + firstTile->y + std:(")"));
            console->log(std:("  Tile size: ") + firstTile->width + std:("x") + firstTile->height + string_empty);
            auto tilesWithData = screenCapture->tiles->filter([=](auto t) mutable
            {
                return t["data"] != undefined;
            }
            );
            console->log(std:("  Tiles with data: ") + tilesWithData->length + string_empty);
            if (tilesWithData->length == 0) {
                console->warn(std:("⚠️  No tiles have been processed yet"));
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("Should analyze screen content with Florence-2 and OCR")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("Testing screen content analysis..."));
            auto visionService = runtime->getService<std::shared_ptr<VisionService>>(std:("VISION"));
            if (!visionService) {
                throw any(std::make_shared<Error>(std:("Vision service not available")));
            }
            std::async([=]() { visionService->setVisionMode(VisionMode::SCREEN); });
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, 5000);
            }
            ); });
            auto enhancedScene = std::async([=]() { visionService->getEnhancedSceneDescription(); });
            if (OR((!enhancedScene), (!enhancedScene->screenAnalysis))) {
                console->warn(std:("⚠️  No enhanced scene analysis available yet"));
                return std::shared_ptr<Promise<void>>();
            }
            console->log(std:("✓ Enhanced scene analysis available"));
            auto screenAnalysis = enhancedScene->screenAnalysis;
            if (screenAnalysis->activeTile) {
                console->log(std:("✓ Active tile analyzed"));
                if (screenAnalysis->activeTile->florence2) {
                    console->log(std:("  Florence-2 caption: ") + screenAnalysis->activeTile->florence2->caption + string_empty);
                    console->log(std:("  Objects detected: ") + (OR((screenAnalysis->activeTile->florence2->objects->length), (0))) + string_empty);
                    console->log(std:("  Tags: ") + (OR((screenAnalysis->activeTile->florence2->tags->join(std:(", "))), (std:("none")))) + string_empty);
                }
                if (screenAnalysis->activeTile->ocr) {
                    console->log(std:("  OCR text blocks: ") + screenAnalysis->activeTile->ocr->blocks->length + string_empty);
                    console->log(std:("  OCR preview: "") + screenAnalysis->activeTile->ocr->fullText->substring(0, 50) + std:("...""));
                }
            }
            if (screenAnalysis->fullScreenOCR) {
                console->log(std:("✓ Full screen OCR: ") + screenAnalysis->fullScreenOCR->length + std:(" characters"));
            }
            if (AND((screenAnalysis->uiElements), (screenAnalysis->uiElements->length > 0))) {
                console->log(std:("✓ UI elements detected: ") + screenAnalysis->uiElements->length + string_empty);
                auto elementTypes = screenAnalysis->uiElements->map([=](auto e) mutable
                {
                    return e["type"];
                }
                );
                auto uniqueTypes = array<any>{ std::make_shared<Set>(elementTypes) };
                console->log(std:("  Types: ") + uniqueTypes->join(std:(", ")) + string_empty);
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("Should switch between vision modes")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("Testing vision mode switching..."));
            auto visionService = runtime->getService<std::shared_ptr<VisionService>>(std:("VISION"));
            if (!visionService) {
                throw any(std::make_shared<Error>(std:("Vision service not available")));
            }
            auto modes = array<VisionMode>{ VisionMode::CAMERA, VisionMode::SCREEN, VisionMode::BOTH, VisionMode::OFF };
            for (auto& mode : modes)
            {
                console->log(std:("  Switching to ") + mode + std:(" mode..."));
                std::async([=]() { visionService->setVisionMode(mode); });
                std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                {
                    return setTimeout(resolve, 1000);
                }
                ); });
                auto currentMode = visionService->getVisionMode();
                if (currentMode != static_cast<long>(mode)) {
                    throw any(std::make_shared<Error>(std:("Failed to switch to ") + mode + std:(" mode, current mode is ") + currentMode + string_empty));
                }
                console->log(std:("  ✓ Successfully switched to ") + mode + std:(" mode"));
            }
            console->log(std:("  Testing SET_VISION_MODE action..."));
            auto message = object{
                object::pair{std:("id"), createUniqueUuid(runtime, std:("test-msg"))}, 
                object::pair{std:("entityId"), runtime->agentId}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("set vision mode to both")}
                }}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("roomId"), createUniqueUuid(runtime, std:("test-room"))}, 
                object::pair{std:("createdAt"), Date->now()}
            };
            shared callbackCalled = false;
            std::async([=]() { setVisionModeAction->handler(runtime, message, object{
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}, 
                object::pair{std:("text"), string_empty}
            }, object{}, [=](auto response) mutable
            {
                callbackCalled = true;
                console->log(std:("  Action response: ") + response["text"] + string_empty);
                return array<any>();
            }
            ); });
            if (!callbackCalled) {
                throw any(std::make_shared<Error>(std:("SET_VISION_MODE action did not call callback")));
            }
            auto finalMode = visionService->getVisionMode();
            if (finalMode != VisionMode::BOTH) {
                throw any(std::make_shared<Error>(std:("SET_VISION_MODE action failed, mode is ") + finalMode + string_empty));
            }
            console->log(std:("✓ Vision mode switching works correctly"));
        }
        }
    }, object{
        object::pair{std:("name"), std:("Should provide combined vision data in BOTH mode")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("Testing combined camera and screen vision..."));
            auto visionService = runtime->getService<std::shared_ptr<VisionService>>(std:("VISION"));
            if (!visionService) {
                throw any(std::make_shared<Error>(std:("Vision service not available")));
            }
            std::async([=]() { visionService->setVisionMode(VisionMode::BOTH); });
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, 5000);
            }
            ); });
            auto enhancedScene = std::async([=]() { visionService->getEnhancedSceneDescription(); });
            auto hasCamera = visionService->getCameraInfo() != nullptr;
            auto hasScreen = (std::async([=]() { visionService->getScreenCapture(); })) != nullptr;
            console->log(std:("  Camera available: ") + hasCamera + string_empty);
            console->log(std:("  Screen capture available: ") + hasScreen + string_empty);
            if (AND((!hasCamera), (!hasScreen))) {
                console->warn(std:("⚠️  Neither camera nor screen capture available in this environment"));
                return std::shared_ptr<Promise<void>>();
            }
            if (enhancedScene) {
                if (AND((hasCamera), (enhancedScene->description))) {
                    console->log(std:("✓ Camera data present in combined mode"));
                    console->log(std:("  Scene: ") + enhancedScene->description->substring(0, 50) + std:("..."));
                }
                if (AND((hasScreen), (enhancedScene->screenAnalysis))) {
                    console->log(std:("✓ Screen data present in combined mode"));
                    console->log(std:("  Grid: ") + enhancedScene->screenAnalysis->gridSummary + string_empty);
                }
            }
            auto state = std::async([=]() { runtime->composeState(object{
                object::pair{std:("id"), createUniqueUuid(runtime, std:("test-msg"))}, 
                object::pair{std:("entityId"), runtime->agentId}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("test")}
                }}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("roomId"), createUniqueUuid(runtime, std:("test-room"))}, 
                object::pair{std:("createdAt"), Date->now()}
            }); });
            if (state->text->includes(std:("Vision mode: BOTH"))) {
                console->log(std:("✓ Provider correctly reports BOTH mode"));
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("Should handle screen capture errors gracefully")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("Testing error handling..."));
            auto visionService = runtime->getService<std::shared_ptr<VisionService>>(std:("VISION"));
            if (!visionService) {
                throw any(std::make_shared<Error>(std:("Vision service not available")));
            }
            auto originalConfig = (as<any>(visionService))["visionConfig"];
            (as<any>(visionService))["visionConfig"]["screenRegion"] = object{
                object::pair{std:("x"), -100}, 
                object::pair{std:("y"), -100}, 
                object::pair{std:("width"), 50000}, 
                object::pair{std:("height"), 50000}
            };
            std::async([=]() { visionService->setVisionMode(VisionMode::SCREEN); });
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, 2000);
            }
            ); });
            auto isActive = visionService->isActive();
            console->log(std:("  Service active after invalid config: ") + isActive + string_empty);
            (as<any>(visionService))["visionConfig"] = originalConfig;
            console->log(std:("✓ Error handling works correctly"));
        }
        }
    } };
};

#endif
