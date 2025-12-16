#include "AIService/AIService.js.hpp"
#include "Configuration.js.hpp"
#include "DirectoryTraversal.js.hpp"
#include "DocumentationGenerator.js.hpp"
#include "GitManager.js.hpp"
#include "JsDocAnalyzer.js.hpp"
#include "JsDocGenerator.js.hpp"
#include "PluginDocumentationGenerator.js.hpp"
#include "TypeScriptParser.js.hpp"
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
 * Main  A promise that resolves when the documentation generation is complete.
 */

async );

    let prFiles: string[] = [];
    if (
      typeof configuration.repository.pullNumber === 'number' &&
      !Number.isNaN(configuration.repository.pullNumber)
    ) {
      console.log('Pull Request Number: ', configuration.repository.pullNumber);
      try {
        const files = await gitManager.getFilesInPullRequest(configuration.repository.pullNumber);
        prFiles = files.map((file) => file.filename);
      } catch (prError) {
        console.error('Error fetching PR files:', {
          error: prError,
          pullNumber: configuration.repository.pullNumber,
          repository: `${configuration.repository.owner}/${configuration.repository.name}`,
        });
        throw prError;
      }
    }

    try {
      const directoryTraversal = new DirectoryTraversal(configuration, prFiles);
      const typeScriptParser = new TypeScriptParser();
      const jsDocAnalyzer = new JsDocAnalyzer(typeScriptParser);
      const aiService = new AIService(configuration);
      const jsDocGenerator = new JsDocGenerator(aiService);

      const documentationGenerator = new DocumentationGenerator(
        directoryTraversal,
        typeScriptParser,
        jsDocAnalyzer,
        jsDocGenerator,
        gitManager,
        configuration,
        aiService
      );

      const pluginDocGenerator = new PluginDocumentationGenerator(
        aiService,
        gitManager,
        configuration
      );

      const { todoItems, envUsages } = await documentationGenerator.analyzeCodebase();

      // Generate JSDoc documentation first
      const { documentedItems, branchName } = await documentationGenerator.generate(
        configuration.repository.pullNumber
      );

      // If both are true, use JSDoc branch for README
      // If only README is true, create new branch
      if (configuration.generateReadme) {
        const targetBranch =
          configuration.generateJsDoc && branchName
            ? branchName
            : `docs-update-readme-${Date.now()}`;

        if (!configuration.generateJsDoc) {
          await gitManager.createBranch(targetBranch, configuration.branch);
        }

        await pluginDocGenerator.generate(documentedItems, targetBranch, todoItems, envUsages);

        // Only create PR if we're not also generating JSDoc (otherwise changes go in JSDoc PR)
        if (!configuration.generateJsDoc) {
          const prContent = {
            title: 'docs: Update plugin documentation',
            body: 'Updates plugin documentation with latest changes',
          };

          await gitManager.createPullRequest({
            title: prContent.title,
            body: prContent.body,
            head: targetBranch,
            base: configuration.branch,
            labels: ['documentation', 'automated-pr'],
            reviewers: configuration.pullRequestReviewers || [],
          });
        }
      }
    } catch (error) {
      console.error('Error during documentation generation:', {
        message: error instanceof Error ? error.message : String(error),
        stack: error instanceof Error ? error.stack : undefined,
        timestamp: new Date().toISOString(),
      });
      process.exit(1);
    }
  } catch (error) {
    console.error('Critical error during documentation generation:', {
      error:
        error instanceof Error
          ? {
              name: error.name,
              message: error.message,
              stack: error.stack,
            }
          : error,
      timestamp: new Date().toISOString(),
      nodeVersion: process.version,
      platform: process.platform,
    });
    process.exit(1);
  }
}

// Simple error handling for the main );

} // namespace elizaos
