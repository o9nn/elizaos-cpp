#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
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
    WindowedFile(WindowedFileConfig = {} config);
    void loadFile();
    void saveFile();
    void saveRegistry();
    double firstLine() const;
    void firstLine(double value);
    double window() const;
    void window(double value);
    double nLines() const;
    void printWindow();
    void replaceInWindow(const std::string& oldText, const std::string& newText);
    void goto(double lineNumber, const std::variant<'top', 'center' = 'center'>& mode);
    void scroll(double lines);
    std::string getWindowText(boolean = false header, boolean = false lineNumbers, boolean = false footer);
    std::vector<double> findAllOccurrences(const std::string& searchText, boolean = false zeroBased);
    void for(auto let i = 0; i < this._content.length; i++);
    void if(auto nReplacements > 0);
    void undoEdit();


} // namespace elizaos
