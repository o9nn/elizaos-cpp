#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-VISION_SRC_TESTS_E2E_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-VISION_SRC_TESTS_E2E_INDEX_H
#include "core.h"
#include "./vision-basic.h"
using VisionBasicE2ETestSuite = _default;
#include "./vision-autonomy.h"
using VisionAutonomyE2ETestSuite = _default;
#include "./vision-capture-log.h"
using VisionCaptureLogTestSuite = _default;
#include "./screen-vision.h"
using ScreenVisionE2ETestSuite = _default;
#include "./vision-runtime.h"
using VisionRuntimeTestSuite = _default;
#include "./vision-worker-tests.h"
using VisionWorkerE2ETestSuite = _default;

extern array<any> testSuites;
#endif
