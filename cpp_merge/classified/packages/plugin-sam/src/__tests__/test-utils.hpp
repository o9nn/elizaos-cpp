#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_SAM_SRC___TESTS___TEST_UTILS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_SAM_SRC___TESTS___TEST_UTILS_H
#include "core.hpp"
#include "bun:test.h"
// External dependency removed

std::shared_ptr<IAgentRuntime> createMockRuntime(Partial<std::shared_ptr<IAgentRuntime>> overrides = object{});

void setupLoggerSpies();

std::shared_ptr<Memory> createMockMemory(Partial<std::shared_ptr<Memory>> overrides = object{});

std::shared_ptr<State> createMockState(Partial<std::shared_ptr<State>> overrides = object{});

std::shared_ptr<Uint8Array> createMockAudioBuffer(double length = 1000);

std::shared_ptr<Uint8Array> createMockWAVBuffer(std::shared_ptr<Uint8Array> audioData, double sampleRate = 22050);

#endif
