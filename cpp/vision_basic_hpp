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
    string name = std::string("plugin-vision-basic-e2e");

    string description = std::string("Basic end-to-end tests for vision plugin functionality");

    array<object> tests = array<object>{ object{
        object::pair{std::string("name"), std::string("Should initialize vision service")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("Testing vision service initialization..."));
            auto visionService = runtime->getService<std::shared_ptr<VisionService>>(as<any>(std::string("VISION")));
            if (!visionService) {
                throw any(std::make_shared<Error>(std::string("Vision service not available - service must be registered")));
            }
            auto isActive = visionService->isActive();
            auto cameraInfo = visionService->getCameraInfo();
            if (OR((!isActive), (!cameraInfo))) {
                console->warn(std::string("⚠️  No camera detected. Service initialized but not active."));
                console->log(std::string("   This is acceptable in CI/CD environments without cameras."));
            } else {
                console->log(std::string("✓ Vision service initialized and active"));
                console->log(std::string("✓ Connected to camera: ") + cameraInfo->name + std::string(" (ID: ") + cameraInfo->id + std::string(")"));
            }
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Should describe scene when requested")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("Testing scene description action..."));
            auto roomId = createUniqueUuid(runtime, std::string("test-room"));
            auto message = object{
                object::pair{std::string("id"), createUniqueUuid(runtime, std::string("test-msg-describe"))}, 
                object::pair{std::string("entityId"), runtime->agentId}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("what do you see?")}
                }}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("createdAt"), Date->now()}
            };
            shared callbackCalled = false;
            shared callbackResponse = nullptr;
            auto state = object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}, 
                object::pair{std::string("text"), string_empty}
            };
            auto visionService = runtime->getService<std::shared_ptr<VisionService>>(as<any>(std::string("VISION")));
            auto isValid = std::async([=]() { describeSceneAction->validate(runtime, message, state); });
            if (OR((!visionService), (!visionService->isActive()))) {
                if (isValid) {
                    throw any(std::make_shared<Error>(std::string("Action validation should return false when vision service is not active")));
                }
                console->log(std::string("  Action validation correctly returned false (vision not active)"));
                std::async([=]() { describeSceneAction->handler(runtime, message, state, object{}, [=](auto response) mutable
                {
                    callbackCalled = true;
                    callbackResponse = response;
                    return array<any>();
                }
                ); });
                if (!callbackCalled) {
                    throw any(std::make_shared<Error>(std::string("Callback was not called - action handler failed")));
                }
                if (OR((!callbackResponse), (!callbackResponse["text"]))) {
                    throw any(std::make_shared<Error>(std::string("No response text returned from action")));
                }
                console->log(std::string("✓ Scene description action handled unavailability correctly"));
                console->log(std::string("  Response: ") + callbackResponse["text"] + string_empty);
                if (AND((!callbackResponse["text"]["includes"](std::string("cannot see"))), (!callbackResponse["text"]["includes"](std::string("no camera"))))) {
                    throw any(std::make_shared<Error>(std::string("Response does not indicate camera unavailability")));
                }
            } else {
                if (!isValid) {
                    throw any(std::make_shared<Error>(std::string("describeSceneAction validation failed despite active vision")));
                }
                console->log(std::string("  Action validation: passed"));
                std::async([=]() { describeSceneAction->handler(runtime, message, state, object{}, [=](auto response) mutable
                {
                    callbackCalled = true;
                    callbackResponse = response;
                    return array<any>();
                }
                ); });
                if (!callbackCalled) {
                    throw any(std::make_shared<Error>(std::string("Callback was not called - action handler failed")));
                }
                if (OR((!callbackResponse), (!callbackResponse["text"]))) {
                    throw any(std::make_shared<Error>(std::string("No response text returned from action")));
                }
                console->log(std::string("✓ Scene description action executed"));
                console->log(std::string("  Response: ") + callbackResponse["text"] + string_empty);
                if (callbackResponse["thought"]) {
                    console->log(std::string("  Thought: ") + callbackResponse["thought"] + string_empty);
                }
            }
            if (OR((!callbackResponse["actions"]), (!callbackResponse["actions"]["includes"](std::string("DESCRIBE_SCENE"))))) {
                throw any(std::make_shared<Error>(std::string("Response does not include DESCRIBE_SCENE action")));
            }
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Should capture image when requested")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("Testing image capture action..."));
            auto roomId = createUniqueUuid(runtime, std::string("test-room"));
            auto message = object{
                object::pair{std::string("id"), createUniqueUuid(runtime, std::string("test-msg-capture"))}, 
                object::pair{std::string("entityId"), runtime->agentId}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("take a photo")}
                }}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("createdAt"), Date->now()}
            };
            shared callbackCalled = false;
            shared callbackResponse = nullptr;
            auto state = object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}, 
                object::pair{std::string("text"), string_empty}
            };
            auto visionService = runtime->getService<std::shared_ptr<VisionService>>(as<any>(std::string("VISION")));
            auto isValid = std::async([=]() { captureImageAction->validate(runtime, message, state); });
            if (OR((!visionService), (!visionService->isActive()))) {
                if (isValid) {
                    throw any(std::make_shared<Error>(std::string("Action validation should return false when vision service is not active")));
                }
                console->log(std::string("  Action validation correctly returned false (vision not active)"));
                std::async([=]() { captureImageAction->handler(runtime, message, state, object{}, [=](auto response) mutable
                {
                    callbackCalled = true;
                    callbackResponse = response;
                    return array<any>();
                }
                ); });
                if (!callbackCalled) {
                    throw any(std::make_shared<Error>(std::string("Callback was not called - action handler failed")));
                }
                if (OR((!callbackResponse), (!callbackResponse["text"]))) {
                    throw any(std::make_shared<Error>(std::string("No response text returned from action")));
                }
                console->log(std::string("✓ Image capture action handled unavailability correctly"));
                console->log(std::string("  Response: ") + callbackResponse["text"] + string_empty);
                if (AND((!callbackResponse["text"]["includes"](std::string("cannot capture"))), (!callbackResponse["text"]["includes"](std::string("no camera"))))) {
                    throw any(std::make_shared<Error>(std::string("Response does not indicate camera unavailability")));
                }
            } else {
                if (!isValid) {
                    throw any(std::make_shared<Error>(std::string("captureImageAction validation failed despite active vision")));
                }
                console->log(std::string("  Action validation: passed"));
                std::async([=]() { captureImageAction->handler(runtime, message, state, object{}, [=](auto response) mutable
                {
                    callbackCalled = true;
                    callbackResponse = response;
                    return array<any>();
                }
                ); });
                if (!callbackCalled) {
                    throw any(std::make_shared<Error>(std::string("Callback was not called - action handler failed")));
                }
                if (OR((!callbackResponse), (!callbackResponse["text"]))) {
                    throw any(std::make_shared<Error>(std::string("No response text returned from action")));
                }
                console->log(std::string("✓ Image capture action executed"));
                console->log(std::string("  Response: ") + callbackResponse["text"] + string_empty);
                if (OR((!callbackResponse["attachments"]), (callbackResponse["attachments"]["length"] == 0))) {
                    throw any(std::make_shared<Error>(std::string("No image attachment returned despite active camera")));
                }
                auto attachment = const_(callbackResponse["attachments"])[0];
                if (OR((!attachment["url"]), (!attachment["url"]["startsWith"](std::string("data:image/"))))) {
                    throw any(std::make_shared<Error>(std::string("Invalid image attachment format")));
                }
                console->log(std::string("  ✓ Image attachment valid: ") + attachment["title"] + string_empty);
            }
            if (OR((!callbackResponse["actions"]), (!callbackResponse["actions"]["includes"](std::string("CAPTURE_IMAGE"))))) {
                throw any(std::make_shared<Error>(std::string("Response does not include CAPTURE_IMAGE action")));
            }
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Should provide vision context through provider")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("Testing vision provider..."));
            auto message = object{
                object::pair{std::string("id"), createUniqueUuid(runtime, std::string("test-msg-provider"))}, 
                object::pair{std::string("entityId"), runtime->agentId}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("test provider")}
                }}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("roomId"), createUniqueUuid(runtime, std::string("test-room"))}, 
                object::pair{std::string("createdAt"), Date->now()}
            };
            auto state = std::async([=]() { runtime->composeState(message); });
            if (state->values->visionAvailable == undefined) {
                throw any(std::make_shared<Error>(std::string("Vision provider data missing - provider not registered or failed")));
            }
            console->log(std::string("✓ Vision provider data found in state"));
            console->log(std::string("  Vision available: ") + state->values->visionAvailable + string_empty);
            console->log(std::string("  Camera status: ") + state->values->cameraStatus + string_empty);
            if (!state->text->includes(std::string("Visual Perception"))) {
                throw any(std::make_shared<Error>(std::string("Vision context not included in state text")));
            }
            console->log(std::string("✓ Vision context included in state text"));
            if (AND((state->values->visionAvailable), (state->values->sceneDescription))) {
                console->log(std::string("  Scene description: ") + state->values->sceneDescription + string_empty);
            }
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Should handle scene changes efficiently")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("Testing scene change detection..."));
            auto visionService = runtime->getService<std::shared_ptr<VisionService>>(as<any>(std::string("VISION")));
            if (!visionService) {
                throw any(std::make_shared<Error>(std::string("Vision service not available")));
            }
            if (!visionService->isActive()) {
                console->warn(std::string("⚠️  Vision service not active - skipping scene change test"));
                console->log(std::string("   This is acceptable in environments without cameras"));
                return std::shared_ptr<Promise<void>>();
            }
            auto initialScene = std::async([=]() { visionService->getSceneDescription(); });
            console->log(std::string("  Initial scene: ") + (initialScene) ? std::string("Available") : std::string("Pending...") + string_empty);
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, 2000);
            }
            ); });
            auto updatedScene = std::async([=]() { visionService->getSceneDescription(); });
            if (!updatedScene) {
                throw any(std::make_shared<Error>(std::string("No scene description available after 2 seconds")));
            }
            console->log(std::string("✓ Scene monitoring active"));
            console->log(std::string("  Scene timestamp: ") + ((std::make_shared<Date>(updatedScene->timestamp)))->toISOString() + string_empty);
            console->log(std::string("  Description: ") + updatedScene->description->substring(0, 100) + std::string("..."));
            if (updatedScene->changePercentage != undefined) {
                console->log(std::string("  Last change: ") + updatedScene->changePercentage->toFixed(1) + std::string("%"));
            }
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Should detect objects and people in scene")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("Testing object and person detection..."));
            auto visionService = runtime->getService<std::shared_ptr<VisionService>>(as<any>(std::string("VISION")));
            if (!visionService) {
                throw any(std::make_shared<Error>(std::string("Vision service not available")));
            }
            if (!visionService->isActive()) {
                console->warn(std::string("⚠️  Vision service not active - skipping detection test"));
                console->log(std::string("   This is acceptable in environments without cameras"));
                return std::shared_ptr<Promise<void>>();
            }
            console->log(std::string("  Waiting for scene analysis..."));
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, 7777);
            }
            ); });
            auto scene = std::async([=]() { visionService->getSceneDescription(); });
            if (!scene) {
                throw any(std::make_shared<Error>(std::string("No scene description available after 3 seconds")));
            }
            console->log(std::string("✓ Scene analysis complete"));
            console->log(std::string("  Description: ") + scene->description->substring(0, 100) + std::string("..."));
            console->log(std::string("  Objects detected: ") + scene->objects->length + string_empty);
            console->log(std::string("  People detected: ") + scene->people->length + string_empty);
            if (scene->objects->length > 0) {
                console->log(std::string("  Detected objects:"));
                auto objectTypes = scene->objects->reduce([=](auto acc, auto obj) mutable
                {
                    acc[obj["type"]] = (OR((const_(acc)[obj["type"]]), (0))) + 1;
                    return acc;
                }
                , as<Record<string, double>>(object{}));
                for (auto& [type, count] : Object->entries(objectTypes))
                {
                    console->log(std::string("    - ") + count + std::string(" ") + type + std::string("(s)"));
                }
                for (auto& obj : scene->objects)
                {
                    if (OR((OR((OR((!obj->id), (!obj->type))), (obj->confidence == undefined))), (!obj->boundingBox))) {
                        throw any(std::make_shared<Error>(std::string("Detected object missing required fields")));
                    }
                }
            }
            if (scene->people->length > 0) {
                console->log(std::string("  Detected people:"));
                for (auto& person : scene->people)
                {
                    console->log(std::string("    - Person ") + person->id + std::string(": ") + person->pose + std::string(" pose, facing ") + person->facing + std::string(", confidence ") + person->confidence->toFixed(2) + string_empty);
                    if (OR((OR((OR((OR((!person->id), (!person->pose))), (!person->facing))), (person->confidence == undefined))), (!person->boundingBox))) {
                        throw any(std::make_shared<Error>(std::string("Detected person missing required fields")));
                    }
                }
            }
            if (scene->changePercentage > 10) {
                console->log(std::string("  Scene change: ") + scene->changePercentage->toFixed(1) + std::string("%"));
                if (AND((scene->objects->length == 0), (scene->people->length == 0))) {
                    console->warn(std::string("  ⚠️  Significant scene change but no objects/people detected"));
                    console->log(std::string("     This might indicate the motion detection threshold needs adjustment"));
                }
            }
        }
        }
    } };
};

#endif
