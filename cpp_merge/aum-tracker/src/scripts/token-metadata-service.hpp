#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUM_TRACKER_SRC_SCRIPTS_TOKEN_METADATA_SERVICE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUM_TRACKER_SRC_SCRIPTS_TOKEN_METADATA_SERVICE_H
#include "core.hpp"
#include "../services/background/token-metadata.h"
// Using alias removed (invalid transpilation)

extern array<string> args;
extern string command;
void main();

void startContinuousService();

void refreshStaleMetadata();

void healthCheck();

void showQueueStatus();

void clearQueue();

void showHelp();

#endif
