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

using ServerEmojiProps = {
  url: string;
};
const ServerEmoji = ({ url }: ServerEmojiProps) => {
  return <img style={{ width: "24px", height: "24px" }} src={url} />;
};

default ServerEmoji;

} // namespace elizaos
