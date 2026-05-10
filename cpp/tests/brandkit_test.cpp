// brandkit_test.cpp - E2E tests for the BrandKit static interface.
#include <gtest/gtest.h>
#include "elizaos/brandkit.hpp"

using namespace elizaos::brandkit;

TEST(BrandKitColors, GetAllColorsReturnsNonEmpty) {
    auto colors = BrandKit::getAllColors();
    EXPECT_FALSE(colors.empty());
}

TEST(BrandKitColors, EachColorHasNameAndHex) {
    for (const auto& kv : BrandKit::getAllColors()) {
        EXPECT_FALSE(kv.first.empty());
        EXPECT_FALSE(kv.second.name.empty());
        EXPECT_FALSE(kv.second.hex.empty());
    }
}

TEST(BrandKitColors, GetColorByValidName) {
    auto colors = BrandKit::getAllColors();
    if (!colors.empty()) {
        const auto& first = colors.begin()->first;
        auto hex = BrandKit::getColor(first);
        EXPECT_FALSE(hex.empty());
    }
}

TEST(BrandKitColors, GetColorByMissingNameReturnsEmptyOrFallback) {
    auto hex = BrandKit::getColor("__definitely_does_not_exist__");
    // Either empty string or sentinel — both acceptable, just must not crash
    SUCCEED() << "returned: " << hex;
}

TEST(BrandKitFonts, GetAllFontsReturnsList) {
    auto fonts = BrandKit::getAllFonts();
    EXPECT_FALSE(fonts.empty());
    for (const auto& f : fonts) {
        EXPECT_FALSE(f.name.empty());
    }
}

TEST(BrandKitFonts, GetFontReturnsStruct) {
    auto fonts = BrandKit::getAllFonts();
    if (!fonts.empty()) {
        auto f = BrandKit::getFont(fonts.front().name);
        EXPECT_EQ(f.name, fonts.front().name);
    }
}

TEST(BrandKitAssets, GetAllAssetsReturnsList) {
    auto assets = BrandKit::getAllAssets();
    // Allow empty assets list (some configurations may ship without assets)
    for (const auto& a : assets) {
        EXPECT_FALSE(a.name.empty());
    }
}

TEST(BrandKitGenerators, CSSContainsRootBlock) {
    auto css = BrandKit::generateCSSVariables();
    EXPECT_FALSE(css.empty());
    EXPECT_NE(css.find(":root"), std::string::npos);
}

TEST(BrandKitGenerators, JSONIsParsableShape) {
    auto js = BrandKit::generateJSONConfig();
    EXPECT_FALSE(js.empty());
    EXPECT_NE(js.find("{"), std::string::npos);
    EXPECT_NE(js.find("}"), std::string::npos);
}

TEST(BrandKitFreeFunctions, FreeFunctionParity) {
    auto colors = BrandKit::getAllColors();
    if (!colors.empty()) {
        const auto& name = colors.begin()->first;
        EXPECT_EQ(BrandKit::getColor(name), getBrandColor(name));
    }
    EXPECT_EQ(BrandKit::generateCSSVariables(), generateBrandCSS());
    EXPECT_EQ(BrandKit::generateJSONConfig(), generateBrandJSON());
}
