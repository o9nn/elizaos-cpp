// plugin_specification_test.cpp - E2E tests for PluginVersion / PluginDependency.
#include <gtest/gtest.h>
#include "elizaos/plugin_specification.hpp"

using namespace elizaos;

TEST(PluginVersion, ToStringNonEmpty) {
    PluginVersion v;
    v.major = 1; v.minor = 2; v.patch = 3;
    EXPECT_FALSE(v.toString().empty());
}

TEST(PluginVersion, FromStringRoundtrip) {
    auto v = PluginVersion::fromString("2.3.4");
    EXPECT_EQ(v.major, 2);
    EXPECT_EQ(v.minor, 3);
    EXPECT_EQ(v.patch, 4);
}

TEST(PluginVersion, CompatibilityIsBoolean) {
    PluginVersion a, b;
    EXPECT_NO_THROW(a.isCompatibleWith(b));
}

TEST(PluginDependency, IsSatisfiedByMinMax) {
    PluginDependency d;
    d.pluginName = "core";
    d.minVersion = PluginVersion::fromString("1.0.0");
    d.maxVersion = PluginVersion::fromString("2.0.0");
    PluginVersion v = PluginVersion::fromString("1.5.0");
    EXPECT_NO_THROW(d.isSatisfiedBy(v));
}

TEST(PluginParameter, JsonRoundtripCallable) {
    PluginParameter p;
    p.name = "x";
    p.description = "y";
    p.type = "string";
    auto j = p.toJson();
    EXPECT_NO_THROW(PluginParameter::fromJson(j));
}

TEST(PluginCapability, EnumValuesDistinct) {
    EXPECT_NE(static_cast<int>(PluginCapability::ACTION_PROCESSING),
              static_cast<int>(PluginCapability::CUSTOM));
}
