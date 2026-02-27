#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_UTILS_PORT_HANDLING_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_UTILS_PORT_HANDLING_H
#include "core.hpp"
// Node.js module removed - use C++ filesystem
// Using alias removed (invalid transpilation)

std::shared_ptr<Promise<boolean>> isPortFree(double port);

std::shared_ptr<Promise<double>> findNextAvailablePort(double startPort);

#endif
