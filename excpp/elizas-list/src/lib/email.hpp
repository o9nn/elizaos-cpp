#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;
;
;

const transporter = createTransport({
  host: process.env.SMTP_HOST,
  port: parseInt(process.env.SMTP_PORT || '587'),
  auth: {
    user: process.env.SMTP_USER,
    pass: process.env.SMTP_PASS,
  },
});

async 
) {
  const html = render(ProjectNotification({ project }));

  await transporter.sendMail({
    from: '"ELIZAS LIST" <notifications@elizaslist.dev>',
    to,
    subject: `New Project: ${project.name}`,
    html,
  });
} 
} // namespace elizaos
