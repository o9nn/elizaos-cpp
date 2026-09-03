#include <gtest/gtest.h>
#include "elizaos/brandkit.hpp"
#include <string>
#include <unordered_map>
#include <vector>
#include <thread>
#include <atomic>

using namespace elizaos::brandkit;

class BrandKitTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }
    
    void TearDown() override {
        // Cleanup code if needed
    }
};

// ==============================================================================
// BRAND COLOR TESTS
// ==============================================================================

TEST_F(BrandKitTest, GetPrimaryColor) {
    std::string color = BrandKit::getColor("primary");
    EXPECT_FALSE(color.empty());
    EXPECT_EQ(color, "#6366F1");
}

TEST_F(BrandKitTest, GetSecondaryColor) {
    std::string color = BrandKit::getColor("secondary");
    EXPECT_FALSE(color.empty());
    EXPECT_EQ(color, "#8B5CF6");
}

TEST_F(BrandKitTest, GetAccentColor) {
    std::string color = BrandKit::getColor("accent");
    EXPECT_FALSE(color.empty());
    EXPECT_EQ(color, "#EC4899");
}

TEST_F(BrandKitTest, GetSuccessColor) {
    std::string color = BrandKit::getColor("success");
    EXPECT_FALSE(color.empty());
    EXPECT_EQ(color, "#10B981");
}

TEST_F(BrandKitTest, GetWarningColor) {
    std::string color = BrandKit::getColor("warning");
    EXPECT_FALSE(color.empty());
    EXPECT_EQ(color, "#F59E0B");
}

TEST_F(BrandKitTest, GetErrorColor) {
    std::string color = BrandKit::getColor("error");
    EXPECT_FALSE(color.empty());
    EXPECT_EQ(color, "#EF4444");
}

TEST_F(BrandKitTest, GetInfoColor) {
    std::string color = BrandKit::getColor("info");
    EXPECT_FALSE(color.empty());
    EXPECT_EQ(color, "#3B82F6");
}

TEST_F(BrandKitTest, GetBackgroundColor) {
    std::string color = BrandKit::getColor("background");
    EXPECT_FALSE(color.empty());
    EXPECT_EQ(color, "#0F172A");
}

TEST_F(BrandKitTest, GetSurfaceColor) {
    std::string color = BrandKit::getColor("surface");
    EXPECT_FALSE(color.empty());
    EXPECT_EQ(color, "#1E293B");
}

TEST_F(BrandKitTest, GetTextColor) {
    std::string color = BrandKit::getColor("text");
    EXPECT_FALSE(color.empty());
    EXPECT_EQ(color, "#F1F5F9");
}

TEST_F(BrandKitTest, GetTextSecondaryColor) {
    std::string color = BrandKit::getColor("textSecondary");
    EXPECT_FALSE(color.empty());
    EXPECT_EQ(color, "#94A3B8");
}

TEST_F(BrandKitTest, GetInvalidColorReturnsDefault) {
    std::string color = BrandKit::getColor("nonexistent");
    EXPECT_EQ(color, "#000000");
}

TEST_F(BrandKitTest, GetAllColors) {
    auto colors = BrandKit::getAllColors();
    EXPECT_FALSE(colors.empty());
    EXPECT_GE(colors.size(), 11); // At least 11 colors defined
    
    // Check that primary color exists
    EXPECT_NE(colors.find("primary"), colors.end());
    EXPECT_EQ(colors["primary"].hex, "#6366F1");
    EXPECT_EQ(colors["primary"].rgb, "rgb(99, 102, 241)");
}

TEST_F(BrandKitTest, ColorStructureComplete) {
    auto colors = BrandKit::getAllColors();
    auto primaryColor = colors["primary"];
    
    EXPECT_FALSE(primaryColor.name.empty());
    EXPECT_FALSE(primaryColor.hex.empty());
    EXPECT_FALSE(primaryColor.rgb.empty());
    EXPECT_FALSE(primaryColor.description.empty());
    
    EXPECT_EQ(primaryColor.name, "Primary");
    EXPECT_EQ(primaryColor.description, "Main brand color - Indigo");
}

