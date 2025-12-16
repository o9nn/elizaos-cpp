#include ".utils/files.hpp"
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
 * Terminal-based trajectory inspector
 * This is a simplified version - a full implementation would use a terminal UI library
 */

;
;
;
;
;

struct TrajectoryStep {
    std::optional<std::string> thought;
    std::optional<std::string> action;
    std::optional<std::string> observation;
    std::optional<std::string> response;
    std::optional<double> execution_time;
    std::optional<std::unordered_map<std::string, unknown>> state;
    std::optional<std::vector<{ role: string; content: string; messageType?: string }>> query;
    std::optional<std::unordered_map<std::string, unknown>> extraInfo;
};


struct TrajectoryData {
    std::vector<TrajectoryStep> trajectory;
    std::unordered_map<std::string, unknown> info;
    std::optional<std::vector<{ role: string; content: string | Record<string, unknown>; [key: string]: unknown }>> history;
    std::optional<std::unordered_map<std::string, unknown>> replay_config;
};


/**
 * Inspector CLI class
 */
class TrajectoryInspector {
  private trajData: TrajectoryData;
  private currentStep: number = -1;
  private showFull: boolean = false;
  // private trajPath: string;  // Currently unused
  private goldPatch?: string;

  constructor(trajPath: string, dataPath?: string) {
    // this.trajPath = trajPath;  // Currently unused

    // Load trajectory
    const content = fs.readFileSync(trajPath, 'utf-8');
    if (trajPath.endsWith('.yaml') || trajPath.endsWith('.yml')) {
      this.trajData = yaml.load(content) as TrajectoryData;
    } else {
      this.trajData = JSON.parse(content);
    }

    // Load gold patch if available
    if (dataPath) {
      const data = loadFile(dataPath) as Record<string, { patch?: string }> | null;
      const instanceId = path.basename(path.dirname(trajPath));
      this.goldPatch = data?.[instanceId]?.patch;
    }
  }

  get nSteps(): number {
    return this.trajData.trajectory?.length || 0;
  }

  private showStep(): void {
    console.clear();

    if (this.currentStep < 0 || this.currentStep >= this.nSteps) {
      this.showInfo();
      return;
    }

    const step = this.trajData.trajectory[this.currentStep];

    console.log(`=== Step ${this.currentStep + 1}/${this.nSteps} ===\n`);

    if (this.showFull) {
      // Show full YAML
      console.log(yaml.dump(step, { lineWidth: -1 }));
    } else {
      // Simplified view
      if (step.thought) {
        console.log('THOUGHT:');
        console.log(step.thought);
        console.log();
      }

      if (step.action) {
        console.log('ACTION:');
        console.log(step.action);
        console.log();
      }

      if (step.observation) {
        console.log('OBSERVATION:');
        console.log(step.observation);
      }
    }

    this.showHelp();
  }

  private showInfo(): void {
    console.clear();
    console.log('=== Trajectory Info ===\n');

    const info = { ...this.trajData.info };
    if (this.goldPatch) {
      info.gold_patch = this.goldPatch;
    }

    console.log(yaml.dump(info, { lineWidth: -1 }));

    this.showHelp();
  }

  private showHelp(): void {
    console.log('\n' + '='.repeat(80));
    console.log('Commands:');
    console.log('  l/right  - Next step');
    console.log('  h/left   - Previous step');
    console.log('  0        - Go to first step');
    console.log('  $        - Go to last step');
    console.log('  v        - Toggle view (simple/full)');
    console.log('  i        - Show info');
    console.log('  q        - Quit');
    console.log('='.repeat(80));
  }

  async run(): Promise<void> {
    // Set up readline interface
    const rl = readline.createInterface({
      input: process.stdin,
      output: process.stdout,
    });

    // Enable raw mode for single key press
    if (process.stdin.isTTY) {
      process.stdin.setRawMode(true);
    }

    readline.emitKeypressEvents(process.stdin, rl);

    // Show initial view
    this.showStep();

    // Handle keypress events
    process.stdin.on('keypress', (_str, key) => {
      if (!key) {
        return;
      }

      switch (key.name) {
        case 'q':
        case 'escape':
          rl.close();
          if (process.stdin.isTTY) {
            process.stdin.setRawMode(false);
          }
          process.exit(0);
          break;

        case 'l':
        case 'right':
          if (this.currentStep < this.nSteps) {
            this.currentStep++;
            this.showStep();
          }
          break;

        case 'h':
        case 'left':
          if (this.currentStep > -1) {
            this.currentStep--;
            this.showStep();
          }
          break;

        case '0':
          this.currentStep = 0;
          this.showStep();
          break;

        case '$':
          this.currentStep = this.nSteps - 1;
          this.showStep();
          break;

        case 'v':
          this.showFull = !this.showFull;
          this.showStep();
          break;

        case 'i':
          this.currentStep = -1;
          this.showStep();
          break;
      }
    });

    // Keep the process running
    await new Promise(() => {});
  }
}

/**
 * Find trajectory files in a directory
 */
 else if (file.endsWith('.traj')) {
        results.push(filePath);
      }
    }
  }

  if (fs.statSync(dir).isDirectory()) {
    walk(dir);
  } else {
    results.push(dir);
  }

  return results;
}

/**
 * Main 

  if (trajFiles.length === 1) {
    // Single file - open directly
    const inspector = new TrajectoryInspector(trajFiles[0], dataPath);
    await inspector.run();
  } else {
    // Multiple files - show selection menu
    console.log('Multiple trajectory files found:');
    trajFiles.forEach((file, index) => {
      console.log(`  ${index + 1}. ${file}`);
    });

    const rl = readline.createInterface({
      input: process.stdin,
      output: process.stdout,
    });

    const answer = await new Promise<string>((resolve) => {
      rl.question('\nSelect file number (or q to quit): ', resolve);
    });

    rl.close();

    if (answer === 'q') {
      process.exit(0);
    }

    const index = parseInt(answer, 10) - 1;
    if (index >= 0 && index < trajFiles.length) {
      const inspector = new TrajectoryInspector(trajFiles[index], dataPath);
      await inspector.run();
    } else {
      console.log('Invalid selection');
      process.exit(1);
    }
  }
}

} // namespace elizaos
