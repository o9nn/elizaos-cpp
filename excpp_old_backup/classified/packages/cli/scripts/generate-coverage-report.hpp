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



struct CoverageReport {
    double totalFiles;
    double testedFiles;
    std::vector<std::string> untestedFiles;
    double coverage;
    { byCategory;
    { total: number; tested: number; untested: string[] } commands;
    { total: number; tested: number; untested: string[] } utils;
    { total: number; tested: number; untested: string[] } types;
    { total: number; tested: number; untested: string[] } other;
};

std::future<CoverageReport> generateCoverageReport();

std::future<void> main();


} // namespace elizaos