// ==============================================================================
// BRAND FONT TESTS
// ==============================================================================

TEST_F(BrandKitTest, GetHeadingFont) {
    auto font = BrandKit::getFont("heading");
    EXPECT_FALSE(font.name.empty());
    EXPECT_EQ(font.name, "heading");
    EXPECT_EQ(font.family, "Inter");
    EXPECT_EQ(font.weight, "700");
    EXPECT_EQ(font.usage, "Headings and titles");
}

TEST_F(BrandKitTest, GetBodyFont) {
    auto font = BrandKit::getFont("body");
    EXPECT_FALSE(font.name.empty());
    EXPECT_EQ(font.name, "body");
    EXPECT_EQ(font.family, "Inter");
    EXPECT_EQ(font.weight, "400");
}

TEST_F(BrandKitTest, GetMonoFont) {
    auto font = BrandKit::getFont("mono");
    EXPECT_FALSE(font.name.empty());
    EXPECT_EQ(font.name, "mono");
    EXPECT_EQ(font.family, "Fira Code");
    EXPECT_EQ(font.weight, "400");
}

TEST_F(BrandKitTest, GetDisplayFont) {
    auto font = BrandKit::getFont("display");
    EXPECT_FALSE(font.name.empty());
    EXPECT_EQ(font.name, "display");
    EXPECT_EQ(font.family, "Inter");
    EXPECT_EQ(font.weight, "900");
}

TEST_F(BrandKitTest, GetInvalidFontReturnsEmpty) {
    auto font = BrandKit::getFont("nonexistent");
    EXPECT_TRUE(font.name.empty());
    EXPECT_TRUE(font.family.empty());
}

TEST_F(BrandKitTest, GetAllFonts) {
    auto fonts = BrandKit::getAllFonts();
    EXPECT_FALSE(fonts.empty());
    EXPECT_GE(fonts.size(), 4); // At least 4 fonts defined
    
    // Check that all fonts have required fields
    for (const auto& font : fonts) {
        EXPECT_FALSE(font.name.empty());
        EXPECT_FALSE(font.family.empty());
        EXPECT_FALSE(font.weight.empty());
        EXPECT_FALSE(font.usage.empty());
    }
}

// ==============================================================================
// BRAND ASSET TESTS
// ==============================================================================

TEST_F(BrandKitTest, GetLogoAsset) {
    auto asset = BrandKit::getAsset("logo");
    EXPECT_FALSE(asset.name.empty());
    EXPECT_EQ(asset.name, "logo");
    EXPECT_EQ(asset.type, "svg");
    EXPECT_EQ(asset.url, "/assets/logo.svg");
    EXPECT_EQ(asset.description, "Primary ElizaOS logo");
}

TEST_F(BrandKitTest, GetLogoLightAsset) {
    auto asset = BrandKit::getAsset("logoLight");
    EXPECT_FALSE(asset.name.empty());
    EXPECT_EQ(asset.name, "logoLight");
    EXPECT_EQ(asset.type, "svg");
    EXPECT_EQ(asset.url, "/assets/logo-light.svg");
}

TEST_F(BrandKitTest, GetLogoDarkAsset) {
    auto asset = BrandKit::getAsset("logoDark");
    EXPECT_FALSE(asset.name.empty());
    EXPECT_EQ(asset.name, "logoDark");
    EXPECT_EQ(asset.type, "svg");
    EXPECT_EQ(asset.url, "/assets/logo-dark.svg");
}

TEST_F(BrandKitTest, GetIconAsset) {
    auto asset = BrandKit::getAsset("icon");
    EXPECT_FALSE(asset.name.empty());
    EXPECT_EQ(asset.name, "icon");
    EXPECT_EQ(asset.type, "svg");
}

TEST_F(BrandKitTest, GetFaviconAsset) {
    auto asset = BrandKit::getAsset("favicon");
    EXPECT_FALSE(asset.name.empty());
    EXPECT_EQ(asset.name, "favicon");
    EXPECT_EQ(asset.type, "ico");
}

