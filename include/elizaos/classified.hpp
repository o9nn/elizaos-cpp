#pragma once

#include "elizaos/core.hpp"
#include "elizaos/agentlogger.hpp"
#include <string>
#include <memory>

namespace elizaos {

/**
 * @brief Classified - Game/Challenge System
 * 
 * A game that is not a game. Perfectly safe, normal things.
 */

class ClassifiedGame {
public:
    ClassifiedGame();
    ~ClassifiedGame() = default;
    
    bool initialize();
    void runGame();
    void shutdown();
    
private:
    std::shared_ptr<AgentLogger> logger_;
};

} // namespace elizaos
