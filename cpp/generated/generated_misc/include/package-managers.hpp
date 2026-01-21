#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-LIST_SRC_LIB_PACKAGE-MANAGERS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-LIST_SRC_LIB_PACKAGE-MANAGERS_H
#include "core.h"
#include "axios.h"
using axios = _default;

class PackageInfo;
class PackageManagerClient;

class PackageInfo : public object, public std::enable_shared_from_this<PackageInfo> {
public:
    using std::enable_shared_from_this<PackageInfo>::shared_from_this;
    std::string name;

    std::string version;

    std::string description;

    double downloads;

    std::string repository;

    array<string> dependencies;
};

class PackageManagerClient : public object, public std::enable_shared_from_this<PackageManagerClient> {
public:
    using std::enable_shared_from_this<PackageManagerClient>::shared_from_this;
    static std::shared_ptr<Promise<std::shared_ptr<PackageInfo>>> getNpmInfo(std::string packageName);
    static std::shared_ptr<Promise<std::shared_ptr<PackageInfo>>> getPyPiInfo(std::string packageName);
};

#endif
