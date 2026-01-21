#include "..action.hpp"
#include "..service.hpp"
#include "elizaos/core.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class ShellBasicE2ETestSuite implements TestSuite {
  name = 'plugin-shell-basic-e2e';
  description = 'Basic end-to-end tests for shell plugin functionality';

  tests = [
    {
      name: 'Should execute simple echo command',
      fn: async (runtime: IAgentRuntime) => {
        console.log('Testing simple echo command execution...');

        // Create a test message
        const roomId = createUniqueUuid(runtime, 'test-room');
        const message: Memory = {
          id: createUniqueUuid(runtime, 'test-msg-1'),
          entityId: runtime.agentId,
          content: { text: 'echo "Hello from shell plugin"' },
          agentId: runtime.agentId,
          roomId,
          createdAt: Date.now(),
        };

        let callbackCalled = false;
        let callbackResponse: any = null;

        const state: State = { values: {}, data: {}, text: '' };
        await runShellCommandAction.handler(
          runtime,
          message,
          state,
          {},
          async (response) => {
            callbackCalled = true;
            callbackResponse = response;
            return [];
          }
        );

        if (!callbackCalled) {
          throw new Error('Callback was not called');
        }

        if (!callbackResponse.text.includes('Hello from shell plugin')) {
          throw new Error(`Unexpected output: ${callbackResponse.text}`);
        }

        console.log('✓ Echo command executed successfully');
        console.log(`  Output: ${callbackResponse.text}`);
      },
    },

        // Check if a listing command was executed (ls, dir, etc.)

        // Execute some commands first

        // Verify history exists

        // Clear history

        // Verify history is cleared


} // namespace elizaos
