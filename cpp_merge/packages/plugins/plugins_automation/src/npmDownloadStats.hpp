#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_PLUGINS_AUTOMATION_SRC_NPMDOWNLOADSTATS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_PLUGINS_AUTOMATION_SRC_NPMDOWNLOADSTATS_H
#include "core.hpp"
#include "axios.hpp"
// Using alias removed (invalid transpilation)
#include "xlsx.hpp"
#include "chalk.hpp"
// Using alias removed (invalid transpilation)
#include "ora.hpp"
// Using alias removed (invalid transpilation)
#include "fs-extra.h"
// Using alias removed (invalid transpilation)
#include "path.hpp"
// Using alias removed (invalid transpilation)
#include "dotenv.hpp"
// Using alias removed (invalid transpilation)

class PackageInfo;
class VersionDownloads;
class PackageDownloads;
class NpmDownloadStatsGenerator;

class PackageInfo : public object, public std::enable_shared_from_this<PackageInfo> {
public:
    using std::enable_shared_from_this<PackageInfo>::shared_from_this;
    string name;

    array<string> versions;

    string description;

    string repository;

    array<string> maintainers;

    array<string> keywords;

    string license;

    string createdDate;

    string modifiedDate;
};

class VersionDownloads : public object, public std::enable_shared_from_this<VersionDownloads> {
public:
    using std::enable_shared_from_this<VersionDownloads>::shared_from_this;
    string packageName;

    string version;

    double downloads;

    string downloadsPeriod;
};

class PackageDownloads : public object, public std::enable_shared_from_this<PackageDownloads> {
public:
    using std::enable_shared_from_this<PackageDownloads>::shared_from_this;
    string packageName;

    double totalDownloads;

    string downloadsPeriod;

    double weeklyDownloads;

    double monthlyDownloads;

    double yearlyDownloads;
};

class NpmDownloadStatsGenerator : public object, public std::enable_shared_from_this<NpmDownloadStatsGenerator> {
public:
    using std::enable_shared_from_this<NpmDownloadStatsGenerator>::shared_from_this;
    string NPM_REGISTRY_URL = std:("https://registry.npmjs.org");

    string NPM_DOWNLOADS_URL = std:("https://api.npmjs.org/downloads");

    string OUTPUT_DIR = std:("./assets");

    string OUTPUT_FILE = std:("elizaos-npm-download-stats.xlsx");

    virtual std::shared_ptr<Promise<void>> main();
    virtual std::shared_ptr<Promise<array<std::shared_ptr<PackageInfo>>>> fetchOrganizationPackages();
    virtual std::shared_ptr<Promise<array<std::shared_ptr<PackageDownloads>>>> fetchPackageDownloads(array<std::shared_ptr<PackageInfo>> packages);
    virtual std::shared_ptr<Promise<array<std::shared_ptr<VersionDownloads>>>> fetchVersionDownloads(array<std::shared_ptr<PackageInfo>> packages);
    virtual std::shared_ptr<Promise<object>> getDownloadStats(string packageName, string period);
    virtual std::shared_ptr<Promise<void>> generateExcelReport(array<std::shared_ptr<PackageInfo>> packages, array<std::shared_ptr<PackageDownloads>> packageDownloads, array<std::shared_ptr<VersionDownloads>> versionDownloads);
    virtual void printSummary(array<std::shared_ptr<PackageInfo>> packages, array<std::shared_ptr<PackageDownloads>> packageDownloads);
    virtual std::shared_ptr<Promise<void>> delay(double ms);
};

std::shared_ptr<Promise<void>> main();

#endif
