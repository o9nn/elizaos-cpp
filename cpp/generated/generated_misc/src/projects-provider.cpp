#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-autocoder/src/providers/projects-provider.h"

std::shared_ptr<Provider> projectsProvider = object{
    object::pair{std::string("name"), std::string("PROJECTS_CONTEXT")}, 
    object::pair{std::string("description"), std::string("Provides context about active projects and their current status")}, 
    object::pair{std::string("dynamic"), true}, 
    object::pair{std::string("get"), [=](auto runtime, auto _message, auto _state = undefined) mutable
    {
        auto projectsService = runtime->getService(std::string("project-planning"));
        if (!isProjectPlanningService(projectsService)) {
            return object{
                object::pair{std::string("text"), std::string("Project planning service is not available.")}, 
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}
            };
        }
        auto projects = std::async([=]() { (as<std::shared_ptr<ProjectPlanningServiceInterface>>(projectsService))->listProjectPlans(); });
        if (projects->get_length() == 0) {
            return object{
                object::pair{std::string("text"), std::string("No projects found.")}, 
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}
            };
        }
        shared contextText = std::string("[PROJECTS]\
");
        auto activeProjects = projects->filter([=](auto p) mutable
        {
            return OR((OR((p->status == std::string("planning")), (p->status == std::string("generating")))), (p->status == std::string("testing")));
        }
        );
        auto completedProjects = projects->filter([=](auto p) mutable
        {
            return p->status == std::string("completed");
        }
        );
        auto failedProjects = projects->filter([=](auto p) mutable
        {
            return p->status == std::string("failed");
        }
        );
        if (activeProjects->get_length() > 0) {
            contextText += std::string("\
Active Projects:\
");
            activeProjects->forEach([=](auto project) mutable
            {
                contextText += std::string("- ") + project->name + std::string(" (") + project->type + std::string("): ") + project->status + std::string("\
");
                if (project->formId) {
                    contextText += std::string("  Form ID: ") + project->formId + std::string("\
");
                }
                if (project->details["projectName"]) {
                    contextText += std::string("  Project Name: ") + project->details["projectName"] + std::string("\
");
                }
                if (project->error) {
                    contextText += std::string("  Error: ") + project->error + std::string("\
");
                }
            }
            );
        }
        if (completedProjects->get_length() > 0) {
            contextText += std::string("\
Completed Projects:\
");
            completedProjects->forEach([=](auto project) mutable
            {
                contextText += std::string("- ") + project->name + std::string(" (") + project->type + std::string(")\
");
                if (project->details["projectName"]) {
                    contextText += std::string("  Project Name: ") + project->details["projectName"] + std::string("\
");
                }
                if (project->artifacts["files"]) {
                    contextText += std::string("  Generated ") + project->artifacts["files"]->get_length() + std::string(" files\
");
                }
            }
            );
        }
        if (failedProjects->get_length() > 0) {
            contextText += std::string("\
Failed Projects:\
");
            failedProjects->forEach([=](auto project) mutable
            {
                contextText += std::string("- ") + project->name + std::string(" (") + project->type + std::string(")\
");
                if (project->error) {
                    contextText += std::string("  Error: ") + project->error + std::string("\
");
                }
            }
            );
        }
        return object{
            object::pair{std::string("text"), contextText}, 
            object::pair{std::string("values"), object{
                object::pair{std::string("activeProjectsCount"), activeProjects->get_length()}, 
                object::pair{std::string("completedProjectsCount"), completedProjects->get_length()}, 
                object::pair{std::string("failedProjectsCount"), failedProjects->get_length()}, 
                object::pair{std::string("totalProjectsCount"), projects->get_length()}
            }}, 
            object::pair{std::string("data"), object{
                object::pair{std::string("projects"), std::string("projects")}, 
                object::pair{std::string("activeProjects"), std::string("activeProjects")}, 
                object::pair{std::string("completedProjects"), std::string("completedProjects")}, 
                object::pair{std::string("failedProjects"), std::string("failedProjects")}
            }}
        };
    }
    }
};

void Main(void)
{
}

MAIN
