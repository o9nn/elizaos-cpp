// awesome_eliza_test.cpp - Tests for AwesomeElizaManager catalog/registry.
#include <gtest/gtest.h>
#include "elizaos/awesome_eliza.hpp"

using namespace elizaos;

TEST(AwesomeElizaManager, ConstructAndUse) {
    AwesomeElizaManager m;
    SUCCEED();
}

TEST(AwesomeResource, BasicConstruction) {
    AwesomeResource r;
    EXPECT_TRUE(r.name.empty());
}

TEST(ResourceCategoryEnum, NonEmpty) {
    int i = static_cast<int>(ResourceCategory{});
    SUCCEED() << "default ResourceCategory = " << i;
}

TEST(PluginCategoryEnum, NonEmpty) {
    int i = static_cast<int>(PluginCategory{});
    SUCCEED() << "default PluginCategory = " << i;
}
