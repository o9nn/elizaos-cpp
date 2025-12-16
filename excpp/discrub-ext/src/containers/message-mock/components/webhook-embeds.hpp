#include "...classes/message.hpp"
#include "...components/webhook-embed-mock.hpp"
#include "...utils.hpp"
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
;

using WebhookEmbedsProps = {
  message: Message;
};

const WebhookEmbeds = ({ message }: WebhookEmbedsProps) => {
  return (
    <Stack
      sx={{ maxWidth: "600px" }}
      mt="5px"
      direction="column"
      justifyContent="flex-start"
      alignItems="flex-start"
      spacing={1}
    >
      {getRichEmbeds(message).map((embed) => (
        <WebhookEmbedMock
          alwaysExpanded={true}
          embed={embed}
          message={message}
        />
      ))}
    </Stack>
  );
};
default WebhookEmbeds;

} // namespace elizaos
