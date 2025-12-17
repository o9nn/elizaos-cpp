#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".registry.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Windowed file editor
 * Provides windowed view and editing of files
 * Converted from tools/windowed/
 */

struct WindowState {
    std::optional<std::string> currentFile;
    double firstLine;
    double windowSize;
    std::optional<std::vector<std::string>> fileContent;
};

class WindowedEditor {
public:
    WindowedEditor();
    WindowState loadState();
    void saveState();
    std::vector<std::string> loadFile(const std::string& filePath);
    void saveFile(const std::string& filePath, const std::vector<std::string>& lines);
    void printWindow(boolean = true showLineNumbers, boolean = true showStatus);
    void open(const std::string& filePath, std::optional<double> line);
    void goto(double line);
    void scrollDown(std::optional<double> lines);
    void scrollUp(std::optional<double> lines);
    void edit(double startLine, double endLine, const std::string& newContent);
    void create(const std::string& filePath, std::optional<std::string> content);

private:
    WindowState state_;
    std::string stateFile_;
};

// CLI if run directly
// CLI setup

// Only set up CLI commands if running as a script
void setupCLI();

// Set up and parse CLI


} // namespace elizaos
