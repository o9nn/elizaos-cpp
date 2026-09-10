#pragma once

#include <string>

#include "dreamgen_provider.hpp"
#include "koboldcpp_provider.hpp"

namespace elizaos {

/**
 * Metadata for a provider utility library. This class does not register itself
 * with an ElizaOS runtime; consumers call the stateless facades directly and
 * explicitly supply an HTTP transport.
 */
class DgenKoboldPlugin {
public:
    static constexpr const char* PLUGIN_NAME = "dgen_kobold_provider";
    static constexpr const char* PLUGIN_DESCRIPTION =
        "Transport-injected DreamGen and KoboldCpp provider utility library";

    static std::string getName() { return PLUGIN_NAME; }
    static std::string getDescription() { return PLUGIN_DESCRIPTION; }
};

} // namespace elizaos
