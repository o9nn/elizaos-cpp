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

const isNotNullOrUndefined = <T>(value: T): value is NonNullable<T> =>
  value !== null && value !== undefined;

const isNotNullOrUndefinedArray = <T>(
  value: T[],
): value is NonNullable<T>[] => value.every((v) => isNotNullOrUndefined(v));

const isNotNullOrUndefinedField =
  <T, K extends keyof T>(field: K) =>
  (value: T): value is NonNullable<T> =>
    isNotNullOrUndefined(value[field]);

} // namespace elizaos
