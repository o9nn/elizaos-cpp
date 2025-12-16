#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "..action.hpp"
#include "..service.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class ShellStatefulE2ETestSuite {
public:
    void if(auto !shellService);
    void if(auto !shellService);
    void for(auto const { cmd, auto expected } of commands);
    void if(auto !shellService);
    void if(auto !shellService);
    void catch(auto error);


} // namespace elizaos
