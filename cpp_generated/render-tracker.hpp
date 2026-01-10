#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTC-AGENT_SRC_UTILS_RENDER-TRACKER_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTC-AGENT_SRC_UTILS_RENDER-TRACKER_H
#include "core.h"

typedef object RenderRecord;


extern std::shared_ptr<Map<string, std::shared_ptr<RenderRecord>>> renderCounts;
extern object CONFIG;
string summarizeValue(any value);

Record<string, string> getPropsSnapshot(Record<string, any> props);

array<string> findChanges(any prev, Record<string, string> curr);

void trackRender(string componentName, Record<string, any> props = undefined, Record<string, any> state = undefined);

void useRenderTracker(string componentName, Record<string, any> props = undefined, Record<string, any> state = undefined);

#endif
