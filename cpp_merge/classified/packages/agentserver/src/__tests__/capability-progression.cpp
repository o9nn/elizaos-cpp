#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/agentserver/src/__tests__/capability-progression.test.h"

void Main(void)
{
    describe(std:("Capability Progression System E2E Tests"), [=]() mutable
    {
        shared<any> runtime;
        shared<std::shared_ptr<CapabilityProgressionService>> progressionService;
        beforeEach([=]() mutable
        {
            auto character = object{
                object::pair{std:("id"), stringToUuid(std:("TestAgent"))}, 
                object::pair{std:("name"), std:("TestAgent")}, 
                object::pair{std:("system"), std:("You are a test agent for capability progression.")}, 
                object::pair{std:("bio"), array<string>{ std:("Test agent for progression system") }}, 
                object::pair{std:("messageExamples"), array<any>()}, 
                object::pair{std:("postExamples"), array<any>()}, 
                object::pair{std:("topics"), array<any>()}, 
                object::pair{std:("knowledge"), array<any>()}, 
                object::pair{std:("style"), object{
                    object::pair{std:("all"), array<any>()}, 
                    object::pair{std:("chat"), array<any>()}, 
                    object::pair{std:("post"), array<any>()}
                }}, 
                object::pair{std:("adjectives"), array<any>()}, 
                object::pair{std:("settings"), object{
                    object::pair{std:("MODEL_PROVIDER"), std:("test")}, 
                    object::pair{std:("ENABLE_AUTONOMY"), std:("false")}
                }}
            };
            runtime = std::make_shared<AgentRuntime>(object{
                object::pair{std:("character"), std:("character")}, 
                object::pair{std:("agentId"), stringToUuid(std:("TestAgent"))}, 
                object::pair{std:("plugins"), array<any>()}
            });
            runtime["setSetting"] = [=]() mutable
            {
            };
            progressionService = std::make_shared<CapabilityProgressionService>(runtime);
        }
        );
        it(std:("should start with only basic capabilities unlocked"), [=]() mutable
        {
            auto currentLevel = progressionService->getCurrentLevel();
            auto unlockedCapabilities = progressionService->getUnlockedCapabilities();
            auto progressionState = progressionService->getProgressionState();
            expect(currentLevel)->toBe(0);
            expect(unlockedCapabilities)->toEqual(array<string>{ std:("shell"), std:("naming") });
            expect(progressionState->agentNamed)->toBe(false);
            expect(progressionState->completedActions)->toEqual(array<any>());
            expect(progressionState->unlockedLevels)->toEqual(array<any>());
        }
        );
        it(std:("should not unlock browser capabilities until agent is named AND shell is used"), [=]() mutable
        {
            auto initialLevel = progressionService->getCurrentLevel();
            expect(initialLevel)->toBe(0);
            std::async([=]() { progressionService->recordAgentNamed(std:("TestBot")); });
            expect(progressionService->getCurrentLevel())->toBe(0);
            expect(progressionService->isCapabilityUnlocked(std:("browser")))->toBe(false);
            expect(progressionService->isCapabilityUnlocked(std:("stagehand")))->toBe(false);
            progressionService = std::make_shared<CapabilityProgressionService>(runtime);
            std::async([=]() { progressionService->recordCapabilityUsed(std:("shell")); });
            expect(progressionService->getCurrentLevel())->toBe(0);
            expect(progressionService->isCapabilityUnlocked(std:("browser")))->toBe(false);
        }
        );
        it(std:("should unlock browser capabilities when both naming and shell conditions are met"), [=]() mutable
        {
            expect(progressionService->getCurrentLevel())->toBe(0);
            std::async([=]() { progressionService->recordAgentNamed(std:("WebExplorer")); });
            std::async([=]() { progressionService->recordCapabilityUsed(std:("shell")); });
            expect(progressionService->getCurrentLevel())->toBe(1);
            expect(progressionService->isCapabilityUnlocked(std:("browser")))->toBe(true);
            expect(progressionService->isCapabilityUnlocked(std:("stagehand")))->toBe(true);
            expect(progressionService->isCapabilityUnlocked(std:("shell")))->toBe(true);
            expect(progressionService->isCapabilityUnlocked(std:("naming")))->toBe(true);
            auto progressionState = progressionService->getProgressionState();
            expect(progressionState->unlockedLevels)->toContain(std:("level_1_browser"));
            expect(progressionState->lastUnlockTime)->not->toBeNull();
        }
        );
        it(std:("should unlock vision capabilities when browser is used AND form is submitted"), [=]() mutable
        {
            std::async([=]() { progressionService->recordAgentNamed(std:("VisualAgent")); });
            std::async([=]() { progressionService->recordCapabilityUsed(std:("shell")); });
            expect(progressionService->getCurrentLevel())->toBe(1);
            std::async([=]() { progressionService->recordCapabilityUsed(std:("browser")); });
            expect(progressionService->getCurrentLevel())->toBe(1);
            expect(progressionService->isCapabilityUnlocked(std:("vision")))->toBe(false);
            std::async([=]() { progressionService->recordFormSubmitted(object{
                object::pair{std:("data"), std:("test form data")}
            }); });
            expect(progressionService->getCurrentLevel())->toBe(2);
            expect(progressionService->isCapabilityUnlocked(std:("vision")))->toBe(true);
            expect(progressionService->isCapabilityUnlocked(std:("screen_capture")))->toBe(true);
            expect(progressionService->isCapabilityUnlocked(std:("browser")))->toBe(true);
            expect(progressionService->isCapabilityUnlocked(std:("shell")))->toBe(true);
        }
        );
        it(std:("should unlock audio capabilities when vision is used"), [=]() mutable
        {
            std::async([=]() { progressionService->recordAgentNamed(std:("AudioAgent")); });
            std::async([=]() { progressionService->recordCapabilityUsed(std:("shell")); });
            std::async([=]() { progressionService->recordCapabilityUsed(std:("browser")); });
            std::async([=]() { progressionService->recordFormSubmitted(); });
            expect(progressionService->getCurrentLevel())->toBe(2);
            std::async([=]() { progressionService->recordCapabilityUsed(std:("vision")); });
            expect(progressionService->getCurrentLevel())->toBe(3);
            expect(progressionService->isCapabilityUnlocked(std:("microphone")))->toBe(true);
            expect(progressionService->isCapabilityUnlocked(std:("sam")))->toBe(true);
            expect(progressionService->isCapabilityUnlocked(std:("audio")))->toBe(true);
        }
        );
        it(std:("should unlock camera capabilities when microphone is used"), [=]() mutable
        {
            std::async([=]() { progressionService->recordAgentNamed(std:("CameraAgent")); });
            std::async([=]() { progressionService->recordCapabilityUsed(std:("shell")); });
            std::async([=]() { progressionService->recordCapabilityUsed(std:("browser")); });
            std::async([=]() { progressionService->recordFormSubmitted(); });
            std::async([=]() { progressionService->recordCapabilityUsed(std:("vision")); });
            expect(progressionService->getCurrentLevel())->toBe(3);
            std::async([=]() { progressionService->recordCapabilityUsed(std:("microphone")); });
            expect(progressionService->getCurrentLevel())->toBe(4);
            expect(progressionService->isCapabilityUnlocked(std:("camera")))->toBe(true);
            expect(progressionService->isCapabilityUnlocked(std:("advanced_vision")))->toBe(true);
        }
        );
        it(std:("should not allow skipping levels in progression"), [=]() mutable
        {
            expect(progressionService->getCurrentLevel())->toBe(0);
            std::async([=]() { progressionService->recordCapabilityUsed(std:("vision")); });
            std::async([=]() { progressionService->recordCapabilityUsed(std:("microphone")); });
            std::async([=]() { progressionService->recordFormSubmitted(); });
            expect(progressionService->getCurrentLevel())->toBe(0);
            expect(progressionService->isCapabilityUnlocked(std:("vision")))->toBe(false);
            expect(progressionService->isCapabilityUnlocked(std:("microphone")))->toBe(false);
            expect(progressionService->isCapabilityUnlocked(std:("browser")))->toBe(false);
        }
        );
        it(std:("should track progression state correctly through all levels"), [=]() mutable
        {
            auto state = progressionService->getProgressionState();
            expect(state->currentLevel)->toBe(0);
            expect(state->unlockedLevels)->toEqual(array<any>());
            std::async([=]() { progressionService->recordAgentNamed(std:("FullProgressionAgent")); });
            std::async([=]() { progressionService->recordCapabilityUsed(std:("shell")); });
            state = progressionService->getProgressionState();
            expect(state->currentLevel)->toBe(1);
            expect(state->unlockedLevels)->toContain(std:("level_1_browser"));
            expect(state->agentNamed)->toBe(true);
            expect(state->completedActions)->toContain(std:("capability_used_shell"));
            std::async([=]() { progressionService->recordCapabilityUsed(std:("browser")); });
            std::async([=]() { progressionService->recordFormSubmitted(); });
            state = progressionService->getProgressionState();
            expect(state->currentLevel)->toBe(2);
            expect(state->unlockedLevels)->toContain(std:("level_2_vision"));
            expect(state->completedActions)->toContain(std:("capability_used_browser"));
            expect(state->completedActions)->toContain(std:("form_submitted"));
            std::async([=]() { progressionService->recordCapabilityUsed(std:("vision")); });
            state = progressionService->getProgressionState();
            expect(state->currentLevel)->toBe(3);
            expect(state->unlockedLevels)->toContain(std:("level_3_audio"));
            std::async([=]() { progressionService->recordCapabilityUsed(std:("microphone")); });
            state = progressionService->getProgressionState();
            expect(state->currentLevel)->toBe(4);
            expect(state->unlockedLevels)->toContain(std:("level_4_camera"));
            auto allCapabilities = progressionService->getUnlockedCapabilities();
            expect(allCapabilities)->toContain(std:("shell"));
            expect(allCapabilities)->toContain(std:("naming"));
            expect(allCapabilities)->toContain(std:("browser"));
            expect(allCapabilities)->toContain(std:("stagehand"));
            expect(allCapabilities)->toContain(std:("vision"));
            expect(allCapabilities)->toContain(std:("screen_capture"));
            expect(allCapabilities)->toContain(std:("microphone"));
            expect(allCapabilities)->toContain(std:("sam"));
            expect(allCapabilities)->toContain(std:("audio"));
            expect(allCapabilities)->toContain(std:("camera"));
            expect(allCapabilities)->toContain(std:("advanced_vision"));
        }
        );
        it(std:("should handle duplicate action recordings gracefully"), [=]() mutable
        {
            expect(progressionService->getCurrentLevel())->toBe(0);
            std::async([=]() { progressionService->recordAgentNamed(std:("DuplicateTest")); });
            std::async([=]() { progressionService->recordAgentNamed(std:("DuplicateTest")); });
            std::async([=]() { progressionService->recordCapabilityUsed(std:("shell")); });
            std::async([=]() { progressionService->recordCapabilityUsed(std:("shell")); });
            expect(progressionService->getCurrentLevel())->toBe(1);
            auto state = progressionService->getProgressionState();
            auto shellActions = state->completedActions->filter([=](auto action) mutable
            {
                return action == std:("capability_used_shell");
            }
            );
            expect(shellActions->get_length())->toBe(1);
        }
        );
        it(std:("should provide correct available levels information"), [=]() mutable
        {
            auto availableLevels = progressionService->getAvailableLevels();
            expect(availableLevels->get_length())->toBe(5);
            expect(const_(availableLevels)[0]->id)->toBe(std:("level_0_basic"));
            expect(const_(availableLevels)[0]->isUnlocked)->toBe(true);
            expect(const_(availableLevels)[0]->unlockedCapabilities)->toEqual(array<string>{ std:("shell"), std:("naming") });
            expect(const_(availableLevels)[1]->id)->toBe(std:("level_1_browser"));
            expect(const_(availableLevels)[1]->isUnlocked)->toBe(false);
            expect(const_(availableLevels)[1]->unlockedCapabilities)->toEqual(array<string>{ std:("browser"), std:("stagehand") });
            expect(const_(availableLevels)[2]->id)->toBe(std:("level_2_vision"));
            expect(const_(availableLevels)[2]->isUnlocked)->toBe(false);
            expect(const_(availableLevels)[2]->unlockedCapabilities)->toEqual(array<string>{ std:("vision"), std:("screen_capture") });
            expect(const_(availableLevels)[3]->id)->toBe(std:("level_3_audio"));
            expect(const_(availableLevels)[3]->isUnlocked)->toBe(false);
            expect(const_(availableLevels)[3]->unlockedCapabilities)->toEqual(array<string>{ std:("microphone"), std:("sam"), std:("audio") });
            expect(const_(availableLevels)[4]->id)->toBe(std:("level_4_camera"));
            expect(const_(availableLevels)[4]->isUnlocked)->toBe(false);
            expect(const_(availableLevels)[4]->unlockedCapabilities)->toEqual(array<string>{ std:("camera"), std:("advanced_vision") });
        }
        );
        describe(std:("Unlocked Mode Tests"), [=]() mutable
        {
            it(std:("should start with all capabilities unlocked when PROGRESSION_MODE is set to unlocked"), [=]() mutable
            {
                runtime["getSetting"] = [=](auto key) mutable
                {
                    if (key == std:("PROGRESSION_MODE")) return std:("unlocked");
                    return undefined;
                };
                auto unlockedService = std::make_shared<CapabilityProgressionService>(runtime);
                expect(unlockedService->isUnlockedModeEnabled())->toBe(true);
                expect(unlockedService->getCurrentLevel())->toBe(5);
                auto unlockedCapabilities = unlockedService->getUnlockedCapabilities();
                expect(unlockedCapabilities)->toContain(std:("shell"));
                expect(unlockedCapabilities)->toContain(std:("browser"));
                expect(unlockedCapabilities)->toContain(std:("vision"));
                expect(unlockedCapabilities)->toContain(std:("microphone"));
                expect(unlockedCapabilities)->toContain(std:("camera"));
                auto availableLevels = unlockedService->getAvailableLevels();
                availableLevels->forEach([=](auto level) mutable
                {
                    expect(level->isUnlocked)->toBe(true);
                }
                );
            }
            );
            it(std:("should not track progression in unlocked mode"), [=]() mutable
            {
                runtime["getSetting"] = [=](auto key) mutable
                {
                    if (key == std:("PROGRESSION_MODE")) return std:("unlocked");
                    return undefined;
                };
                auto unlockedService = std::make_shared<CapabilityProgressionService>(runtime);
                std::async([=]() { unlockedService->recordCapabilityUsed(std:("shell")); });
                std::async([=]() { unlockedService->recordAgentNamed(std:("TestAgent")); });
                std::async([=]() { unlockedService->recordFormSubmitted(); });
                auto state = unlockedService->getProgressionState();
                expect(state->completedActions)->toEqual(array<any>());
            }
            );
            it(std:("should switch from progression to unlocked mode"), [=]() mutable
            {
                expect(progressionService->isUnlockedModeEnabled())->toBe(false);
                expect(progressionService->getCurrentLevel())->toBe(0);
                std::async([=]() { progressionService->setProgressionMode(std:("unlocked")); });
                expect(progressionService->isUnlockedModeEnabled())->toBe(true);
                expect(progressionService->getCurrentLevel())->toBe(5);
                expect(progressionService->getUnlockedCapabilities()->get_length())->toBeGreaterThan(10);
            }
            );
            it(std:("should switch from unlocked to progression mode and reset progress"), [=]() mutable
            {
                runtime["getSetting"] = [=](auto key) mutable
                {
                    if (key == std:("PROGRESSION_MODE")) return std:("unlocked");
                    return undefined;
                };
                auto unlockedService = std::make_shared<CapabilityProgressionService>(runtime);
                expect(unlockedService->isUnlockedModeEnabled())->toBe(true);
                std::async([=]() { unlockedService->setProgressionMode(std:("progression")); });
                expect(unlockedService->isUnlockedModeEnabled())->toBe(false);
                expect(unlockedService->getCurrentLevel())->toBe(0);
                expect(unlockedService->getUnlockedCapabilities())->toEqual(array<string>{ std:("shell"), std:("naming") });
                auto state = unlockedService->getProgressionState();
                expect(state->completedActions)->toEqual(array<any>());
                expect(state->agentNamed)->toBe(false);
            }
            );
        }
        );
    }
    );
}

MAIN