TEST_F(BrandKitTest, GetOgImageAsset) {
    auto asset = BrandKit::getAsset("ogImage");
    EXPECT_FALSE(asset.name.empty());
    EXPECT_EQ(asset.name, "ogImage");
    EXPECT_EQ(asset.type, "png");
}

TEST_F(BrandKitTest, GetInvalidAssetReturnsEmpty) {
    auto asset = BrandKit::getAsset("nonexistent");
    EXPECT_TRUE(asset.name.empty());
    EXPECT_TRUE(asset.type.empty());
}

TEST_F(BrandKitTest, GetAllAssets) {
    auto assets = BrandKit::getAllAssets();
    EXPECT_FALSE(assets.empty());
    EXPECT_GE(assets.size(), 6); // At least 6 assets defined
    
    // Check that all assets have required fields
    for (const auto& asset : assets) {
        EXPECT_FALSE(asset.name.empty());
        EXPECT_FALSE(asset.type.empty());
        EXPECT_FALSE(asset.url.empty());
        EXPECT_FALSE(asset.description.empty());
    }
}

// ==============================================================================
// CSS GENERATION TESTS
// ==============================================================================

TEST_F(BrandKitTest, GenerateCSSVariables) {
    std::string css = BrandKit::generateCSSVariables();
    EXPECT_FALSE(css.empty());
    
    // Check for :root declaration
    EXPECT_NE(css.find(":root"), std::string::npos);
    
    // Check for color variables
    EXPECT_NE(css.find("--color-primary"), std::string::npos);
    EXPECT_NE(css.find("--color-secondary"), std::string::npos);
    EXPECT_NE(css.find("--color-accent"), std::string::npos);
    
    // Check for font variables
    EXPECT_NE(css.find("--font-heading"), std::string::npos);
    EXPECT_NE(css.find("--font-body"), std::string::npos);
    EXPECT_NE(css.find("--font-mono"), std::string::npos);
    
    // Check for font weight variables
    EXPECT_NE(css.find("--font-heading-weight"), std::string::npos);
    EXPECT_NE(css.find("--font-body-weight"), std::string::npos);
}

TEST_F(BrandKitTest, CSSVariablesContainAllColors) {
    std::string css = BrandKit::generateCSSVariables();
    auto colors = BrandKit::getAllColors();
    
    for (const auto& [colorName, colorData] : colors) {
        std::string varName = "--color-" + colorName;
        EXPECT_NE(css.find(varName), std::string::npos) 
            << "CSS should contain variable: " << varName;
        EXPECT_NE(css.find(colorData.hex), std::string::npos)
            << "CSS should contain color value: " << colorData.hex;
    }
}

TEST_F(BrandKitTest, CSSVariablesContainAllFonts) {
    std::string css = BrandKit::generateCSSVariables();
    auto fonts = BrandKit::getAllFonts();
    
    for (const auto& font : fonts) {
        std::string varName = "--font-" + font.name;
        EXPECT_NE(css.find(varName), std::string::npos)
            << "CSS should contain variable: " << varName;
        EXPECT_NE(css.find(font.family), std::string::npos)
            << "CSS should contain font family: " << font.family;
    }
}

// ==============================================================================
// JSON GENERATION TESTS
// ==============================================================================

TEST_F(BrandKitTest, GenerateJSONConfig) {
    std::string json = BrandKit::generateJSONConfig();
    EXPECT_FALSE(json.empty());
    
    // Check for JSON structure
    EXPECT_NE(json.find("\"colors\""), std::string::npos);
    EXPECT_NE(json.find("\"fonts\""), std::string::npos);
    
    // Check for color entries
    EXPECT_NE(json.find("\"primary\""), std::string::npos);
    EXPECT_NE(json.find("\"secondary\""), std::string::npos);
    
    // Check for font entries
    EXPECT_NE(json.find("\"heading\""), std::string::npos);
    EXPECT_NE(json.find("\"body\""), std::string::npos);
    EXPECT_NE(json.find("\"family\""), std::string::npos);
    EXPECT_NE(json.find("\"weight\""), std::string::npos);
}

