#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTC_AGENT_SRC_UTILS_RENDER_TRACKER_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTC_AGENT_SRC_UTILS_RENDER_TRACKER_H
#include "core.hpp"

typedef object RenderRecord;


extern std::shared_ptr<Map<string, std::shared_ptr<RenderRecord>>> renderCounts;
extern object CONFIG;
string summarizeValue(any value);

Record<string, string> getPropsSnapshot(Record<string, any> props);

array<string> findChanges(any prev, Record<string, string> curr);

void trackRender(string componentName, Record<string, any> props = undefined, Record<string, any> state = undefined);

void useRenderTracker(string componentName, Record<string, any> props = undefined, Record<string, any> state = undefined);

#endif
