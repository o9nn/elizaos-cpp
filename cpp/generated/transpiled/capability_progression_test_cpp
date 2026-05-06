#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/agentserver/src/__tests__/capability-progression.test.h"

void Main(void)
{
    describe(std::string("Capability Progression System E2E Tests"), [=]() mutable
    {
        shared<any> runtime;
        shared<std::shared_ptr<CapabilityProgressionService>> progressionService;
        beforeEach([=]() mutable
        {
            auto character = object{
                object::pair{std::string("id"), stringToUuid(std::string("TestAgent"))}, 
                object::pair{std::string("name"), std::string("TestAgent")}, 
                object::pair{std::string("system"), std::string("You are a test agent for capability progression.")}, 
                object::pair{std::string("bio"), array<string>{ std::string("Test agent for progression system") }}, 
                object::pair{std::string("messageExamples"), array<any>()}, 
                object::pair{std::string("postExamples"), array<any>()}, 
                object::pair{std::string("topics"), array<any>()}, 
                object::pair{std::string("knowledge"), array<any>()}, 
                object::pair{std::string("style"), object{
                    object::pair{std::string("all"), array<any>()}, 
                    object::pair{std::string("chat"), array<any>()}, 
                    object::pair{std::string("post"), array<any>()}
                }}, 
                object::pair{std::string("adjectives"), array<any>()}, 
                object::pair{std::string("settings"), object{
                    object::pair{std::string("MODEL_PROVIDER"), std::string("test")}, 
                    object::pair{std::string("ENABLE_AUTONOMY"), std::string("false")}
                }}
            };
            runtime = std::make_shared<AgentRuntime>(object{
                object::pair{std::string("character"), std::string("character")}, 
                object::pair{std::string("agentId"), stringToUuid(std::string("TestAgent"))}, 
                object::pair{std::string("plugins"), array<any>()}
            });
            runtime["setSetting"] = [=]() mutable
            {
            };
            progressionService = std::make_shared<CapabilityProgressionService>(runtime);
        }
        );
        it(std::string("should start with only basic capabilities unlocked"), [=]() mutable
        {
            auto currentLevel = progressionService->getCurrentLevel();
            auto unlockedCapabilities = progressionService->getUnlockedCapabilities();
            auto progressionState = progressionService->getProgressionState();
            expect(currentLevel)->toBe(0);
            expect(unlockedCapabilities)->toEqual(array<string>{ std::string("shell"), std::string("naming") });
            expect(progressionState->agentNamed)->toBe(false);
            expect(progressionState->completedActions)->toEqual(array<any>());
            expect(progressionState->unlockedLevels)->toEqual(array<any>());
        }
        );
        it(std::string("should not unlock browser capabilities until agent is named AND shell is used"), [=]() mutable
        {
            auto initialLevel = progressionService->getCurrentLevel();
            expect(initialLevel)->toBe(0);
            std::async([=]() { progressionService->recordAgentNamed(std::string("TestBot")); });
            expect(progressionService->getCurrentLevel())->toBe(0);
            expect(progressionService->isCapabilityUnlocked(std::string("browser")))->toBe(false);
            expect(progressionService->isCapabilityUnlocked(std::string("stagehand")))->toBe(false);
            progressionService = std::make_shared<CapabilityProgressionService>(runtime);
            std::async([=]() { progressionService->recordCapabilityUsed(std::string("shell")); });
            expect(progressionService->getCurrentLevel())->toBe(0);
            expect(progressionService->isCapabilityUnlocked(std::string("browser")))->toBe(false);
        }
        );
        it(std::string("should unlock browser capabilities when both naming and shell conditions are met"), [=]() mutable
        {
            expect(progressionService->getCurrentLevel())->toBe(0);
            std::async([=]() { progressionService->recordAgentNamed(std::string("WebExplorer")); });
            std::async([=]() { progressionService->recordCapabilityUsed(std::string("shell")); });
            expect(progressionService->getCurrentLevel())->toBe(1);
            expect(progressionService->isCapabilityUnlocked(std::string("browser")))->toBe(true);
            expect(progressionService->isCapabilityUnlocked(std::string("stagehand")))->toBe(true);
            expect(progressionService->isCapabilityUnlocked(std::string("shell")))->toBe(true);
            expect(progressionService->isCapabilityUnlocked(std::string("naming")))->toBe(true);
            auto progressionState = progressionService->getProgressionState();
            expect(progressionState->unlockedLevels)->toContain(std::string("level_1_browser"));
            expect(progressionState->lastUnlockTime)->not->toBeNull();
        }
        );
        it(std::string("should unlock vision capabilities when browser is used AND form is submitted"), [=]() mutable
        {
            std::async([=]() { progressionService->recordAgentNamed(std::string("VisualAgent")); });
            std::async([=]() { progressionService->recordCapabilityUsed(std::string("shell")); });
            expect(progressionService->getCurrentLevel())->toBe(1);
            std::async([=]() { progressionService->recordCapabilityUsed(std::string("browser")); });
            expect(progressionService->getCurrentLevel())->toBe(1);
            expect(progressionService->isCapabilityUnlocked(std::string("vision")))->toBe(false);
            std::async([=]() { progressionService->recordFormSubmitted(object{
                object::pair{std::string("data"), std::string("test form data")}
            }); });
            expect(progressionService->getCurrentLevel())->toBe(2);
            expect(progressionService->isCapabilityUnlocked(std::string("vision")))->toBe(true);
            expect(progressionService->isCapabilityUnlocked(std::string("screen_capture")))->toBe(true);
            expect(progressionService->isCapabilityUnlocked(std::string("browser")))->toBe(true);
            expect(progressionService->isCapabilityUnlocked(std::string("shell")))->toBe(true);
        }
        );
        it(std::string("should unlock audio capabilities when vision is used"), [=]() mutable
        {
            std::async([=]() { progressionService->recordAgentNamed(std::string("AudioAgent")); });
            std::async([=]() { progressionService->recordCapabilityUsed(std::string("shell")); });
            std::async([=]() { progressionService->recordCapabilityUsed(std::string("browser")); });
            std::async([=]() { progressionService->recordFormSubmitted(); });
            expect(progressionService->getCurrentLevel())->toBe(2);
            std::async([=]() { progressionService->recordCapabilityUsed(std::string("vision")); });
            expect(progressionService->getCurrentLevel())->toBe(3);
            expect(progressionService->isCapabilityUnlocked(std::string("microphone")))->toBe(true);
            expect(progressionService->isCapabilityUnlocked(std::string("sam")))->toBe(true);
            expect(progressionService->isCapabilityUnlocked(std::string("audio")))->toBe(true);
        }
        );
        it(std::string("should unlock camera capabilities when microphone is used"), [=]() mutable
        {
            std::async([=]() { progressionService->recordAgentNamed(std::string("CameraAgent")); });
            std::async([=]() { progressionService->recordCapabilityUsed(std::string("shell")); });
            std::async([=]() { progressionService->recordCapabilityUsed(std::string("browser")); });
            std::async([=]() { progressionService->recordFormSubmitted(); });
            std::async([=]() { progressionService->recordCapabilityUsed(std::string("vision")); });
            expect(progressionService->getCurrentLevel())->toBe(3);
            std::async([=]() { progressionService->recordCapabilityUsed(std::string("microphone")); });
            expect(progressionService->getCurrentLevel())->toBe(4);
            expect(progressionService->isCapabilityUnlocked(std::string("camera")))->toBe(true);
            expect(progressionService->isCapabilityUnlocked(std::string("advanced_vision")))->toBe(true);
        }
        );
        it(std::string("should not allow skipping levels in progression"), [=]() mutable
        {
            expect(progressionService->getCurrentLevel())->toBe(0);
            std::async([=]() { progressionService->recordCapabilityUsed(std::string("vision")); });
            std::async([=]() { progressionService->recordCapabilityUsed(std::string("microphone")); });
            std::async([=]() { progressionService->recordFormSubmitted(); });
            expect(progressionService->getCurrentLevel())->toBe(0);
            expect(progressionService->isCapabilityUnlocked(std::string("vision")))->toBe(false);
            expect(progressionService->isCapabilityUnlocked(std::string("microphone")))->toBe(false);
            expect(progressionService->isCapabilityUnlocked(std::string("browser")))->toBe(false);
        }
        );
        it(std::string("should track progression state correctly through all levels"), [=]() mutable
        {
            auto state = progressionService->getProgressionState();
            expect(state->currentLevel)->toBe(0);
            expect(state->unlockedLevels)->toEqual(array<any>());
            std::async([=]() { progressionService->recordAgentNamed(std::string("FullProgressionAgent")); });
            std::async([=]() { progressionService->recordCapabilityUsed(std::string("shell")); });
            state = progressionService->getProgressionState();
            expect(state->currentLevel)->toBe(1);
            expect(state->unlockedLevels)->toContain(std::string("level_1_browser"));
            expect(state->agentNamed)->toBe(true);
            expect(state->completedActions)->toContain(std::string("capability_used_shell"));
            std::async([=]() { progressionService->recordCapabilityUsed(std::string("browser")); });
            std::async([=]() { progressionService->recordFormSubmitted(); });
            state = progressionService->getProgressionState();
            expect(state->currentLevel)->toBe(2);
            expect(state->unlockedLevels)->toContain(std::string("level_2_vision"));
            expect(state->completedActions)->toContain(std::string("capability_used_browser"));
            expect(state->completedActions)->toContain(std::string("form_submitted"));
            std::async([=]() { progressionService->recordCapabilityUsed(std::string("vision")); });
            state = progressionService->getProgressionState();
            expect(state->currentLevel)->toBe(3);
            expect(state->unlockedLevels)->toContain(std::string("level_3_audio"));
            std::async([=]() { progressionService->recordCapabilityUsed(std::string("microphone")); });
            state = progressionService->getProgressionState();
            expect(state->currentLevel)->toBe(4);
            expect(state->unlockedLevels)->toContain(std::string("level_4_camera"));
            auto allCapabilities = progressionService->getUnlockedCapabilities();
            expect(allCapabilities)->toContain(std::string("shell"));
            expect(allCapabilities)->toContain(std::string("naming"));
            expect(allCapabilities)->toContain(std::string("browser"));
            expect(allCapabilities)->toContain(std::string("stagehand"));
            expect(allCapabilities)->toContain(std::string("vision"));
            expect(allCapabilities)->toContain(std::string("screen_capture"));
            expect(allCapabilities)->toContain(std::string("microphone"));
            expect(allCapabilities)->toContain(std::string("sam"));
            expect(allCapabilities)->toContain(std::string("audio"));
            expect(allCapabilities)->toContain(std::string("camera"));
            expect(allCapabilities)->toContain(std::string("advanced_vision"));
        }
        );
        it(std::string("should handle duplicate action recordings gracefully"), [=]() mutable
        {
            expect(progressionService->getCurrentLevel())->toBe(0);
            std::async([=]() { progressionService->recordAgentNamed(std::string("DuplicateTest")); });
            std::async([=]() { progressionService->recordAgentNamed(std::string("DuplicateTest")); });
            std::async([=]() { progressionService->recordCapabilityUsed(std::string("shell")); });
            std::async([=]() { progressionService->recordCapabilityUsed(std::string("shell")); });
            expect(progressionService->getCurrentLevel())->toBe(1);
            auto state = progressionService->getProgressionState();
            auto shellActions = state->completedActions->filter([=](auto action) mutable
            {
                return action == std::string("capability_used_shell");
            }
            );
            expect(shellActions->get_length())->toBe(1);
        }
        );
        it(std::string("should provide correct available levels information"), [=]() mutable
        {
            auto availableLevels = progressionService->getAvailableLevels();
            expect(availableLevels->get_length())->toBe(5);
            expect(const_(availableLevels)[0]->id)->toBe(std::string("level_0_basic"));
            expect(const_(availableLevels)[0]->isUnlocked)->toBe(true);
            expect(const_(availableLevels)[0]->unlockedCapabilities)->toEqual(array<string>{ std::string("shell"), std::string("naming") });
            expect(const_(availableLevels)[1]->id)->toBe(std::string("level_1_browser"));
            expect(const_(availableLevels)[1]->isUnlocked)->toBe(false);
            expect(const_(availableLevels)[1]->unlockedCapabilities)->toEqual(array<string>{ std::string("browser"), std::string("stagehand") });
            expect(const_(availableLevels)[2]->id)->toBe(std::string("level_2_vision"));
            expect(const_(availableLevels)[2]->isUnlocked)->toBe(false);
            expect(const_(availableLevels)[2]->unlockedCapabilities)->toEqual(array<string>{ std::string("vision"), std::string("screen_capture") });
            expect(const_(availableLevels)[3]->id)->toBe(std::string("level_3_audio"));
            expect(const_(availableLevels)[3]->isUnlocked)->toBe(false);
            expect(const_(availableLevels)[3]->unlockedCapabilities)->toEqual(array<string>{ std::string("microphone"), std::string("sam"), std::string("audio") });
            expect(const_(availableLevels)[4]->id)->toBe(std::string("level_4_camera"));
            expect(const_(availableLevels)[4]->isUnlocked)->toBe(false);
            expect(const_(availableLevels)[4]->unlockedCapabilities)->toEqual(array<string>{ std::string("camera"), std::string("advanced_vision") });
        }
        );
        describe(std::string("Unlocked Mode Tests"), [=]() mutable
        {
            it(std::string("should start with all capabilities unlocked when PROGRESSION_MODE is set to unlocked"), [=]() mutable
            {
                runtime["getSetting"] = [=](auto key) mutable
                {
                    if (key == std::string("PROGRESSION_MODE")) return std::string("unlocked");
                    return undefined;
                };
                auto unlockedService = std::make_shared<CapabilityProgressionService>(runtime);
                expect(unlockedService->isUnlockedModeEnabled())->toBe(true);
                expect(unlockedService->getCurrentLevel())->toBe(5);
                auto unlockedCapabilities = unlockedService->getUnlockedCapabilities();
                expect(unlockedCapabilities)->toContain(std::string("shell"));
                expect(unlockedCapabilities)->toContain(std::string("browser"));
                expect(unlockedCapabilities)->toContain(std::string("vision"));
                expect(unlockedCapabilities)->toContain(std::string("microphone"));
                expect(unlockedCapabilities)->toContain(std::string("camera"));
                auto availableLevels = unlockedService->getAvailableLevels();
                availableLevels->forEach([=](auto level) mutable
                {
                    expect(level->isUnlocked)->toBe(true);
                }
                );
            }
            );
            it(std::string("should not track progression in unlocked mode"), [=]() mutable
            {
                runtime["getSetting"] = [=](auto key) mutable
                {
                    if (key == std::string("PROGRESSION_MODE")) return std::string("unlocked");
                    return undefined;
                };
                auto unlockedService = std::make_shared<CapabilityProgressionService>(runtime);
                std::async([=]() { unlockedService->recordCapabilityUsed(std::string("shell")); });
                std::async([=]() { unlockedService->recordAgentNamed(std::string("TestAgent")); });
                std::async([=]() { unlockedService->recordFormSubmitted(); });
                auto state = unlockedService->getProgressionState();
                expect(state->completedActions)->toEqual(array<any>());
            }
            );
            it(std::string("should switch from progression to unlocked mode"), [=]() mutable
            {
                expect(progressionService->isUnlockedModeEnabled())->toBe(false);
                expect(progressionService->getCurrentLevel())->toBe(0);
                std::async([=]() { progressionService->setProgressionMode(std::string("unlocked")); });
                expect(progressionService->isUnlockedModeEnabled())->toBe(true);
                expect(progressionService->getCurrentLevel())->toBe(5);
                expect(progressionService->getUnlockedCapabilities()->get_length())->toBeGreaterThan(10);
            }
            );
            it(std::string("should switch from unlocked to progression mode and reset progress"), [=]() mutable
            {
                runtime["getSetting"] = [=](auto key) mutable
                {
                    if (key == std::string("PROGRESSION_MODE")) return std::string("unlocked");
                    return undefined;
                };
                auto unlockedService = std::make_shared<CapabilityProgressionService>(runtime);
                expect(unlockedService->isUnlockedModeEnabled())->toBe(true);
                std::async([=]() { unlockedService->setProgressionMode(std::string("progression")); });
                expect(unlockedService->isUnlockedModeEnabled())->toBe(false);
                expect(unlockedService->getCurrentLevel())->toBe(0);
                expect(unlockedService->getUnlockedCapabilities())->toEqual(array<string>{ std::string("shell"), std::string("naming") });
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