TEST_F(BrandKitTest, JSONConfigContainsAllColors) {
    std::string json = BrandKit::generateJSONConfig();
    auto colors = BrandKit::getAllColors();
    
    for (const auto& [colorName, colorData] : colors) {
        EXPECT_NE(json.find("\"" + colorName + "\""), std::string::npos)
            << "JSON should contain color: " + colorName;
        EXPECT_NE(json.find(colorData.hex), std::string::npos)
            << "JSON should contain color value: " + colorData.hex;
    }
}

TEST_F(BrandKitTest, JSONConfigContainsAllFonts) {
    std::string json = BrandKit::generateJSONConfig();
    auto fonts = BrandKit::getAllFonts();
    
    for (const auto& font : fonts) {
        EXPECT_NE(json.find("\"" + font.name + "\""), std::string::npos)
            << "JSON should contain font: " + font.name;
        EXPECT_NE(json.find(font.family), std::string::npos)
            << "JSON should contain font family: " + font.family;
    }
}

// ==============================================================================
// FREE FUNCTION TESTS
// ==============================================================================

TEST_F(BrandKitTest, GetBrandColorFunction) {
    std::string color = getBrandColor("primary");
    EXPECT_EQ(color, "#6366F1");
    
    color = getBrandColor("secondary");
    EXPECT_EQ(color, "#8B5CF6");
    
    color = getBrandColor("nonexistent");
    EXPECT_EQ(color, "#000000");
}

TEST_F(BrandKitTest, GenerateBrandCSSFunction) {
    std::string css = generateBrandCSS();
    EXPECT_FALSE(css.empty());
    EXPECT_NE(css.find(":root"), std::string::npos);
    EXPECT_NE(css.find("--color-primary"), std::string::npos);
}

TEST_F(BrandKitTest, GenerateBrandJSONFunction) {
    std::string json = generateBrandJSON();
    EXPECT_FALSE(json.empty());
    EXPECT_NE(json.find("\"colors\""), std::string::npos);
    EXPECT_NE(json.find("\"fonts\""), std::string::npos);
}

// ==============================================================================
// THREAD SAFETY TESTS
// ==============================================================================

TEST_F(BrandKitTest, ConcurrentColorAccess) {
    std::vector<std::thread> threads;
    std::atomic<int> successCount{0};
    
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&successCount]() {
            for (int j = 0; j < 100; ++j) {
                std::string color = BrandKit::getColor("primary");
                if (color == "#6366F1") {
                    successCount++;
                }
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    EXPECT_EQ(successCount.load(), 1000);
}

TEST_F(BrandKitTest, ConcurrentCSSGeneration) {
    std::vector<std::thread> threads;
    std::atomic<int> successCount{0};
    
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&successCount]() {
            std::string css = BrandKit::generateCSSVariables();
            if (!css.empty() && css.find(":root") != std::string::npos) {
                successCount++;
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    EXPECT_EQ(successCount.load(), 10);
}

// ==============================================================================
// INTEGRATION TESTS
// ==============================================================================

TEST_F(BrandKitTest, CompleteWorkflow) {
    // Get all colors
    auto colors = BrandKit::getAllColors();
    EXPECT_FALSE(colors.empty());
    
    // Get all fonts
    auto fonts = BrandKit::getAllFonts();
    EXPECT_FALSE(fonts.empty());
    
    // Get all assets
    auto assets = BrandKit::getAllAssets();
    EXPECT_FALSE(assets.empty());
    
    // Generate CSS
    std::string css = BrandKit::generateCSSVariables();
    EXPECT_FALSE(css.empty());
    
    // Generate JSON
    std::string json = BrandKit::generateJSONConfig();
    EXPECT_FALSE(json.empty());
    
    // Verify consistency
    for (const auto& [colorName, colorData] : colors) {
        EXPECT_NE(css.find(colorData.hex), std::string::npos);
        EXPECT_NE(json.find(colorData.hex), std::string::npos);
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
