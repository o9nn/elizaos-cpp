#include <functional>
#include <future>
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
    { total; tested; untested: std:[] } commands;
    { total; tested; untested: std:[] } utils;
    { total; tested; untested: std:[] } types;
    { total; tested; untested: std:[] } other;
};

std::future<CoverageReport> generateCoverageReport();

std::future<void> main();


} // namespace elizaos
