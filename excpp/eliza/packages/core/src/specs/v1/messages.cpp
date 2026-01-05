#include "messages.hpp"
#include <iostream>
#include <stdexcept>
#include <sstream>

namespace elizaos {

std::future<std::vector<ActorDetails>> getActorDetails(UUID roomId, IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - stub implementation for C++
    
    std::promise<std::vector<ActorDetails>> promise;
    
    try {
        // TODO: Implement proper actor details retrieval
        std::cerr << "getActorDetails is not yet implemented." << std::endl;
        promise.set_value({});
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        promise.set_exception(std::current_exception());
    }
    
    return promise.get_future();
}

std::string formatActors(const std::vector<Actor>& actors) {
    // NOTE: Auto-converted from TypeScript - manually refined for C++
    
    if (actors.empty()) {
        return "No actors available.";
    }
    
    std::ostringstream oss;
    for (size_t i = 0; i < actors.size(); ++i) {
        oss << actors[i].name;
        if (i < actors.size() - 1) {
            oss << ", ";
        }
    }
    
    return oss.str();
}

} // namespace elizaos
