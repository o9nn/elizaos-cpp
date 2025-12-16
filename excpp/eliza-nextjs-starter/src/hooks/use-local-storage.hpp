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
 * Custom hook to manage state backed by Local Storage.
 * @param key The key to use in Local Storage.
 * @param initialValue The initial value or a 
    try {
      const item = window.localStorage.getItem(key);
      // Parse stored json or if none return initialValue
      return item
        ? JSON.parse(item)
        : typeof initialValue === "function"
          ? (initialValue as () => T)()
          : initialValue;
    } catch (error) {
      // If error also return initialValue
      console.error(
        `Error reading localStorage key \u201C${key}\u201D:`,
        error,
      );
      return typeof initialValue === "function"
        ? (initialValue as () => T)()
        : initialValue;
    }
  });

  // Function to update the state and local storage
  const setValue = (value: T | ((val: T) => T)) => {
    if (typeof window === "undefined") {
      console.warn(
        `Tried setting localStorage key \u201C${key}\u201D even though environment is not a client`,
      );
      return;
    }

    try {
      // Allow value to be a \u201D:`,
        error,
      );
    }
  };

  // Effect to update state if local storage changes from another tab/window
  useEffect(() => {
    if (typeof window === "undefined") return;

    const handleStorageChange = (event: StorageEvent) => {
      if (event.key === key) {
        try {
          setStoredValue(
            event.newValue ? JSON.parse(event.newValue) : initialValue,
          );
        } catch (error) {
          console.error(
            `Error parsing stored value for key \u201C${key}\u201D:`,
            error,
          );
          setStoredValue(initialValue);
        }
      }
    };

    window.addEventListener("storage", handleStorageChange);
    return () => window.removeEventListener("storage", handleStorageChange);
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [key]); // Only re-run if key changes (shouldn't typically happen)

  return [storedValue, setValue];
}

default useLocalStorage;

} // namespace elizaos
