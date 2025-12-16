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

struct CardProps {
    React.ReactNode children;
    std::optional<std::string> className;
};


: CardProps) {
  return (
    <div className={`rounded-lg border bg-card text-card-foreground shadow-sm ${className}`}>
      {children}
    </div>
  );
}

: CardProps) {
  return <div className={`flex flex-col space-y-1.5 p-6 ${className}`}>{children}</div>;
}

: CardProps) {
  return <div className={`flex items-center p-6 pt-0 ${className}`}>{children}</div>;
}

: CardProps) {
  return (
    <h3 className={`text-2xl font-semibold leading-none tracking-tight ${className}`}>
      {children}
    </h3>
  );
}

: CardProps) {
  return <p className={`text-sm text-muted-foreground ${className}`}>{children}</p>;
}

: CardProps) {
  return <div className={`p-6 pt-0 ${className}`}>{children}</div>;
}

} // namespace elizaos
