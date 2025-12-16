#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/// <reference no-default-lib="true"/>
/// <reference lib="es2022" />
/// <reference lib="dom" />
/// <reference lib="dom.iterable" />
/// <reference lib="webworker" />

// Type aliases for WebGL array types
using _Float32List = std::variant<Float32Array, ArrayLike<number>>;
using _Int32List = std::variant<Int32Array, ArrayLike<number>>;
using _Uint32List = std::variant<Uint32Array, ArrayLike<number>>;

// Fix pg-protocol
  struct NoticeMessage {
    double length;
    std::string name;
    std::string severity;
    std::string code;
    std::string message;
    std::optional<std::string> detail;
    std::optional<std::string> hint;
    std::optional<std::string> position;
    std::optional<std::string> internalPosition;
    std::optional<std::string> internalQuery;
    std::optional<std::string> where;
    std::optional<std::string> schema;
    std::optional<std::string> table;
    std::optional<std::string> column;
    std::optional<std::string> dataType;
    std::optional<std::string> constraint;
    std::optional<std::string> file;
    std::optional<std::string> line;
    std::optional<std::string> routine;
};

// React types for React Router

  struct ReactElement {
    std::any type;
    std::any props;
    std::any key;
};

  using ReactNode = std::variant<ReactElement, std::string, double, bool, nullptr, std::nullopt>;

// Fix React Router types

  struct SwitchProps {
    std::optional<std::any> children;
    std::optional<Location> location;
};

  struct PromptProps {
    std::optional<bool> when;
};

  struct RedirectProps {
    std::variant<std::string, Location> to;
    std::optional<bool> push;
    std::optional<std::string> from;
    std::optional<std::string> path;
    std::optional<bool> exact;
    std::optional<bool> strict;
};

  struct Location {
    std::string pathname;
    std::string search;
    std::string hash;
    std::optional<std::any> state;
};

  struct History {
    double length;
    std::string action;
    Location location;
};

// Fix MDX types
  using MDXComponents = {


} // namespace elizaos
