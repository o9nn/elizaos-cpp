#include "index.hpp"

array<std::shared_ptr<TestSuite>> testSuites = array<std::shared_ptr<TestSuite>>{ basicFunctionalityTestSuite, codeGenerationE2ETestSuite, claudeCodeIntegrationTestSuite, claudeCodeStressTestSuite };

void Main(void)
{
}

MAIN
