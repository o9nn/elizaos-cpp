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
 * Submit tool
 * Submits the current changes as a patch
 * Converted from tools/submit/bin/submit
 */

;
;
;
;
;

const rootDir = process.env.ROOT || process.env.TMPDIR || '/tmp';
const PATCH_PATH = path.join(rootDir, 'model.patch');
const TEST_PATCH_PATH = path.join(rootDir, 'test.patch');

"`, {
            cwd: repoRoot as string,
            stdio: 'ignore'
          });
        } catch (error) {
          console.error('Warning: Failed to reverse test patch');
        }
      }
    }

    // Add all changes and create patch
    execSync('git add -A', { cwd: repoRoot as string, stdio: 'ignore' });
    const patch = execSync('git diff --cached', {
      cwd: repoRoot as string,
      encoding: 'utf-8',
      maxBuffer: 10 * 1024 * 1024 // 10MB buffer
    });

    // Write patch to file
    fs.writeFileSync(PATCH_PATH, patch);

    // Output submission markers
    console.log('<<SWE_AGENT_SUBMISSION>>');
    console.log(patch);
    console.log('<<SWE_AGENT_SUBMISSION>>');

  } catch (error) {
    console.error('Error creating submission:', error);
    process.exit(1);
  }
}

// CLI if run directly
if (require.main === module) {
  program
    .name('submit')
    .description('Submit the current changes as a patch')
    .version('1.0.0')
    .action(() => {
      submit();
    });

  // Default action if no arguments
  if (process.argv.length === 2) {
    submit();
  } else {
    program.parse(process.argv);
  }
}

{ submit };

} // namespace elizaos
