#include "use-view-mode.h"
#include <string>

std::function<any()> useViewMode = [=]() mutable
{
    auto [viewMode, setViewMode] = useUrlSearchParams<ViewMode>(std::string("view"), std::string("grid"));
    return as<std::shared_ptr<const>>(array<std::shared_ptr<const>>{ viewMode, setViewMode });
};

void Main(void)
{
}

MAIN
