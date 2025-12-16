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
 * A custom hook for handling partial updates of objects with nested JSONb fields.
 * This hook ensures that updates to nested objects and arrays are properly
 * managed when sending updates to the server.
 *
 * @param initialValue The initial state object
 * @returns A tuple containing:
 *   - The current state object
 *   - A  as T;
      }

      // Handle nested paths
      const pathParts = path.split('.');
      const fieldToUpdate = pathParts[0];
      const remainingPath = pathParts.slice(1).join('.');

      // Handle arrays in path (e.g., 'style.all.0')
      const isArrayIndex = !isNaN(Number(pathParts[1]));

      if (isArrayIndex) {
        const arrayName = pathParts[0];
        const index = Number(pathParts[1]);
        // Ensure we're working with an array and handle it safely
        const currentValue = prevValue[arrayName as keyof T];
        const array = Array.isArray(currentValue) ? [...currentValue] : [];

        if (pathParts.length === 2) {
          // Direct array item update
          array[index] = newValue;
        } else {
          // Updating a property of an object in an array
          const deeperPath = pathParts.slice(2).join('.');
          array[index] = updateNestedObject(array[index], deeperPath, newValue);
        }

        return {
          ...prevValue,
          [arrayName]: array,
        } as T;
      }

      // Special case for settings.secrets path
      if (path.startsWith('settings.secrets.')) {
        const secretKey = path.split('.')[2];

        const currentSettings = (prevValue as any).settings || {};
        const currentSecrets = currentSettings.secrets || {};

        const newSecrets = {
          ...currentSecrets,
          [secretKey]: newValue,
        };

        return {
          ...prevValue,
          settings: {
            ...currentSettings,
            secrets: newSecrets,
          },
        } as T;
      }

      // Handle regular nested objects
      const result = {
        ...prevValue,
        [fieldToUpdate]: updateNestedObject(
          prevValue[fieldToUpdate as keyof T],
          remainingPath,
          newValue
        ),
      } as T;

      return result;
    });
  }, []);

  /**
   * Helper  as unknown as K;
    }

    const [field, ...remainingPath] = path.split('.');
    const nextPath = remainingPath.join('.');

    return {
      ...obj,
      [field]: updateNestedObject((obj as any)[field] || {}, nextPath, value),
    } as unknown as K;
  };

  /**
   * Adds an item to an array field
   *
   * @param path Path to the array field
   * @param item Item to add
   */
  const addArrayItem = useCallback(<V>(path: string, item: V) => {
    setValue((prevValue) => {
      const pathParts = path.split('.');

      // Handle simple array field
      if (pathParts.length === 1) {
        const fieldName = pathParts[0];
        const currentArray = Array.isArray(prevValue[fieldName as keyof T])
          ? [...(prevValue[fieldName as keyof T] as unknown as V[])]
          : [];

        return {
          ...prevValue,
          [fieldName]: [...currentArray, item],
        } as T;
      }

      // Handle nested array field
      const updatePath = path;
      const currentValue = getNestedValue(prevValue, updatePath);
      const currentArray = Array.isArray(currentValue) ? [...currentValue] : [];

      return setNestedValue(prevValue, updatePath, [...currentArray, item]);
    });
  }, []);

  /**
   * Removes an item from an array field
   *
   * @param path Path to the array field
   * @param index Index of the item to remove
   */
  const removeArrayItem = useCallback((path: string, index: number) => {
    setValue((prevValue) => {
      const pathParts = path.split('.');

      // Handle simple array field
      if (pathParts.length === 1) {
        const fieldName = pathParts[0];
        const currentArray = Array.isArray(prevValue[fieldName as keyof T])
          ? [...(prevValue[fieldName as keyof T] as unknown as any[])]
          : [];

        if (index < 0 || index >= currentArray.length) return prevValue;

        return {
          ...prevValue,
          [fieldName]: [...currentArray.slice(0, index), ...currentArray.slice(index + 1)],
        } as T;
      }

      // Handle nested array field
      const updatePath = path;
      const currentValue = getNestedValue(prevValue, updatePath);

      if (!Array.isArray(currentValue) || index < 0 || index >= currentValue.length) {
        return prevValue;
      }

      const newArray = [...currentValue.slice(0, index), ...currentValue.slice(index + 1)];
      return setNestedValue(prevValue, updatePath, newArray);
    });
  }, []);

  /**
   * Helper 
      current = current[part];
    }

    return current;
  };

  /**
   * Helper  as O;
    }

    const [first, ...rest] = parts;
    const nextObj = (obj as any)[first] || {};

    return {
      ...obj,
      [first]: setNestedValue(nextObj, rest.join('.'), value),
    } as O;
  };

  /**
   * Resets to the initial state
   */
  const reset = useCallback(() => {
    setValue(initialValue);
  }, [initialValue]);

  // Special handling for updating the entire settings object
  const updateSettings = useCallback(
    (settings: any) => {
      setValue((prevValue) => {
        // Extract settings but remove 'secrets' key to avoid duplication
        const { secrets, avatar, ...otherSettings } = settings;

        // Only include avatar if it's a valid string; otherwise, omit it from the update
        const safeAvatar = typeof avatar === 'string' ? avatar : '';

        // Create the updated settings object
        const updatedSettings = {
          ...(prevValue as any).settings, // Start with existing settings
          ...otherSettings, // Add other settings (not secrets)
          avatar: safeAvatar,
        };

        // Only add secrets if it was included in the update
        if (secrets) {
          // Create a new secrets object that only contains non-null values
          const filteredSecrets: Record<string, any> = {};

          Object.entries(secrets).forEach(([key, value]) => {
            // If value is null, don't include it (this is how we delete)
            if (value !== null) {
              filteredSecrets[key] = value;
            }
          });

          updatedSettings.secrets = filteredSecrets;
        }

        const result = {
          ...prevValue,
          settings: updatedSettings,
        } as T;

        return result;
      });
    },
    [] // Remove value from dependencies to avoid unnecessary rerenders
  );

  return {
    value,
    updateField,
    addArrayItem,
    removeArrayItem,
    reset,
    updateSettings,
  };
}

} // namespace elizaos
