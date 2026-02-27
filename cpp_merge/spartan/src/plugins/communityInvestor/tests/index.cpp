#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/communityInvestor/tests/index.h"

array<std::shared_ptr<TestCase>> allTestCases = array<std::shared_ptr<TestCase>>{ serviceTestSuite->tests, eventsTestSuite->tests, trustScoreTestSuite->tests, communityInvestorE2ETestSuite->tests };
std::shared_ptr<TestSuite> allCommunityInvestorPluginTests = object{
    object::pair{std:("name"), std:("CommunityInvestor Plugin - All Tests")}, 
    object::pair{std:("tests"), allTestCases}
};

void Main(void)
{
}

MAIN
