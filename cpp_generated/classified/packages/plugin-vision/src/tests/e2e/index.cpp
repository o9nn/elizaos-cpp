#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-vision/src/tests/e2e/index.h"

array<any> testSuites = array<std::shared_ptr<VisionRuntimeTestSuite>>{ VisionRuntimeTestSuite, VisionBasicE2ETestSuite, VisionAutonomyE2ETestSuite, VisionCaptureLogTestSuite, ScreenVisionE2ETestSuite, VisionWorkerE2ETestSuite };

void Main(void)
{
}

MAIN
