#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
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
    [number[], number[]] mergeIntervals(const std::vector<double>& starts, const std::vector<double>& stops);
    std::string formatFile(const std::string& text, const std::vector<double>& starts, const std::vector<double>& stops, std::optional<std::any> options);
    std::unordered_map<std::string, std::vector<[number>, number[]]> getHunkLines(bool original, number = 50 contextLength);
    void readFiles(bool original);
    std::string concatFilesStrings(Map<string files, auto string>);
    std::string getFilesStr(std::optional<std::any> options);

private:
    std::vector<PatchFile> parsedPatch_;
};


} // namespace elizaos
