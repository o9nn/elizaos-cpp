#include "cypress/support/tasks.hpp"
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

;
;
;
;
;

const execAsync = promisify(exec);

default defineConfig({
  viewportWidth: 1280,
  viewportHeight: 720,
  defaultCommandTimeout: 30000,
  requestTimeout: 30000,
  responseTimeout: 30000,
  pageLoadTimeout: 60000,
  retries: {
    runMode: 2,
    openMode: 0,
  },

  e2e: {
    // Support dynamic baseUrl from environment variables
    baseUrl:
      process.env.CYPRESS_FRONTEND_URL ||
      process.env.FRONTEND_URL ||
      `http://localhost:${process.env.FRONTEND_PORT || '5173'}`,
    supportFile: 'cypress/support/e2e.ts',
    specPattern: 'cypress/e2e/**/*.cy.{js,jsx,ts,tsx}',
    video: true,
    screenshotOnRunFailure: true,

    setupNodeEvents(on, config) {
      // Add all tasks from tasks.ts
      on('task', cypressTasks);

      // Additional inline tasks
      on('task', {
        async checkPortAvailable(port: number) {
          const checkCommand =
            process.platform === 'win32' ? `netstat -an | findstr :${port}` : `lsof -i :${port}`;

          try {
            await execAsync(checkCommand);
            return false; // Port is in use
          } catch {
            return true; // Port is available
          }
        },

        fileExists(filePath: string) {
          return existsSync(filePath);
        },
      });

      // Log configuration
      console.log('Cypress E2E Configuration:');
      console.log(`  Base URL: ${config.baseUrl}`);
      console.log(`  Backend URL: ${process.env.CYPRESS_BACKEND_URL || 'http://localhost:7777'}`);
      console.log(`  Test Mode: ${process.env.NODE_ENV === 'test'}`);

      return config;
    },
  },
});

} // namespace elizaos
