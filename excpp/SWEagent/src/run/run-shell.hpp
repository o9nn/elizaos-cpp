#include ".agent/agents.hpp"
#include ".agent/hooks.hpp"
#include ".agent/problem-statement.hpp"
#include ".environment/swe-env.hpp"
#include ".utils/log.hpp"
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
 * Run shell mode
 * Converted from sweagent/run/run_shell.py
 */

;
;
;
;
;
;

/**
 * Run shell - interactive shell mode
 */
class RunShell {
  private env: SWEEnv;
  private agent: AbstractAgent;
  private problemStatement: ProblemStatement | ProblemStatementConfig;
  private outputDir: string;
  private hooks: AbstractAgentHook[];
  private logger: AgentLogger;
  private rl: readline.Interface;

  constructor(config: {
    env: SWEEnv;
    agent: AbstractAgent;
    problemStatement: ProblemStatement | ProblemStatementConfig;
    outputDir?: string;
    hooks?: AbstractAgentHook[];
  }) {
    this.env = config.env;
    this.agent = config.agent;
    this.problemStatement = config.problemStatement;
    this.outputDir = config.outputDir || '.';
    this.hooks = config.hooks || [];
    this.logger = getLogger('run-shell', '🐚');

    // Setup readline interface
    this.rl = readline.createInterface({
      input: process.stdin,
      output: process.stdout,
      prompt: '> ',
    });

    // Add hooks to agent
    for (const hook of this.hooks) {
      this.agent.addHook(hook);
    }
  }

  addHook(hook: AbstractAgentHook): void {
    this.hooks.push(hook);
    this.agent.addHook(hook);
  }

  async run(): Promise<void> {
    this.logger.info('Starting shell mode');
    this.logger.info('Type commands to execute, or "exit" to quit');

    // Start environment
    await this.env.start();

    try {
      // Setup agent (DefaultAgent has setup method)
      if (this.agent instanceof DefaultAgent) {
        await this.agent.setup(this.env, this.problemStatement, this.outputDir);
      }

      // Interactive loop
      await this.interactiveLoop();
    } finally {
      // Clean up
      this.rl.close();
      await this.env.close();
    }
  }

  private async interactiveLoop(): Promise<void> {
    return new Promise((resolve) => {
      this.rl.prompt();

      this.rl.on('line', async (input) => {
        const command = input.trim();

        if (command === 'exit' || command === 'quit') {
          this.logger.info('Exiting shell mode');
          resolve();
          return;
        }

        if (command === 'help') {
          this.printHelp();
          this.rl.prompt();
          return;
        }

        if (command === 'status') {
          await this.printStatus();
          this.rl.prompt();
          return;
        }

        if (command === 'save') {
          this.saveState();
          this.rl.prompt();
          return;
        }

        if (command) {
          try {
            // Execute command through environment
            const result = await this.env.communicate(command);
            console.log(result);
          } catch (error) {
            this.logger.error(`Error executing command: ${error}`);
          }
        }

        this.rl.prompt();
      });

      this.rl.on('close', () => {
        resolve();
      });
    });
  }

  private printHelp(): void {
    console.log('\nShell Mode Commands:');
    console.log('  exit/quit - Exit shell mode');
    console.log('  help      - Show this help message');
    console.log('  status    - Show current environment status');
    console.log('  save      - Save current state');
    console.log('\nOr enter any bash command to execute in the environment\n');
  }

  private async printStatus(): Promise<void> {
    console.log('\nEnvironment Status:');
    console.log(`  Name: ${this.env.name}`);

    if (this.env.repo) {
      console.log(`  Repository: ${this.env.repo.repoName}`);
      console.log(`  Base commit: ${this.env.repo.baseCommit}`);
    }

    // Get current working directory
    try {
      const pwd = await this.env.communicate('pwd');
      console.log(`  Current directory: ${pwd.trim()}`);
    } catch {
      // Ignore errors
    }

    console.log('');
  }

  private saveState(): void {
    try {
      if (this.agent instanceof DefaultAgent) {
        this.agent.saveTrajectory();
        this.logger.info('State saved to trajectory file');
      } else {
        this.logger.warning('Cannot save state for this agent type');
      }
    } catch (error) {
      this.logger.error(`Error saving state: ${error}`);
    }
  }
}

/**
 * Run a shell session from configuration options
 * @param _options - Configuration options for the shell session
 * @param _options.repo - Repository URL or path
 * @param _options.config - Configuration file path
 * @param _options.problemStatement - Problem statement to solve
 */
std::future<void> runShellFromConfig(std::optional<{
  repo: string;
  config: string;
  problemStatement: string;
}> _options);

} // namespace elizaos
