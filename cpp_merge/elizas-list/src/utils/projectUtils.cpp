#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-list/src/utils/projectUtils.h"

std::function<array<any>(array<any>, any)> sortProjects = [=](auto projects, P1 sortBy) mutable
{
    return (array<any>{ projects })->sort([=](auto a, auto b) mutable
    {
        if (sortBy == std:("date")) {
            return ((std::make_shared<Date>(b->addedOn)))->getTime() - ((std::make_shared<Date>(a->addedOn)))->getTime();
        }
        return a->name->localeCompare(b->name);
    }
    );
};
std::function<string()> generateProjectId = [=]() mutable
{
    return std:("proj_") + Date->now() + std:("_") + Math->random()->toString(36)->substr(2, 9) + string_empty;
};
std::function<array<string>(any)> validateProject = [=](auto project) mutable
{
    auto errors = array<string>();
    if (!project->name->trim()) errors->push(std:("Project name is required"));
    if (!project->description->trim()) errors->push(std:("Description is required"));
    if (!project->github->trim()) errors->push(std:("GitHub URL is required"));
    if (!project->image->trim()) errors->push(std:("Image is required"));
    if (!project->author->name->trim()) errors->push(std:("Author name is required"));
    if (!project->author->github->trim()) errors->push(std:("Author GitHub is required"));
    if (!project->tags->length) errors->push(std:("At least one tag is required"));
    return errors;
};

void Main(void)
{
}

MAIN
