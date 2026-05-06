#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct CoverageReport {
    double totalFiles;
    double testedFiles;
    std::vector<std::string> untestedFiles;
    double coverage;
    double tested;
    double tested;
    double tested;
    double tested;
};

std::variant<'commands', 'utils', 'types', 'other'> categorizeFile(const std::string& filePath);

std::future<CoverageReport> generateCoverageReport();

std::future<void> main();


} // namespace elizaos
