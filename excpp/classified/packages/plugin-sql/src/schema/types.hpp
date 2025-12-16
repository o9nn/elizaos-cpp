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

/**
 * Represents a custom type for converting a string to a JSONB format and vice versa.
 * @param {Object} options - The options for the custom type.
 * @param {Function} options.dataType - A  options.toDriver - A  options.fromDriver - A  - The custom type for string to JSONB conversion.
 */

const stringJsonb = customType<{ data: string; driverData: string }>({
  dataType() {
    return 'jsonb';
  },
  toDriver(value: string): string {
    return JSON.stringify(value);
  },
  fromDriver(value: string): string {
    return JSON.stringify(value);
  },
});

/**
 * Represents a custom type for converting a number to a timestamp string and vice versa.
 * @param {Object} options - The options for the custom type.
 * @param {Function} options.dataType - A  options.toDriver - A  options.fromDriver - A  - The custom type for number to timestamp conversion.
 */
const numberTimestamp = customType<{ data: number; driverData: string }>({
  dataType() {
    return 'timestamptz';
  },
  toDriver(value: number): string {
    return new Date(value).toISOString();
  },
  fromDriver(value: string): number {
    return new Date(value).getTime();
  },
});

} // namespace elizaos
