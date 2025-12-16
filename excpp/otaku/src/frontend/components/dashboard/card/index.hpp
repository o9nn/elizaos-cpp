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

interface DashboardCardProps
  extends Omit<React.ComponentProps<typeof Card>, "title"> {
  title: string;
  subtitle?: string;
  addon?: React.ReactNode;
  intent?: "default" | "success";
  children: React.ReactNode;
}

default : DashboardCardProps) {
  return (
    <Card className={className} {...props}>
      <CardHeader className="flex items-center justify-between">
        <div>
          <CardTitle className="flex items-center gap-2.5">
            <Bullet variant={intent} />
            {title}
          </CardTitle>
          {subtitle && (
            <p className="text-xs text-muted-foreground mt-1 ml-5">{subtitle}</p>
          )}
        </div>
        {addon && <div>{addon}</div>}
      </CardHeader>

      <CardContent className="flex-1 relative">{children}</CardContent>
    </Card>
  );
}

} // namespace elizaos
