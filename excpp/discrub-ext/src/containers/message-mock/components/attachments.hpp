#include "...classes/message.hpp"
#include "...components/attachment-mock.hpp"
#include "...components/embed-mock.hpp"
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

using AttachmentsProps = { message: Message };

const Attachments = ({ message }: AttachmentsProps) => {
  return (
    <Stack
      mt="5px"
      direction="column"
      justifyContent="flex-start"
      alignItems="flex-start"
      spacing={1}
    >
      {message.attachments.map((attachment) => (
        <AttachmentMock attachment={attachment} />
      ))}
      {message.embeds.map((embed, index) => (
        <EmbedMock embed={embed} index={index} />
      ))}
    </Stack>
  );
};
default Attachments;

} // namespace elizaos
