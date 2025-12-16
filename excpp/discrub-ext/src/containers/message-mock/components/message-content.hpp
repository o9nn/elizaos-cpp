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

using MessageContentProps = {
  id: string;
  isReply?: boolean;
  rawHtml: string;
};

const MessageContent = ({
  id,
  isReply = false,
  rawHtml,
}: MessageContentProps) => {
  const theme = useTheme();
  return (
    <Typography
      id={id}
      variant={isReply ? "caption" : "body1"}
      sx={{
        display: "block",
        color: isReply
          ? theme.palette.text.disabled
          : theme.palette.text.secondary,
        whiteSpace: isReply ? "nowrap" : "pre-line",
        overflow: isReply ? "hidden" : undefined,
        wordBreak: "break-word",
      }}
      dangerouslySetInnerHTML={{ __html: rawHtml }}
    />
  );
};
default MessageContent;

} // namespace elizaos
