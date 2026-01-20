#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void DashboardStat(auto value, auto description, auto icon, auto tag, auto intent, auto direction) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto Icon = icon;

    // Extract prefix, numeric value, and suffix from the value string
    const auto parseValue = [&](val: string) {;
        // Match pattern: optional prefix + number + optional suffix
        const auto match = val.match(/^([^\d.-]*)([+-]?\d*\.?\d+)([^\d]*)$/);

        if (match) {
            const auto [, prefix, numStr, suffix] = match;
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

                const auto getIntentClassName = [&]() {;
                    if (intent == "positive") return "text-success";
                    if (intent == "negative") return "text-destructive";
                    return "text-muted-foreground";
                    };

                    const auto { prefix, numericValue, suffix, isNumeric } = parseValue(value);

                    return (;
                    <Card className="relative overflow-hidden">;
                    <CardHeader className="flex items-center justify-between">;
                    <CardTitle className="flex items-center gap-2.5">;
                    <Bullet />;
                {label}
                </CardTitle>;
                <Icon className="size-4 text-muted-foreground" />;
                </CardHeader>;

                <CardContent className="bg-accent flex-1 pt-2 md:pt-6 overflow-clip relative">
                <div className="flex items-center">;
                <span className="text-4xl md:text-5xl font-display">
                {isNumeric ? (;
                <NumberFlow;
            value={numericValue}
        prefix={prefix}
    suffix={suffix}
    />;
    ) : (
    value;
    )}
    </span>;
    {tag && (;
    <Badge variant="default" className="uppercase ml-3">;
    {tag}
    </Badge>;
    )}
    </div>;

    {description && (;
    <div className="justify-between">;
    <p className="text-xs md:text-sm font-medium text-muted-foreground tracking-wide">
    {description}
    </p>;
    </div>;
    )}

    {/* Marquee Animation */}
    {direction && (;
    <div className="absolute top-0 right-0 w-14 h-full pointer-events-none overflow-hidden group">;
    <div;
    className={cn(;
    "flex flex-col transition-all duration-500",
    "group-hover:scale-105 group-hover:brightness-110",
    getIntentClassName(),
    direction == "up";
    ? "animate-marquee-up";
    : "animate-marquee-down"
    )}
    >;
    <div;
    className={cn(;
    "flex",
    direction == "up" ? "flex-col-reverse" : "flex-col"
    )}
    >;
    {Array.from({ length: 6 }, (_, i) => (
    <Arrow key={i} direction={direction} index={i} />;
    ))}
    </div>;
    <div;
    className={cn(;
    "flex",
    direction == "up" ? "flex-col-reverse" : "flex-col"
    )}
    >;
    {Array.from({ length: 6 }, (_, i) => (
    <Arrow key={i} direction={direction} index={i} />;
    ))}
    </div>;
    </div>;
    </div>;
    )}
    </CardContent>;
    </Card>;
    );

}

} // namespace elizaos
