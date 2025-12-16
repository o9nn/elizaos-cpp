#include "communityInvestor.e2e.hpp"
#include "events.hpp"
#include "service.hpp"
#include "trustScore.hpp"
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

import type { TestSuite, TestCase } from '@elizaos/core';
;
;
;
;

const allTestCases: TestCase[] = [
  ...serviceTestSuite.tests,
  ...eventsTestSuite.tests,
  ...trustScoreTestSuite.tests,
  ...communityInvestorE2ETestSuite.tests,
];

const allCommunityInvestorPluginTests: TestSuite = {
  name: 'CommunityInvestor Plugin - All Tests',
  tests: allTestCases,
};

} // namespace elizaos
