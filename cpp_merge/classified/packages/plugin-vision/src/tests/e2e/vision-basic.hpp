#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-VISION_SRC_TESTS_E2E_VISION-BASIC_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-VISION_SRC_TESTS_E2E_VISION-BASIC_H
#include "core.h"
#include "@elizaos/core.h"
#include "@elizaos/core.h"
#include "../../service.h"
#include "../../action.h"

class VisionBasicE2ETestSuite;

class VisionBasicE2ETestSuite : public TestSuite, public std::enable_shared_from_this<VisionBasicE2ETestSuite> {
public:
    using std::enable_shared_from_this<VisionBasicE2ETestSuite>::shared_from_this;
    string name = std:("plugin-vision-basic-e2e");

    string description = std:("Basic end-to-end tests for vision plugin functionality");

    array<object> tests = array<object>{ object{
        object::pair{std:("name"), std:("Should initialize vision service")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("Testing vision service initialization..."));
            auto visionService = runtime->getService<std::shared_ptr<VisionService>>(as<any>(std:("VISION")));
            if (!visionService) {
                throw any(std::make_shared<Error>(std:("Vision service not available - service must be registered")));
            }
            auto isActive = visionService->isActive();
            auto cameraInfo = visionService->getCameraInfo();
            if (OR((!isActive), (!cameraInfo))) {
                console->warn(std:("⚠️  No camera detected. Service initialized but not active."));
                console->log(std:("   This is acceptable in CI/CD environments without cameras."));
            } else {
                console->log(std:("✓ Vision service initialized and active"));
                console->log(std:("✓ Connected to camera: ") + cameraInfo->name + std:(" (ID: ") + cameraInfo->id + std:(")"));
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("Should describe scene when requested")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("Testing scene description action..."));
            auto roomId = createUniqueUuid(runtime, std:("test-room"));
            auto message = object{
                object::pair{std:("id"), createUniqueUuid(runtime, std:("test-msg-describe"))}, 
                object::pair{std:("entityId"), runtime->agentId}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("what do you see?")}
                }}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("roomId"), std:("roomId")}, 
                object::pair{std:("createdAt"), Date->now()}
            };
            shared callbackCalled = false;
            shared callbackResponse = nullptr;
            auto state = object{
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}, 
                object::pair{std:("text"), string_empty}
            };
            auto visionService = runtime->getService<std::shared_ptr<VisionService>>(as<any>(std:("VISION")));
            auto isValid = std::async([=]() { describeSceneAction->validate(runtime, message, state); });
            if (OR((!visionService), (!visionService->isActive()))) {
                if (isValid) {
                    throw any(std::make_shared<Error>(std:("Action validation should return false when vision service is not active")));
                }
                console->log(std:("  Action validation correctly returned false (vision not active)"));
                std::async([=]() { describeSceneAction->handler(runtime, message, state, object{}, [=](auto response) mutable
                {
                    callbackCalled = true;
                    callbackResponse = response;
                    return array<any>();
                }
                ); });
                if (!callbackCalled) {
                    throw any(std::make_shared<Error>(std:("Callback was not called - action handler failed")));
                }
                if (OR((!callbackResponse), (!callbackResponse["text"]))) {
                    throw any(std::make_shared<Error>(std:("No response text returned from action")));
                }
                console->log(std:("✓ Scene description action handled unavailability correctly"));
                console->log(std:("  Response: ") + callbackResponse["text"] + string_empty);
                if (AND((!callbackResponse["text"]["includes"](std:("cannot see"))), (!callbackResponse["text"]["includes"](std:("no camera"))))) {
                    throw any(std::make_shared<Error>(std:("Response does not indicate camera unavailability")));
                }
            } else {
                if (!isValid) {
                    throw any(std::make_shared<Error>(std:("describeSceneAction validation failed despite active vision")));
                }
                console->log(std:("  Action validation: passed"));
                std::async([=]() { describeSceneAction->handler(runtime, message, state, object{}, [=](auto response) mutable
                {
                    callbackCalled = true;
                    callbackResponse = response;
                    return array<any>();
                }
                ); });
                if (!callbackCalled) {
                    throw any(std::make_shared<Error>(std:("Callback was not called - action handler failed")));
                }
                if (OR((!callbackResponse), (!callbackResponse["text"]))) {
                    throw any(std::make_shared<Error>(std:("No response text returned from action")));
                }
                console->log(std:("✓ Scene description action executed"));
                console->log(std:("  Response: ") + callbackResponse["text"] + string_empty);
                if (callbackResponse["thought"]) {
                    console->log(std:("  Thought: ") + callbackResponse["thought"] + string_empty);
                }
            }
            if (OR((!callbackResponse["actions"]), (!callbackResponse["actions"]["includes"](std:("DESCRIBE_SCENE"))))) {
                throw any(std::make_shared<Error>(std:("Response does not include DESCRIBE_SCENE action")));
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("Should capture image when requested")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("Testing image capture action..."));
            auto roomId = createUniqueUuid(runtime, std:("test-room"));
            auto message = object{
                object::pair{std:("id"), createUniqueUuid(runtime, std:("test-msg-capture"))}, 
                object::pair{std:("entityId"), runtime->agentId}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("take a photo")}
                }}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("roomId"), std:("roomId")}, 
                object::pair{std:("createdAt"), Date->now()}
            };
            shared callbackCalled = false;
            shared callbackResponse = nullptr;
            auto state = object{
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}, 
                object::pair{std:("text"), string_empty}
            };
            auto visionService = runtime->getService<std::shared_ptr<VisionService>>(as<any>(std:("VISION")));
            auto isValid = std::async([=]() { captureImageAction->validate(runtime, message, state); });
            if (OR((!visionService), (!visionService->isActive()))) {
                if (isValid) {
                    throw any(std::make_shared<Error>(std:("Action validation should return false when vision service is not active")));
                }
                console->log(std:("  Action validation correctly returned false (vision not active)"));
                std::async([=]() { captureImageAction->handler(runtime, message, state, object{}, [=](auto response) mutable
                {
                    callbackCalled = true;
                    callbackResponse = response;
                    return array<any>();
                }
                ); });
                if (!callbackCalled) {
                    throw any(std::make_shared<Error>(std:("Callback was not called - action handler failed")));
                }
                if (OR((!callbackResponse), (!callbackResponse["text"]))) {
                    throw any(std::make_shared<Error>(std:("No response text returned from action")));
                }
                console->log(std:("✓ Image capture action handled unavailability correctly"));
                console->log(std:("  Response: ") + callbackResponse["text"] + string_empty);
                if (AND((!callbackResponse["text"]["includes"](std:("cannot capture"))), (!callbackResponse["text"]["includes"](std:("no camera"))))) {
                    throw any(std::make_shared<Error>(std:("Response does not indicate camera unavailability")));
                }
            } else {
                if (!isValid) {
                    throw any(std::make_shared<Error>(std:("captureImageAction validation failed despite active vision")));
                }
                console->log(std:("  Action validation: passed"));
                std::async([=]() { captureImageAction->handler(runtime, message, state, object{}, [=](auto response) mutable
                {
                    callbackCalled = true;
                    callbackResponse = response;
                    return array<any>();
                }
                ); });
                if (!callbackCalled) {
                    throw any(std::make_shared<Error>(std:("Callback was not called - action handler failed")));
                }
                if (OR((!callbackResponse), (!callbackResponse["text"]))) {
                    throw any(std::make_shared<Error>(std:("No response text returned from action")));
                }
                console->log(std:("✓ Image capture action executed"));
                console->log(std:("  Response: ") + callbackResponse["text"] + string_empty);
                if (OR((!callbackResponse["attachments"]), (callbackResponse["attachments"]["length"] == 0))) {
                    throw any(std::make_shared<Error>(std:("No image attachment returned despite active camera")));
                }
                auto attachment = const_(callbackResponse["attachments"])[0];
                if (OR((!attachment["url"]), (!attachment["url"]["startsWith"](std:("data:image/"))))) {
                    throw any(std::make_shared<Error>(std:("Invalid image attachment format")));
                }
                console->log(std:("  ✓ Image attachment valid: ") + attachment["title"] + string_empty);
            }
            if (OR((!callbackResponse["actions"]), (!callbackResponse["actions"]["includes"](std:("CAPTURE_IMAGE"))))) {
                throw any(std::make_shared<Error>(std:("Response does not include CAPTURE_IMAGE action")));
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("Should provide vision context through provider")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("Testing vision provider..."));
            auto message = object{
                object::pair{std:("id"), createUniqueUuid(runtime, std:("test-msg-provider"))}, 
                object::pair{std:("entityId"), runtime->agentId}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("test provider")}
                }}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("roomId"), createUniqueUuid(runtime, std:("test-room"))}, 
                object::pair{std:("createdAt"), Date->now()}
            };
            auto state = std::async([=]() { runtime->composeState(message); });
            if (state->values->visionAvailable == undefined) {
                throw any(std::make_shared<Error>(std:("Vision provider data missing - provider not registered or failed")));
            }
            console->log(std:("✓ Vision provider data found in state"));
            console->log(std:("  Vision available: ") + state->values->visionAvailable + string_empty);
            console->log(std:("  Camera status: ") + state->values->cameraStatus + string_empty);
            if (!state->text->includes(std:("Visual Perception"))) {
                throw any(std::make_shared<Error>(std:("Vision context not included in state text")));
            }
            console->log(std:("✓ Vision context included in state text"));
            if (AND((state->values->visionAvailable), (state->values->sceneDescription))) {
                console->log(std:("  Scene description: ") + state->values->sceneDescription + string_empty);
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("Should handle scene changes efficiently")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("Testing scene change detection..."));
            auto visionService = runtime->getService<std::shared_ptr<VisionService>>(as<any>(std:("VISION")));
            if (!visionService) {
                throw any(std::make_shared<Error>(std:("Vision service not available")));
            }
            if (!visionService->isActive()) {
                console->warn(std:("⚠️  Vision service not active - skipping scene change test"));
                console->log(std:("   This is acceptable in environments without cameras"));
                return std::shared_ptr<Promise<void>>();
            }
            auto initialScene = std::async([=]() { visionService->getSceneDescription(); });
            console->log(std:("  Initial scene: ") + (initialScene) ? std:("Available") : std:("Pending...") + string_empty);
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, 2000);
            }
            ); });
            auto updatedScene = std::async([=]() { visionService->getSceneDescription(); });
            if (!updatedScene) {
                throw any(std::make_shared<Error>(std:("No scene description available after 2 seconds")));
            }
            console->log(std:("✓ Scene monitoring active"));
            console->log(std:("  Scene timestamp: ") + ((std::make_shared<Date>(updatedScene->timestamp)))->toISOString() + string_empty);
            console->log(std:("  Description: ") + updatedScene->description->substring(0, 100) + std:("..."));
            if (updatedScene->changePercentage != undefined) {
                console->log(std:("  Last change: ") + updatedScene->changePercentage->toFixed(1) + std:("%"));
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("Should detect objects and people in scene")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("Testing object and person detection..."));
            auto visionService = runtime->getService<std::shared_ptr<VisionService>>(as<any>(std:("VISION")));
            if (!visionService) {
                throw any(std::make_shared<Error>(std:("Vision service not available")));
            }
            if (!visionService->isActive()) {
                console->warn(std:("⚠️  Vision service not active - skipping detection test"));
                console->log(std:("   This is acceptable in environments without cameras"));
                return std::shared_ptr<Promise<void>>();
            }
            console->log(std:("  Waiting for scene analysis..."));
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, 7777);
            }
            ); });
            auto scene = std::async([=]() { visionService->getSceneDescription(); });
            if (!scene) {
                throw any(std::make_shared<Error>(std:("No scene description available after 3 seconds")));
            }
            console->log(std:("✓ Scene analysis complete"));
            console->log(std:("  Description: ") + scene->description->substring(0, 100) + std:("..."));
            console->log(std:("  Objects detected: ") + scene->objects->length + string_empty);
            console->log(std:("  People detected: ") + scene->people->length + string_empty);
            if (scene->objects->length > 0) {
                console->log(std:("  Detected objects:"));
                auto objectTypes = scene->objects->reduce([=](auto acc, auto obj) mutable
                {
                    acc[obj["type"]] = (OR((const_(acc)[obj["type"]]), (0))) + 1;
                    return acc;
                }
                , as<Record<string, double>>(object{}));
                for (auto& [type, count] : Object->entries(objectTypes))
                {
                    console->log(std:("    - ") + count + std:(" ") + type + std:("(s)"));
                }
                for (auto& obj : scene->objects)
                {
                    if (OR((OR((OR((!obj->id), (!obj->type))), (obj->confidence == undefined))), (!obj->boundingBox))) {
                        throw any(std::make_shared<Error>(std:("Detected object missing required fields")));
                    }
                }
            }
            if (scene->people->length > 0) {
                console->log(std:("  Detected people:"));
                for (auto& person : scene->people)
                {
                    console->log(std:("    - Person ") + person->id + std:(": ") + person->pose + std:(" pose, facing ") + person->facing + std:(", confidence ") + person->confidence->toFixed(2) + string_empty);
                    if (OR((OR((OR((OR((!person->id), (!person->pose))), (!person->facing))), (person->confidence == undefined))), (!person->boundingBox))) {
                        throw any(std::make_shared<Error>(std:("Detected person missing required fields")));
                    }
                }
            }
            if (scene->changePercentage > 10) {
                console->log(std:("  Scene change: ") + scene->changePercentage->toFixed(1) + std:("%"));
                if (AND((scene->objects->length == 0), (scene->people->length == 0))) {
                    console->warn(std:("  ⚠️  Significant scene change but no objects/people detected"));
                    console->log(std:("     This might indicate the motion detection threshold needs adjustment"));
                }
            }
        }
        }
    } };
};

#endif
