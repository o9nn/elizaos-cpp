#include "messages.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> getActorDetails(auto {
  runtime, auto roomId, const std::any& }) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        throw std::runtime_error('getActorDetails is not implemented.');

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

void formatActors(const std::any& { actors }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!actors || actors.length == 0) {
        return 'No actors available.';
    }
    return actors.map((actor) => actor.name).join(', ');

}

} // namespace elizaos
