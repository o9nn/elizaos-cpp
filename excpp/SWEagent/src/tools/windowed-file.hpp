#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "registry.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * WindowedFile class for managing windowed file editing
 * Converted from Python windowed_file.py
 */

class TextNotFound {
public:
    TextNotFound(const std::string& message);
};

struct WindowedFileConfig {
    std::optional<bool> exitOnException;
    std::optional<std::string> path;
};

class WindowedFile {
public:
    WindowedFile(WindowedFileConfig config = {});
    void loadFile();
    void saveFile();
    void saveRegistry();
    double firstLine() const;
    double window() const;
    double nLines() const;
    std::tuple<double, double> lineRange() const;
    void printWindow();
    void replaceInWindow(const std::string& oldText, const std::string& newText);
    void scroll(double lines);
    std::string getWindowText(bool header = false, bool lineNumbers = false, bool footer = false);
    std::vector<double> findAllOccurrences(const std::string& searchText, bool zeroBased = false);
    void undoEdit();


} // namespace elizaos
