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
;
;
;
;

struct DashboardStatProps {
    std::string label;
    std::string value;
    std::optional<std::string> description;
    std::optional<std::string> tag;
    React.ElementType icon;
    std::optional<"positive" | "negative" | "neutral"> intent;
    std::optional<"up" | "down"> direction;
};


default : DashboardStatProps) {
  const Icon = icon;

  // Extract prefix, numeric value, and suffix from the value string
  const parseValue = (val: string) => {
    // Match pattern: optional prefix + number + optional suffix
    const match = val.match(/^([^\d.-]*)([+-]?\d*\.?\d+)([^\d]*)$/);

    if (match) {
      const [, prefix, numStr, suffix] = match;
      return {
        prefix: prefix || "",
        numericValue: parseFloat(numStr),
        suffix: suffix || "",
        isNumeric: !isNaN(parseFloat(numStr)),
      };
    }

    return {
      prefix: "",
      numericValue: 0,
      suffix: val,
      isNumeric: false,
    };
  };

  const getIntentClassName = () => {
    if (intent === "positive") return "text-success";
    if (intent === "negative") return "text-destructive";
    return "text-muted-foreground";
  };

  const { prefix, numericValue, suffix, isNumeric } = parseValue(value);

  return (
    <Card className="relative overflow-hidden">
      <CardHeader className="flex items-center justify-between">
        <CardTitle className="flex items-center gap-2.5">
          <Bullet />
          {label}
        </CardTitle>
        <Icon className="size-4 text-muted-foreground" />
      </CardHeader>

      <CardContent className="bg-accent flex-1 pt-2 md:pt-6 overflow-clip relative">
        <div className="flex items-center">
          <span className="text-4xl md:text-5xl font-display">
            {isNumeric ? (
              <NumberFlow
                value={numericValue}
                prefix={prefix}
                suffix={suffix}
              />
            ) : (
              value
            )}
          </span>
          {tag && (
            <Badge variant="default" className="uppercase ml-3">
              {tag}
            </Badge>
          )}
        </div>

        {description && (
          <div className="justify-between">
            <p className="text-xs md:text-sm font-medium text-muted-foreground tracking-wide">
              {description}
            </p>
          </div>
        )}

        {/* Marquee Animation */}
        {direction && (
          <div className="absolute top-0 right-0 w-14 h-full pointer-events-none overflow-hidden group">
            <div
              className={cn(
                "flex flex-col transition-all duration-500",
                "group-hover:scale-105 group-hover:brightness-110",
                getIntentClassName(),
                direction === "up"
                  ? "animate-marquee-up"
                  : "animate-marquee-down"
              )}
            >
              <div
                className={cn(
                  "flex",
                  direction === "up" ? "flex-col-reverse" : "flex-col"
                )}
              >
                {Array.from({ length: 6 }, (_, i) => (
                  <Arrow key={i} direction={direction} index={i} />
                ))}
              </div>
              <div
                className={cn(
                  "flex",
                  direction === "up" ? "flex-col-reverse" : "flex-col"
                )}
              >
                {Array.from({ length: 6 }, (_, i) => (
                  <Arrow key={i} direction={direction} index={i} />
                ))}
              </div>
            </div>
          </div>
        )}
      </CardContent>
    </Card>
  );
}

struct ArrowProps {
    "up" | "down" direction;
    double index;
};


const Arrow = ({ direction, index }: ArrowProps) => {
  const staggerDelay = index * 0.15; // Faster stagger
  const phaseDelay = (index % 3) * 0.8; // Different phase groups

  return (
    <span
      style={{
        animationDelay: `${staggerDelay + phaseDelay}s`,
        animationDuration: "3s",
        animationTimingFunction: "cubic-bezier(0.4, 0.0, 0.2, 1)",
      }}
      className={cn(
        "text-center text-5xl size-14 font-display leading-none block",
        "transition-all duration-700 ease-out",
        "animate-marquee-pulse",

        "will-change-transform"
      )}
    >
      {direction === "up" ? "" : ""}
    </span>
  );
};

} // namespace elizaos
