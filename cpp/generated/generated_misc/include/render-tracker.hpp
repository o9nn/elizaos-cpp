#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTC-AGENT_SRC_UTILS_RENDER-TRACKER_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTC-AGENT_SRC_UTILS_RENDER-TRACKER_H
#include "core.h"

typedef object RenderRecord;


extern std::shared_ptr<Map<std::string, std::shared_ptr<RenderRecord>>> renderCounts;
extern object CONFIG;
std::string summarizeValue(std::any value);

Record<std::string, string> getPropsSnapshot(Record<std::string, any> props);

array<string> findChanges(std::any prev, Record<std::string, string> curr);

void trackRender(std::string componentName, Record<std::string, any> props = undefined, Record<std::string, any> state = undefined);

void useRenderTracker(std::string componentName, Record<std::string, any> props = undefined, Record<std::string, any> state = undefined);

#endif
