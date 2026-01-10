#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUM-TRACKER_SRC_SCRIPTS_TOKEN-METADATA-SERVICE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUM-TRACKER_SRC_SCRIPTS_TOKEN-METADATA-SERVICE_H
#include "core.h"
#include "../services/background/token-metadata.h"
using tokenMetadataService = _default;

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
