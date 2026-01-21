// brandkit.cpp - Brand kit resources and management for ElizaOS
// Provides brand assets, colors, fonts, and styling guidelines

#include "elizaos/core.hpp"
#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>

namespace elizaos {
namespace brandkit {

// ==============================================================================
// BRAND COLORS
// ==============================================================================

struct BrandColor {
    std::string name;
    std::string hex;
    std::string rgb;
    std::string description;
};

static const std::unordered_map<std::string, BrandColor> BRAND_COLORS = {
    {"primary", {"Primary", "#6366F1", "rgb(99, 102, 241)", "Main brand color - Indigo"}},
    {"secondary", {"Secondary", "#8B5CF6", "rgb(139, 92, 246)", "Secondary brand color - Purple"}},
    {"accent", {"Accent", "#EC4899", "rgb(236, 72, 153)", "Accent color - Pink"}},
    {"success", {"Success", "#10B981", "rgb(16, 185, 129)", "Success state - Green"}},
    {"warning", {"Warning", "#F59E0B", "rgb(245, 158, 11)", "Warning state - Amber"}},
    {"error", {"Error", "#EF4444", "rgb(239, 68, 68)", "Error state - Red"}},
    {"info", {"Info", "#3B82F6", "rgb(59, 130, 246)", "Info state - Blue"}},
    {"background", {"Background", "#0F172A", "rgb(15, 23, 42)", "Dark background"}},
    {"surface", {"Surface", "#1E293B", "rgb(30, 41, 59)", "Surface color"}},
    {"text", {"Text", "#F1F5F9", "rgb(241, 245, 249)", "Primary text color"}},
    {"textSecondary", {"Text Secondary", "#94A3B8", "rgb(148, 163, 184)", "Secondary text color"}},
};

// ==============================================================================
// BRAND FONTS
// ==============================================================================

struct BrandFont {
    std::string name;
    std::string family;
    std::string weight;
    std::string usage;
};

static const std::vector<BrandFont> BRAND_FONTS = {
    {"heading", "Inter", "700", "Headings and titles"},
    {"body", "Inter", "400", "Body text and paragraphs"},
    {"mono", "Fira Code", "400", "Code and technical content"},
    {"display", "Inter", "900", "Large display text"},
};

// ==============================================================================
// BRAND ASSETS
// ==============================================================================

struct BrandAsset {
    std::string name;
    std::string type;
    std::string url;
    std::string description;
};

static const std::vector<BrandAsset> BRAND_ASSETS = {
    {"logo", "svg", "/assets/logo.svg", "Primary ElizaOS logo"},
    {"logoLight", "svg", "/assets/logo-light.svg", "Logo for light backgrounds"},
    {"logoDark", "svg", "/assets/logo-dark.svg", "Logo for dark backgrounds"},
    {"icon", "svg", "/assets/icon.svg", "App icon"},
    {"favicon", "ico", "/assets/favicon.ico", "Favicon"},
    {"ogImage", "png", "/assets/og-image.png", "Open Graph image"},
};

// ==============================================================================
// BRANDKIT API
// ==============================================================================

class BrandKit {
private:
    static std::mutex mutex_;
    
public:
    // Get brand color by name
    static std::string getColor(const std::string& colorName) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = BRAND_COLORS.find(colorName);
        return (it != BRAND_COLORS.end()) ? it->second.hex : "#000000";
    }
    
    // Get all brand colors
    static std::unordered_map<std::string, BrandColor> getAllColors() {
        std::lock_guard<std::mutex> lock(mutex_);
        return BRAND_COLORS;
    }
    
    // Get brand font by name
    static BrandFont getFont(const std::string& fontName) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& font : BRAND_FONTS) {
            if (font.name == fontName) {
                return font;
            }
        }
        return {"", "", "", ""};
    }
    
    // Get all brand fonts
    static std::vector<BrandFont> getAllFonts() {
        std::lock_guard<std::mutex> lock(mutex_);
        return BRAND_FONTS;
    }
    
    // Get brand asset by name
    static BrandAsset getAsset(const std::string& assetName) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& asset : BRAND_ASSETS) {
            if (asset.name == assetName) {
                return asset;
            }
        }
        return {"", "", "", ""};
    }
    
    // Get all brand assets
    static std::vector<BrandAsset> getAllAssets() {
        std::lock_guard<std::mutex> lock(mutex_);
        return BRAND_ASSETS;
    }
    
    // Generate CSS variables
    static std::string generateCSSVariables() {
        std::lock_guard<std::mutex> lock(mutex_);
        std::string css = ":root {\n";
        
        for (const auto& pair : BRAND_COLORS) {
            css += "  --color-" + pair.first + ": " + pair.second.hex + ";\n";
        }
        
        for (const auto& font : BRAND_FONTS) {
            css += "  --font-" + font.name + ": " + font.family + ";\n";
            css += "  --font-" + font.name + "-weight: " + font.weight + ";\n";
        }
        
        css += "}\n";
        return css;
    }
    
    // Generate JSON config
    static std::string generateJSONConfig() {
        std::lock_guard<std::mutex> lock(mutex_);
        std::string json = "{\n  \"colors\": {\n";
        
        bool first = true;
        for (const auto& pair : BRAND_COLORS) {
            if (!first) json += ",\n";
            json += "    \"" + pair.first + "\": \"" + pair.second.hex + "\"";
            first = false;
        }
        
        json += "\n  },\n  \"fonts\": {\n";
        
        first = true;
        for (const auto& font : BRAND_FONTS) {
            if (!first) json += ",\n";
            json += "    \"" + font.name + "\": {\"family\": \"" + font.family + 
                   "\", \"weight\": \"" + font.weight + "\"}";
            first = false;
        }
        
        json += "\n  }\n}\n";
        return json;
    }
};

std::mutex BrandKit::mutex_;

// ==============================================================================
// EXPORTED FUNCTIONS
// ==============================================================================

std::string getBrandColor(const std::string& colorName) {
    return BrandKit::getColor(colorName);
}

std::string generateBrandCSS() {
    return BrandKit::generateCSSVariables();
}

std::string generateBrandJSON() {
    return BrandKit::generateJSONConfig();
}

} // namespace brandkit
} // namespace elizaos
