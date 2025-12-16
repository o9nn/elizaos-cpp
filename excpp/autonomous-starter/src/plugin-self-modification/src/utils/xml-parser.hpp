#include ".types.hpp"
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

// Configure parser with security settings
const parser = new XMLParser({
  ignoreAttributes: false,
  attributeNamePrefix: "@_",
  parseAttributeValue: true,
  trimValues: true,
  // Security settings to prevent XXE attacks
  processEntities: false,
  stopNodes: ["*.script", "*.style"],
  parseTagValue: true,
});

const builder = new XMLBuilder({
  ignoreAttributes: false,
  attributeNamePrefix: "@_",
  format: true,
  indentBy: "  ",
  suppressEmptyNode: true,
  suppressBooleanAttributes: false,
});

// Sanitize XML input to prevent injection attacks
);

  return sanitized;
}

// Escape special XML characters


// Validate operation type




    const operations: ModificationOperation[] = [];
    const opsRoot = root.operations;

    if (opsRoot) {
      // Handle single operation or array of operations
      const processOperation = (op: any, type: string) => {
        if (!isValidOperationType(type)) {
          throw new Error(`Invalid operation type: ${type}`);
        }

        const items = Array.isArray(op) ? op : [op];
        items.forEach((item: any) => {
          // Validate path format
          const path = item["@_path"];
          if (!path || typeof path !== "string") {
            throw new Error(`Invalid path in ${type} operation`);
          }

          // Basic path validation - should not contain dangerous patterns
          if (path.includes("..") || path.includes("//")) {
            throw new Error(`Dangerous path pattern detected: ${path}`);
          }

          const operation: ModificationOperation = {
            type,
            path,
          };
          
          // Only add value for add and modify operations
          if (type !== "delete") {
            operation.value = item["#text"] || item;
            operation.dataType = item["@_type"];
          }
          
          operations.push(operation);
        });
      };

      // Check for any unknown operation types
      const validOps = ["add", "modify", "delete"];
      const opsKeys = Object.keys(opsRoot);
      for (const key of opsKeys) {
        if (!validOps.includes(key)) {
          throw new Error(`Invalid operation type: ${key}`);
        }
      }

      if (opsRoot.add) processOperation(opsRoot.add, "add");
      if (opsRoot.modify) processOperation(opsRoot.modify, "modify");
      if (opsRoot.delete) processOperation(opsRoot.delete, "delete");
    }

    // Validate reasoning
    const reasoning = root.reasoning;
    if (
      !reasoning ||
      typeof reasoning !== "string" ||
      reasoning.trim().length === 0
    ) {
      throw new Error("Missing or empty reasoning in character modification");
    }

    return {
      operations,
      reasoning: reasoning.trim(),
      timestamp: root.timestamp || new Date().toISOString(),
    };
  } catch (error) {
    throw new Error(
      `Failed to parse character diff XML: ${error instanceof Error ? error.message : String(error)}`,
    );
  }
}



  if (!Array.isArray(diff.operations)) {
    throw new Error("Operations must be an array");
  }

  // Validate each operation has a valid path
  for (const op of diff.operations) {
    if (!op.path || typeof op.path !== "string") {
      throw new Error(`Invalid path in operation: ${JSON.stringify(op)}`);
    }
  }

  const xmlObj = {
    "character-modification": {
      operations: {
        add: diff.operations
          .filter((op) => op.type === "add")
          .map((op) => ({
            "@_path": op.path,
            "@_type": op.dataType || "string",
            "#text": op.value,
          })),
        modify: diff.operations
          .filter((op) => op.type === "modify")
          .map((op) => ({
            "@_path": op.path,
            "@_type": op.dataType || "string",
            "#text": op.value,
          })),
        delete: diff.operations
          .filter((op) => op.type === "delete")
          .map((op) => ({
            "@_path": op.path,
          })),
      },
      reasoning: diff.reasoning,
      timestamp: diff.timestamp,
    },
  };

  // Remove empty operation arrays
  const ops = xmlObj["character-modification"].operations;
  if (ops.add.length === 0) delete ops.add;
  if (ops.modify.length === 0) delete ops.modify;
  if (ops.delete.length === 0) delete ops.delete;

  try {
    return builder.build(xmlObj);
  } catch (error) {
    throw new Error(
      `Failed to build character diff XML: ${error instanceof Error ? error.message : String(error)}`,
    );
  }
}

} // namespace elizaos
