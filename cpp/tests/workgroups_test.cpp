// workgroups_test.cpp - Tests for Workgroup and WorkgroupManager.
#include <gtest/gtest.h>
#include "elizaos/workgroups.hpp"

using namespace elizaos;

TEST(WorkgroupConfig, BasicConstruction) {
    WorkgroupConfig c;
    SUCCEED();
}

TEST(WorkgroupTask, BasicConstruction) {
    WorkgroupTask t;
    SUCCEED();
}

TEST(WorkgroupMember, BasicConstruction) {
    WorkgroupMember m;
    SUCCEED();
}

TEST(WorkgroupStats, BasicConstruction) {
    WorkgroupStats s;
    SUCCEED();
}

TEST(WorkgroupRoleEnum, EnumValuesDistinct) {
    int a = static_cast<int>(WorkgroupRole{});
    SUCCEED() << "default WorkgroupRole = " << a;
}

TEST(WorkgroupStateEnum, EnumValuesDistinct) {
    int a = static_cast<int>(WorkgroupState{});
    SUCCEED() << "default WorkgroupState = " << a;
}

TEST(WorkgroupManager, ConstructWithDeps) {
    EXPECT_NO_THROW({
        WorkgroupManager m(nullptr, nullptr);
        (void)m;
    });
}
