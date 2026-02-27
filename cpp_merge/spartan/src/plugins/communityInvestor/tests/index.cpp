#include "index.hpp"
#include <string>

array<std::shared_ptr<TestCase>> allTestCases = array<std::shared_ptr<TestCase>>{ serviceTestSuite->tests, eventsTestSuite->tests, trustScoreTestSuite->tests, communityInvestorE2ETestSuite->tests };
std::shared_ptr<TestSuite> allCommunityInvestorPluginTests = object{
    object::pair{std::string("name"), std::string("CommunityInvestor Plugin - All Tests")}, 
    object::pair{std::string("tests"), allTestCases}
};

void Main(void)
{
}

MAIN
