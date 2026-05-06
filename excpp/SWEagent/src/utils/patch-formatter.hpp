#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Patch formatter utilities
 * Converted from sweagent/utils/patch_formatter.py
 */

struct PatchHunk {
    double sourceStart;
    double sourceLines;
    double targetStart;
    double targetLines;
    std::vector<std::string> lines;
};

struct PatchFile {
    std::string source;
    std::string target;
    std::vector<PatchHunk> hunks;
};

class PatchFormatter {
public:
    std::vector<PatchFile> parsePatch(const std::string& patch);
    static std::tuple<std::vector<double>, std::vector<double>> mergeIntervals(const std::vector<double>& starts, const std::vector<double>& stops);
    std::string formatFile(const std::string& text, const std::vector<double>& starts, const std::vector<double>& stops, std::any options = {});
    void readFiles(bool original);
    std::string getFilesStr(std::any options = {});


} // namespace elizaos
