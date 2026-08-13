// vercel_api_test.cpp - Tests for VercelAPI integration scaffolding.
#include <gtest/gtest.h>
#include "elizaos/vercel_api.hpp"

using namespace elizaos;

TEST(VercelConfig, BasicConstruction) {
    VercelConfig c;
    SUCCEED();
}

TEST(HttpClient, ConstructAndUse) {
    HttpClient c;
    SUCCEED();
}

TEST(VercelAPI, ConstructWithConfig) {
    VercelConfig cfg;
    VercelAPI api(cfg);
    SUCCEED();
}

TEST(VercelIntegration, ConstructWithConfig) {
    VercelConfig cfg;
    VercelIntegration v(cfg);
    SUCCEED();
}

TEST(VercelDeployment, BasicConstruction) {
    VercelDeployment d;
    SUCCEED();
}

TEST(VercelProject, BasicConstruction) {
    VercelProject p;
    SUCCEED();
}

TEST(VercelAPI, AddDomainRejectsMissingDomainWithoutNetworkCall) {
    VercelConfig cfg;
    VercelAPI api(cfg);
    const VercelDomain domain = api.addDomain("", "project-id");
    EXPECT_TRUE(domain.name.empty());
    ASSERT_TRUE(api.hasError());
    EXPECT_EQ(api.getLastError().code, 400);
    EXPECT_NE(api.getLastError().message.find("Domain name"), std::string::npos);
}

TEST(VercelAPI, AddDomainRejectsMissingProjectWithoutNetworkCall) {
    VercelConfig cfg;
    VercelAPI api(cfg);
    const VercelDomain domain = api.addDomain("example.test", "");
    EXPECT_TRUE(domain.project_id.empty());
    ASSERT_TRUE(api.hasError());
    EXPECT_EQ(api.getLastError().code, 400);
    EXPECT_NE(api.getLastError().message.find("project ID"), std::string::npos);
}
