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

using V0ContextType = {
  isV0: boolean;
};

const V0Context = createContext<V0ContextType | undefined>(undefined);

using V0ProviderProps = {
  children: ReactNode;
  isV0: boolean;
};

const V0Provider = ({ children, isV0 }: V0ProviderProps) => {
  return <V0Context.Provider value={{ isV0 }}>{children}</V0Context.Provider>;
};

const useIsV0 = (): boolean => {
  const context = useContext(V0Context);
  if (context === undefined) {
    throw new Error("useIsV0 must be used within a V0Provider");
  }
  return context.isV0;
};

} // namespace elizaos
