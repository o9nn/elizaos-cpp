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

struct ChatImageProps {
    std::string imageUrl;
    std::optional<std::string> caption;
    std::string author;
    std::string timestamp;
};


const ChatImage: React.FC<ChatImageProps> = ({ imageUrl, caption }) => {
  return (
    <div className="max-w-md mx-auto overflow-hidden">
      <div className="relative">
        <img
          src={imageUrl}
          alt={caption || "Chat image"}
          className="w-full h-auto object-cover"
          loading="lazy"
        />
        {caption && (
          <div className="absolute bottom-0 left-0 right-0 bg-black bg-opacity-50 text-white p-2">
            {caption}
          </div>
        )}
      </div>
    </div>
  );
};

} // namespace elizaos
