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

 catch {
      // If not valid JSON, return as is (for strings, etc.)
      return param as unknown as T;
    }
  };

  const [value, setValue] = useState<T>(getValueFromUrl());

  // Update state when URL changes
  useEffect(() => {
    setValue(getValueFromUrl());
  }, [searchParams]);

  // Update URL when state changes
  const updateValue = useCallback(
    (newValue: T | ((val: T) => T)) => {
      const updatedValue =
        typeof newValue === "function"
          ? (newValue as (val: T) => T)(value)
          : newValue;

      setValue(updatedValue);

      const newParams = new URLSearchParams(searchParams);

      if (
        typeof updatedValue === "string" ||
        typeof updatedValue === "number" ||
        typeof updatedValue === "boolean"
      ) {
        newParams.set(key, String(updatedValue));
      } else {
        // For objects and arrays, stringify to JSON
        newParams.set(key, JSON.stringify(updatedValue));
      }

      setSearchParams(newParams);
    },
    [key, value, searchParams, setSearchParams],
  );

  return [value, updateValue];
}

} // namespace elizaos
