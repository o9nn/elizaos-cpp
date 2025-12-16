#include "ui/button.hpp"
#include "ui/card.hpp"
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

struct ButtonConfig {
    std::optional<std::string> label;
    std::optional<React.ReactNode> icon;
    std::optional<() => void> action;
    std::optional<std::string> className;
    std::optional<'default' | 'sm' | 'lg' | 'icon' | null | undefined> size;
    std::optional<bool> disabled;
};


struct ProfileCardProps {
    React.ReactNode title;
    React.ReactNode content;
    std::vector<ButtonConfig> buttons;
    std::optional<std::string> className;
};


default : ProfileCardProps) {
  return (
    <Card className={`h-full ${className || ''}`}>
      <CardHeader className="pb-2 pt-3">
        <CardTitle className="text-base truncate max-w-48">{title}</CardTitle>
      </CardHeader>
      <CardContent className="p-3">
        <div
          className="rounded-md bg-muted w-full mb-3 overflow-hidden"
          style={{ aspectRatio: '1 / 1' }}
        >
          <div className="flex items-center justify-center h-full">
            <div className="text-4xl font-bold uppercase h-full flex items-center justify-center w-full">
              {content}
            </div>
          </div>
        </div>
      </CardContent>
      <CardFooter className="p-3 pb-4">
        <div className="flex items-center gap-2 w-full justify-center">
          {buttons.map(({ label, icon, action, className, variant, size, disabled }, index) => (
            <Button
              key={index}
              variant={variant}
              className={className}
              onClick={action}
              size={size}
              disabled={disabled}
            >
              {icon}
              {label && <span>{label}</span>}
            </Button>
          ))}
        </div>
      </CardFooter>
    </Card>
  );
}

} // namespace elizaos
