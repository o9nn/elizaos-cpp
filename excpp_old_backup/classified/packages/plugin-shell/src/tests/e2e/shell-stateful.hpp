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



class ShellStatefulE2ETestSuite implements TestSuite {
  name = 'plugin-shell-stateful-e2e';
  description = 'Tests for stateful shell operations and directory navigation';

  tests = [
    {
      name: 'Should maintain current working directory across commands',
      fn: async (runtime: IAgentRuntime) => {
        console.log('Testing CWD persistence across commands...');

        const shellService = runtime.getService<ShellService>('SHELL');
        if (!shellService) {
          throw new Error('Shell service not available');
        }

        const initialCwd = shellService.getCurrentWorkingDirectory();
        console.log(`  Initial CWD: ${initialCwd}`);

        // Navigate to parent directory
        const roomId = createUniqueUuid(runtime, 'test-room');
        const cdMessage: Memory = {
          id: createUniqueUuid(runtime, 'test-cd'),
          entityId: runtime.agentId,
          content: { text: 'cd ..' },
          agentId: runtime.agentId,
          roomId,
          createdAt: Date.now(),
        };

        const state: State = { values: {}, data: {}, text: '' };
        await runShellCommandAction.handler(
          runtime,
          cdMessage,
          state,
          {},
          async () => {
            return [];
          }
        );

        const newCwd = shellService.getCurrentWorkingDirectory();
        const expectedCwd = path.resolve(initialCwd, '..');

        if (newCwd !== expectedCwd) {
          throw new Error(
            `CWD not updated correctly. Expected: ${expectedCwd}, Got: ${newCwd}`
          );
        }

        console.log(`✓ CWD changed to: ${newCwd}`);

        // Verify PWD shows the new directory
        const pwdMessage: Memory = {
          id: createUniqueUuid(runtime, 'test-pwd'),
          entityId: runtime.agentId,
          content: { text: 'pwd' },
          agentId: runtime.agentId,
          roomId,
          createdAt: Date.now(),
        };

        let pwdResponse: any = null;
        await runShellCommandAction.handler(
          runtime,
          pwdMessage,
          state,
          {},
          async (resp) => {
            pwdResponse = resp;
            return [];
          }
        );

        const attachment = pwdResponse.attachments[0];
        const outputData = JSON.parse(attachment.text);

        if (!outputData.stdout.includes(newCwd)) {
          throw new Error(
            `PWD output doesn't match current directory: ${outputData.stdout}`
          );
        }

        // Navigate back
        await shellService.executeCommand(`cd ${initialCwd}`);
        console.log('✓ Successfully navigated back to initial directory');
      },
    },

        // Create a test directory structure

        // Create directories

          // Navigate through directories

          // Cleanup

        // Set an environment variable

        // Read the environment variable in the same session

        // Note: Environment variables set via in a child process
        // won't persist in the parent. This is a limitation of using execSync.
        // The test should acknowledge this limitation.

          // Create test directory and navigate to it

          // Create a file in the new directory

          // Verify file exists

          // Verify file path

          // Cleanup

          // Perform various file operations

          // Check file operation history

          // Verify operations were tracked
          // Cleanup if test fails


} // namespace elizaos
