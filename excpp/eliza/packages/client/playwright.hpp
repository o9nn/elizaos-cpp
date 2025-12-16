#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Read environment variables from file.
 * https://github.com/motdotla/dotenv
 */

/**
 * See https://playwright.dev/docs/test-configuration.
 */
  /* Maximum time one test can run for */
  /* Fail the build on CI if you accidentally left test.only in the source code. */
  /* Enable retries in CI to reduce flakiness */
  /* Opt out of parallel tests on CI */
  /* Reporter to use. See https://playwright.dev/docs/test-reporters */
  /* Shared settings for all the projects below. See https://playwright.dev/docs/api/class-testoptions. */
    /* Collect trace when retrying the failed test. See https://playwright.dev/docs/trace-viewer */
    /* Capture screenshot on failure. See https://playwright.dev/docs/screenshots */
    /* Record video on failure. See https://playwright.dev/docs/videos */
    /* By default, we record the action logs in test artifacts. */
    /* Viewport size for tests */

  /* Configure projects for major browsers */
        /* Record videos for Chromium */
    /* Test against mobile viewports. */
    // {
    //   name: 'Mobile Chrome',
    //   use: { ...devices['Pixel 5'] },
    // },
    // {
    //   name: 'Mobile Safari',
    //   use: { ...devices['iPhone 12'] },
    // },

  /* Run your local dev server before starting the tests */

  /* Global setup/teardown files */
  // globalSetup: require.resolve('./global-setup'),
  // globalTeardown: require.resolve('./global-teardown'),

} // namespace elizaos
