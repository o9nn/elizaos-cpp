// brandkit.hpp - Brand kit resources and management for ElizaOS
// Provides brand assets, colors, fonts, and styling guidelines

#ifndef ELIZAOS_BRANDKIT_HPP
#define ELIZAOS_BRANDKIT_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>

namespace elizaos {
namespace brandkit {

// ==============================================================================
// BRAND DATA STRUCTURES
// ==============================================================================

struct BrandColor {
    std::string name;
    std::string hex;
    std::string rgb;
    std::string description;
};

struct BrandFont {
    std::string name;
    std::string family;
    std::string weight;
    std::string usage;
};

struct BrandAsset {
    std::string name;
    std::string type;
    std::string url;
    std::string description;
};

// ==============================================================================
// BRANDKIT CLASS
// ==============================================================================

class BrandKit {
private:
    static std::mutex mutex_;

public:
    // Get brand color by name
    static std::string getColor(const std::string& colorName);

    // Get all brand colors
    static std::unordered_map<std::string, BrandColor> getAllColors();

    // Get brand font by name
    static BrandFont getFont(const std::string& fontName);

    // Get all brand fonts
    static std::vector<BrandFont> getAllFonts();

    // Get brand asset by name
    static BrandAsset getAsset(const std::string& assetName);

    // Get all brand assets
    static std::vector<BrandAsset> getAllAssets();

    // Generate CSS variables
    static std::string generateCSSVariables();

    // Generate JSON config
    static std::string generateJSONConfig();
};

// ==============================================================================
// FREE FUNCTIONS
// ==============================================================================

std::string getBrandColor(const std::string& colorName);
std::string generateBrandCSS();
std::string generateBrandJSON();

} // namespace brandkit
} // namespace elizaos

#endif // ELIZAOS_BRANDKIT_HPP
