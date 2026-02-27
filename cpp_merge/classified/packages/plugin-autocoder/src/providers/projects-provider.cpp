#include "projects-provider.h"

std::shared_ptr<Provider> projectsProvider = object{
    object::pair{std:("name"), std:("PROJECTS_CONTEXT")}, 
    object::pair{std:("description"), std:("Provides context about active projects and their current status")}, 
    object::pair{std:("dynamic"), true}, 
    object::pair{std:("get"), [=](auto runtime, auto _message, auto _state = undefined) mutable
    {
        auto projectsService = runtime->getService(std:("project-planning"));
        if (!isProjectPlanningService(projectsService)) {
            return object{
                object::pair{std:("text"), std:("Project planning service is not available.")}, 
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}
            };
        }
        auto projects = std::async([=]() { (as<std::shared_ptr<ProjectPlanningServiceInterface>>(projectsService))->listProjectPlans(); });
        if (projects->get_length() == 0) {
            return object{
                object::pair{std:("text"), std:("No projects found.")}, 
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}
            };
        }
        shared contextText = std:("[PROJECTS]\
");
        auto activeProjects = projects->filter([=](auto p) mutable
        {
            return OR((OR((p->status == std:("planning")), (p->status == std:("generating")))), (p->status == std:("testing")));
        }
        );
        auto completedProjects = projects->filter([=](auto p) mutable
        {
            return p->status == std:("completed");
        }
        );
        auto failedProjects = projects->filter([=](auto p) mutable
        {
            return p->status == std:("failed");
        }
        );
        if (activeProjects->get_length() > 0) {
            contextText += std:("\
Active Projects:\
");
            activeProjects->forEach([=](auto project) mutable
            {
                contextText += std:("- ") + project->name + std:(" (") + project->type + std:("): ") + project->status + std:("\
");
                if (project->formId) {
                    contextText += std:("  Form ID: ") + project->formId + std:("\
");
                }
                if (project->details["projectName"]) {
                    contextText += std:("  Project Name: ") + project->details["projectName"] + std:("\
");
                }
                if (project->error) {
                    contextText += std:("  Error: ") + project->error + std:("\
");
                }
            }
            );
        }
        if (completedProjects->get_length() > 0) {
            contextText += std:("\
Completed Projects:\
");
            completedProjects->forEach([=](auto project) mutable
            {
                contextText += std:("- ") + project->name + std:(" (") + project->type + std:(")\
");
                if (project->details["projectName"]) {
                    contextText += std:("  Project Name: ") + project->details["projectName"] + std:("\
");
                }
                if (project->artifacts["files"]) {
                    contextText += std:("  Generated ") + project->artifacts["files"]->get_length() + std:(" files\
");
                }
            }
            );
        }
        if (failedProjects->get_length() > 0) {
            contextText += std:("\
Failed Projects:\
");
            failedProjects->forEach([=](auto project) mutable
            {
                contextText += std:("- ") + project->name + std:(" (") + project->type + std:(")\
");
                if (project->error) {
                    contextText += std:("  Error: ") + project->error + std:("\
");
                }
            }
            );
        }
        return object{
            object::pair{std:("text"), contextText}, 
            object::pair{std:("values"), object{
                object::pair{std:("activeProjectsCount"), activeProjects->get_length()}, 
                object::pair{std:("completedProjectsCount"), completedProjects->get_length()}, 
                object::pair{std:("failedProjectsCount"), failedProjects->get_length()}, 
                object::pair{std:("totalProjectsCount"), projects->get_length()}
            }}, 
            object::pair{std:("data"), object{
                object::pair{std:("projects"), std:("projects")}, 
                object::pair{std:("activeProjects"), std:("activeProjects")}, 
                object::pair{std:("completedProjects"), std:("completedProjects")}, 
                object::pair{std:("failedProjects"), std:("failedProjects")}
            }}
        };
    }
    }
};

void Main(void)
{
}

MAIN
