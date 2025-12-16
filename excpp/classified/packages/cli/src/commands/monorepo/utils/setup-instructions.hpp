#include ".types.hpp"
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

/**
 * Get platform-specific Bun installation instructions
 */
;
  } else {
    const commands = ['curl -fsSL https://bun.sh/install | bash'];
    const alternatives: string[] = [];

    if (platform === 'darwin') {
      alternatives.push('brew install bun (if you have Homebrew)');
    }

    return {
      platform: platform === 'darwin' ? 'macOS' : 'Linux',
      commands,
      alternatives: alternatives.length > 0 ? alternatives : undefined,
    };
  }
}

/**
 * Display the basic setup steps after cloning
 */
`);

  // Step 2: Install dependencies
  console.log('\n2. Install dependencies:');
  console.log('   bun install');

  // Step 3: Build the project
  console.log('\n3. Build the project:');
  console.log('   bun run build');

  // Step 4: Start ElizaOS
  console.log('\n4. Start ElizaOS:');
  console.log('   bun run start or bun run dev');
}

/**
 * Display prerequisites and Bun installation guidance
 */
`);
  console.log(`   ${emoji.bullet('Node.js 23.3.0+')}`);
  console.log(`   ${emoji.bullet('Bun (JavaScript runtime & package manager)')}`);
}

/**
 * Display Bun installation instructions for the current platform
 */
`);

  const instructions = getBunInstallInstructions();

  // Display primary installation commands
  instructions.commands.forEach((command) => {
    console.log(`   ${command}`);
  });

  // Display alternatives if any
  if (instructions.alternatives) {
    instructions.alternatives.forEach((alt) => {
      console.log(`   Alternative: ${alt}`);
    });
  }

  console.log('   More options: https://bun.sh/docs/installation');
  console.log('   After installation, restart your terminal');
}

/**
 * Display complete next step instructions after cloning
 *
 * Shows setup steps, prerequisites, and platform-specific installation guidance.
 */


} // namespace elizaos
