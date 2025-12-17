#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include ".hyperfy/src/core/libs/csm/CSM.hpp"
#include ".hyperfy/src/core/systems/System.js.hpp"
#include ".managers/puppeteer-manager.js.hpp"
#include ".utils.js.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct SkyHandle {
    std::any node;
};

struct EnvironmentConfig {
    std::optional<std::string> bg;
    std::optional<std::string> hdr;
    std::optional<THREE::Vector3> sunDirection;
    std::optional<double> sunIntensity;
    std::optional<std::variant<std::string, double>> sunColor;
    std::optional<double> fogNear;
    std::optional<double> fogFar;
    std::optional<std::string> fogColor;
    std::optional<std::string> model;
};

class AgentEnvironment {
public:
    AgentEnvironment(const std::any& world);
    void start();
    void updateModel();
    SkyHandle addSky(const std::any& node);
    void getSky();
    void updateSky();
    void update(double delta);
    void lateUpdate(double delta);
    void buildCSM();
    void if(auto changes.model);
    void if(auto changes.shadows);
};


} // namespace elizaos
