#include "sidebar.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void useSidebar() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto context = React.useContext(SidebarContext);
        if (!context) {
            throw std::runtime_error('useSidebar must be used within a SidebarProvider.');
        }

        return context;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
