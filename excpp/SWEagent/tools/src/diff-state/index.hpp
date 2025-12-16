#include ".registry.hpp"
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

/**
 * Diff state tool
 * Tracks git diff state in a JSON file
 * Converted from tools/diff_state/
 */

;
;
;
;
;

const rootDir = process.env.ROOT || process.env.TMPDIR || '/tmp';
const STATE_PATH = path.join(rootDir, 'state.json');
const PATCH_PATH = path.join(rootDir, 'model.patch');

struct State {
    std::optional<std::string> diff;
};


 catch {
      return {};
    }
  }
  return {};
}



);
    const patch = execSync(`git diff --cached`, { 
      cwd: repoRoot as string,
      encoding: 'utf-8',
      maxBuffer: 10 * 1024 * 1024 // 10MB buffer
    });
    
    // Save patch to file
    fs.writeFileSync(PATCH_PATH, patch);
    
    // Update state
    state.diff = patch.trim();
    saveState(state);
    
    console.log('Diff state updated successfully');
  } catch (error) {
    console.error('Error updating diff state:', error);
    clearDiff();
  }
}





// CLI if run directly
if (require.main === module) {
  program
    .name('diff-state')
    .description('Manage git diff state')
    .version('1.0.0');

  program
    .command('update')
    .description('Update the diff state with current git changes')
    .action(() => {
      updateDiffState();
    });

  program
    .command('clear')
    .description('Clear the diff state')
    .action(() => {
      clearDiff();
      console.log('Diff state cleared');
    });

  program
    .command('get')
    .description('Get the current diff')
    .action(() => {
      const diff = getDiff();
      console.log(diff);
    });

  program
    .command('state')
    .description('Run the state command (default action)')
    .action(() => {
      updateDiffState();
    });

  // Default action
  if (process.argv.length === 2) {
    updateDiffState();
  } else {
    program.parse(process.argv);
  }
}

{ updateDiffState, clearDiff, getDiff };

} // namespace elizaos
