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

using TTransactionSpeed = std::variant<"fast", "turbo", "ultra">;

const useTransactionSpeed = () => {
  const [transactionSpeed, setTransactionSpeed] =
    useLocalStorage<TTransactionSpeed>("use-transaction-speed", "turbo");
  return [transactionSpeed, setTransactionSpeed] as const;
};

} // namespace elizaos
