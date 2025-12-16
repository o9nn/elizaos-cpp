#include "..service.hpp"
#include ".test-utils.hpp"
#include "shell-advanced.hpp"
#include "shell-basic.hpp"
#include "shell-security.hpp"
#include "shell-stateful.hpp"
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

#!/usr/bin/env node
;
;
;
;
;
;

// Simple test runner for local e2e testing
async 
    return null;
  };

  const testSuites = [
    shellBasicE2ETests,
    shellStatefulE2ETests,
    shellAdvancedE2ETests,
    shellSecurityE2ETests,
  ];

  let totalTests = 0;
  let passedTests = 0;
  let failedTests = 0;

  for (const suite of testSuites) {
    console.log(`\n📦 Running suite: ${suite.name}`);
    console.log(`   ${suite.description}\n`);

    for (const test of suite.tests) {
      totalTests++;
      process.stdout.write(`   🔄 ${test.name}... `);

      try {
        await test.fn(runtime);
        passedTests++;
        console.log('✅ PASSED');
      } catch (error) {
        failedTests++;
        console.log('❌ FAILED');
        console.error(
          `      Error: ${error instanceof Error ? error.message : String(error)}`
        );
      }
    }
  }

  // Summary
  console.log(`\n${'='.repeat(60)}`);
  console.log('📊 Test Summary:');
  console.log(`   Total:  ${totalTests} tests`);
  console.log(`   ✅ Passed: ${passedTests} tests`);
  console.log(`   ❌ Failed: ${failedTests} tests`);
  console.log(`${'='.repeat(60)}\n`);

  // Cleanup
  await shellService.stop();

  process.exit(failedTests > 0 ? 1 : 0);
}

// Run tests
runE2ETests().catch((error) => {
  console.error('Fatal error:', error);
  process.exit(1);
});

} // namespace elizaos
