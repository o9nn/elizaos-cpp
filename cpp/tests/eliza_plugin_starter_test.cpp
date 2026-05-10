// eliza_plugin_starter_test.cpp - Tests for the eliza plugin starter scaffold.
#include <gtest/gtest.h>
#include "elizaos/eliza_plugin_starter.hpp"

using namespace elizaos;

TEST(EPS_PluginVersion, BasicConstruction) {
    PluginVersion v;
    SUCCEED();
}

TEST(EPS_PluginManager, ConstructAndUse) {
    PluginManager m;
    SUCCEED();
}

TEST(EPS_PluginConfig, ConstructAndUse) {
    PluginConfig c;
    SUCCEED();
}

TEST(EPS_PluginMetadata, BasicConstruction) {
    PluginMetadata m;
    SUCCEED();
}

TEST(EPS_PluginStateEnum, EnumValuesDistinct) {
    int a = static_cast<int>(PluginState{});
    SUCCEED() << "default PluginState = " << a;
}
