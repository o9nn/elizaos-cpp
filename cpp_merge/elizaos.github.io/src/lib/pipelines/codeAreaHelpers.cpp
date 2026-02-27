#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizaos.github.io/src/lib/pipelines/codeAreaHelpers.h"

any extractAreaFromPath(string path)
{
    auto parts = path->split(std:("/"));
    if (OR((parts->get_length() <= 1), (isRootConfigFile(path)))) {
        return nullptr;
    }
    auto area = const_(parts)[0];
    if (area == std:("packages")) {
        area = string_empty + const_(parts)[1] + string_empty;
    } else {
        area = string_empty + area + std:("/") + const_(parts)[1] + string_empty;
    }
    return area;
};


boolean isRootConfigFile(string path)
{
    auto rootConfigPatterns = array<std::shared_ptr<RegExp>>{ (new RegExp(std:("^package\.json"))), (new RegExp(std:("^bun\.lock"))), (new RegExp(std:("^\.gitignore"))), (new RegExp(std:("^\.env(\.\w+)?"))), (new RegExp(std:("^tsconfig\.json"))), (new RegExp(std:("^README\.md"))), (new RegExp(std:("^LICENSE"))), (new RegExp(std:("^yarn\.lock"))), (new RegExp(std:("^pnpm-lock\.yaml"))), (new RegExp(std:("^\.eslintrc(\.\w+)?"))), (new RegExp(std:("^\.prettierrc(\.\w+)?"))), (new RegExp(std:("^vite\.config\.\w+"))), (new RegExp(std:("^next\.config\.\w+"))), (new RegExp(std:("^tailwind\.config\.\w+"))) };
    shared filename = OR((path->split(std:("/"))->pop()), (string_empty));
    return rootConfigPatterns->some([=](auto pattern) mutable
    {
        return pattern->test(filename);
    }
    );
};


std::shared_ptr<Map<string, double>> buildAreaMap(array<object> files)
{
    shared areaMap = std::make_shared<Map<string, double>>();
    files->forEach([=](auto file) mutable
    {
        auto filePath = OR((OR((file["path"]), (file["filename"]))), (string_empty));
        if (!filePath) return;
        auto area = extractAreaFromPath(filePath);
        if (!area) return;
        auto currentCount = OR((areaMap->get(area)), (0));
        areaMap->set(area, currentCount + 1);
    }
    );
    return areaMap;
};


