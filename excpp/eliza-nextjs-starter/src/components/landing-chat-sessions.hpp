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

"use client";

;
;
;

const LandingChatSessions = () => {
  const [userEntity, setUserEntity] = useState<string | null>(null);

  // Initialize user entity on client side only to avoid hydration mismatch
  useEffect(() => {
    if (typeof window !== "undefined") {
      const storedEntity = localStorage.getItem("elizaHowUserEntity");
      if (storedEntity) {
        setUserEntity(storedEntity);
      } else {
        const newEntity = uuidv4();
        localStorage.setItem("elizaHowUserEntity", newEntity);
        setUserEntity(newEntity);
      }
    }
  }, []);

  return (
    <div className="w-full">
      <ChatSessions userId={userEntity} />
    </div>
  );
};

default LandingChatSessions;

} // namespace elizaos
