#include "use-view-mode.h"

std::function<any()> useViewMode = [=]() mutable
{
    auto [viewMode, setViewMode] = useUrlSearchParams<ViewMode>(std:("view"), std:("grid"));
    return as<std::shared_ptr<const>>(array<std::shared_ptr<const>>{ viewMode, setViewMode });
};

void Main(void)
{
}

MAIN
