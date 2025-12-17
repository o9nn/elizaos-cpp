#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct PackageInfo {
    std::string name;
    std::vector<std::string> versions;
    std::optional<std::string> description;
    std::optional<std::string> repository;
    std::optional<std::vector<std::string>> maintainers;
    std::optional<std::vector<std::string>> keywords;
    std::optional<std::string> license;
    std::optional<std::string> createdDate;
    std::optional<std::string> modifiedDate;
};

struct VersionDownloads {
    std::string packageName;
    std::string version;
    double downloads;
    std::string downloadsPeriod;
};

struct PackageDownloads {
    std::string packageName;
    double totalDownloads;
    std::string downloadsPeriod;
    double weeklyDownloads;
    double monthlyDownloads;
    double yearlyDownloads;
};

class NpmDownloadStatsGenerator {
public:
    std::future<void> main();
    std::future<std::vector<PackageInfo>> fetchOrganizationPackages();
    std::future<std::vector<PackageDownloads>> fetchPackageDownloads(const std::vector<PackageInfo>& packages);
    std::future<std::vector<VersionDownloads>> fetchVersionDownloads(const std::vector<PackageInfo>& packages);
    Promise< getDownloadStats(const std::string& packageName, const std::string& period);
    void catch(const std::any& error);
    std::future<void> generateExcelReport(const std::vector<PackageInfo>& packages, const std::vector<PackageDownloads>& packageDownloads, const std::vector<VersionDownloads>& versionDownloads);
    void printSummary(const std::vector<PackageInfo>& packages, const std::vector<PackageDownloads>& packageDownloads);
    std::future<void> delay(double ms);
};

std::future<void> main();

} // namespace elizaos
