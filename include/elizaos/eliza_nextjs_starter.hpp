// eliza_nextjs_starter.hpp - Next.js starter template generator for ElizaOS
// Provides utilities to generate Next.js project templates with ElizaOS integration

#ifndef ELIZAOS_ELIZA_NEXTJS_STARTER_HPP
#define ELIZAOS_ELIZA_NEXTJS_STARTER_HPP

#include <string>
#include <vector>

namespace elizaos {
namespace nextjs_starter {

// ==============================================================================
// TEMPLATE GENERATOR
// ==============================================================================

class NextJSTemplateGenerator {
public:
    // Generate a complete Next.js project with ElizaOS integration
    static bool generateProject(const std::string& projectPath, const std::string& projectName);

    // Utility function to write files
    static bool writeFileStatic(const std::string& path, const std::string& content);

private:
    static bool createDirectory(const std::string& path);
    static bool writeFile(const std::string& path, const std::string& content);
    static bool generatePackageJson(const std::string& projectPath, const std::string& projectName);
    static bool generateTsConfig(const std::string& projectPath);
    static bool generateNextConfig(const std::string& projectPath);
    static bool generatePages(const std::string& projectPath);
    static bool generateApiRoutes(const std::string& projectPath);
    static bool generateComponents(const std::string& projectPath);
    static bool generateLibrary(const std::string& projectPath);
    static bool generateStyles(const std::string& projectPath);
    static bool generateReadme(const std::string& projectPath, const std::string& projectName);
};

// ==============================================================================
// WEBSOCKET INTEGRATION
// ==============================================================================

class WebSocketIntegration {
public:
    // Generate WebSocket support files for the Next.js project
    static bool generateWebSocketSupport(const std::string& projectPath);

private:
    static bool createDirectory(const std::string& path);
};

// ==============================================================================
// FREE FUNCTIONS
// ==============================================================================

// Generate a Next.js project with ElizaOS integration
bool generateNextJSProject(const std::string& projectPath, const std::string& projectName);

// Generate a Next.js project with WebSocket support
bool generateNextJSProjectWithWebSocket(const std::string& projectPath, const std::string& projectName);

} // namespace nextjs_starter
} // namespace elizaos

#endif // ELIZAOS_ELIZA_NEXTJS_STARTER_HPP
