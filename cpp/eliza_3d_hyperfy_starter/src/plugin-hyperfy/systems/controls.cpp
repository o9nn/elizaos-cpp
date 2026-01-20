#include "controls.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void createButtonState() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return {
        $button: true,
        down: false,
        pressed: false,
        released: false,
    }

}

void bindRotations(auto quaternion, auto euler) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    euler._onChange(() => {
        quaternion.setFromEuler(euler, false);
        });
        quaternion._onChange(() => {
            euler.setFromQuaternion(quaternion, std::nullopt, false);
            });

}

} // namespace elizaos
