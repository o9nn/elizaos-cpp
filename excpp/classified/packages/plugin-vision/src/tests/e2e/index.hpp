#include "screen-vision.hpp"
#include "vision-autonomy.hpp"
#include "vision-basic.hpp"
#include "vision-capture-log.hpp"
#include "vision-runtime.hpp"
#include "vision-worker-tests.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// Export all E2E test suites
{ default as VisionBasicE2ETestSuite } from './vision-basic';
{ default as VisionAutonomyE2ETestSuite } from './vision-autonomy';
{ default as VisionCaptureLogTestSuite } from './vision-capture-log';
{ default as ScreenVisionE2ETestSuite } from './screen-vision';
{ default as VisionRuntimeTestSuite } from './vision-runtime';
{ default as VisionWorkerE2ETestSuite } from './vision-worker-tests';

// Export as array for convenience
;
;
;
;
;
;

const testSuites = [
  VisionRuntimeTestSuite, // Real runtime tests first
  VisionBasicE2ETestSuite,
  VisionAutonomyE2ETestSuite,
  VisionCaptureLogTestSuite,
  ScreenVisionE2ETestSuite,
  VisionWorkerE2ETestSuite, // Worker-based tests
];

default testSuites;

} // namespace elizaos
