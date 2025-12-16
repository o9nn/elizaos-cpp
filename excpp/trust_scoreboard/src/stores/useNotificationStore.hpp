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

import create, { State } from "zustand";
;

interface NotificationStore extends State {
  notifications: Array<{
    type: string
    message: string
    description?: string
    txid?: string
  }>
  set: (x: any) => void
}

const useNotificationStore = create<NotificationStore>((set, _get) => ({
  notifications: [],
  set: (fn) => set(produce(fn)),
}))

default useNotificationStore

} // namespace elizaos
