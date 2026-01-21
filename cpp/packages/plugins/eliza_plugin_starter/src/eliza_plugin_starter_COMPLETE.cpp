// ==============================================================================
// ELIZA PLUGIN STARTER
// ==============================================================================
// File: cpp/eliza_plugin_starter/src/eliza_plugin_starter_COMPLETE.cpp

#include "elizaos/core.hpp"
#include <string>
#include <fstream>
#include <sstream>

namespace elizaos {
namespace plugin_starter {

class PluginTemplateGenerator {
public:
    static bool generatePlugin(const std::string& pluginPath, const std::string& pluginName) {
        // Create plugin structure
        mkdir(pluginPath.c_str(), 0755);
        mkdir((pluginPath + "/src").c_str(), 0755);
        mkdir((pluginPath + "/include").c_str(), 0755);
        
        // Generate plugin.json
        std::ostringstream manifest;
        manifest << "{\n"
                 << "  \"name\": \"" << pluginName << "\",\n"
                 << "  \"version\": \"1.0.0\",\n"
                 << "  \"description\": \"ElizaOS plugin\",\n"
                 << "  \"main\": \"lib/index.js\",\n"
                 << "  \"author\": \"\",\n"
                 << "  \"license\": \"MIT\",\n"
                 << "  \"elizaos\": {\n"
                 << "    \"apiVersion\": \"1.0\",\n"
                 << "    \"capabilities\": [\"chat\", \"memory\", \"action\"]\n"
                 << "  }\n"
                 << "}\n";
        
        std::ofstream manifestFile(pluginPath + "/plugin.json");
        manifestFile << manifest.str();
        manifestFile.close();
        
        // Generate main plugin file
        std::string pluginCode = R"(#include "elizaos/core.hpp"
#include <string>

namespace elizaos {
namespace plugins {

class )" + pluginName + R"(Plugin {
public:
    )" + pluginName + R"(Plugin() {}
    
    void initialize() {
        // Plugin initialization
    }
    
    void shutdown() {
        // Plugin cleanup
    }
    
    std::string processMessage(const std::string& message) {
        // Process incoming message
        return "Processed: " + message;
    }
    
    void registerActions() {
        // Register plugin actions
    }
};

} // namespace plugins
} // namespace elizaos
)";
        
        std::ofstream pluginFile(pluginPath + "/src/" + pluginName + ".cpp");
        pluginFile << pluginCode;
        pluginFile.close();
        
        // Generate CMakeLists.txt
        std::ostringstream cmake;
        cmake << "cmake_minimum_required(VERSION 3.16)\n"
              << "project(" << pluginName << ")\n\n"
              << "set(CMAKE_CXX_STANDARD 17)\n\n"
              << "add_library(" << pluginName << " SHARED\n"
              << "    src/" << pluginName << ".cpp\n"
              << ")\n\n"
              << "target_include_directories(" << pluginName << " PUBLIC\n"
              << "    ${CMAKE_SOURCE_DIR}/include\n"
              << ")\n\n"
              << "target_link_libraries(" << pluginName << "\n"
              << "    elizaos-core\n"
              << ")\n";
        
        std::ofstream cmakeFile(pluginPath + "/CMakeLists.txt");
        cmakeFile << cmake.str();
        cmakeFile.close();
        
        // Generate README
        std::ostringstream readme;
        readme << "# " << pluginName << "\n\n"
               << "ElizaOS Plugin\n\n"
