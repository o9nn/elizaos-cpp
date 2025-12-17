#include "email.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> sendProjectNotification(const std::string& to, const std::any& project) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto html = render(ProjectNotification({ project }));

    transporter.sendMail({
        from: ""ELIZAS LIST" <notifications@elizaslist.dev>",
        to,
        "subject: " + "New Project: " + project.name
        html,
        });

}

} // namespace elizaos
