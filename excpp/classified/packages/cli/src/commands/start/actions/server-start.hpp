#include "agent-start.hpp"
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

;
;
;
;
;
;
;
;
;

/**
 * Server start options
 */
struct ServerStartOptions {
    std::optional<bool> configure;
    std::optional<double> port;
    std::optional<std::vector<Character>> characters;
    std::optional<std::vector<ProjectAgent>> projectAgents;
};


/**
 * Start the agents and server
 *
 * Initializes the database, creates the server instance, configures port settings, and starts the specified agents or default Eliza character.
 */
std::future<void> startAgents(ServerStartOptions options); = serverModule;

  // Get the directory where the CLI is installed to find client files
  const __filename = fileURLToPath(import.meta.url);
  const __dirname = path.dirname(__filename);

  // Calculate the CLI dist path more reliably
  // In development/monorepo: packages/cli/dist/commands/start/actions -> packages/cli/dist
  // In production/global: node_modules/@elizaos/cli/dist/commands/start/actions -> node_modules/@elizaos/cli/dist
  let cliDistPath = path.resolve(__dirname, '../../../');

  // Verify the path contains index.html, if not try alternative resolution
  const indexPath = path.join(cliDistPath, 'index.html');
  if (!existsSync(indexPath)) {
    // Try to find the dist directory by looking for package.json and then dist
    let currentDir = __dirname;
    while (currentDir !== path.dirname(currentDir)) {
      const packageJsonPath = path.join(currentDir, 'package.json');
      if (existsSync(packageJsonPath)) {
        try {
          const packageJson = JSON.parse(readFileSync(packageJsonPath, 'utf-8'));
          if (packageJson.name === '@elizaos/cli') {
            const distPath = path.join(currentDir, 'dist');
            if (existsSync(path.join(distPath, 'index.html'))) {
              cliDistPath = distPath;
              break;
            }
          }
        } catch {
          // Ignore parse errors
        }
      }
      currentDir = path.dirname(currentDir);
    }
  }

  const server = new AgentServer();
  await server.initialize({
    dataDir: pgliteDataDir,
    postgresUrl: postgresUrl || undefined,
  });

  server.startAgent = (character) => startAgent(character, server);
  server.stopAgent = (runtime) => stopAgent(runtime, server);
  server.loadCharacterTryPath = loadCharacterTryPath;
  server.jsonToCharacter = jsonToCharacter;

  const desiredPort = options.port || Number.parseInt(process.env.SERVER_PORT || '3000');
  const serverPort = await findNextAvailablePort(desiredPort);
  if (serverPort !== desiredPort) {
    logger.warn(`Port ${desiredPort} is in use, using port ${serverPort} instead`);
  }
  process.env.SERVER_PORT = serverPort.toString();
  try {
    await server.start(serverPort);
  } catch (error) {
    logger.error(`Failed to start server on port ${serverPort}:`, error);
    throw error;
  }

  // If we have project agents, start them with their init functions
  if (options.projectAgents && options.projectAgents.length > 0) {
    for (const projectAgent of options.projectAgents) {
      await startAgent(
        projectAgent.character,
        server,
        projectAgent.init,
        projectAgent.plugins || []
      );
    }
  }
  // If we have standalone characters, start them
  else if (options.characters && options.characters.length > 0) {
    for (const character of options.characters) {
      await startAgent(character, server);
    }
  }
  // Default fallback to Eliza character
  else {
    const elizaCharacter = getElizaCharacter();
    await startAgent(elizaCharacter, server);
  }
}

} // namespace elizaos
