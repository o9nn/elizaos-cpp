#include "project-overview.h"

any getComponentByPath(string path)
{
    auto entryPoint = ENTRY_POINTS->find([=](auto ep) mutable
    {
        return ep->path == path;
    }
    );
    if (entryPoint) {
        return object{
            object::pair{std:("component"), std:("entry-point")}, 
            object::pair{std:("description"), entryPoint->description}
        };
    }
    if (path == MAIN_AGENT_CLASS->path) {
        return object{
            object::pair{std:("component"), std:("main-agent")}, 
            object::pair{std:("description"), MAIN_AGENT_CLASS->description}
        };
    }
    if (path == SWE_ENV_CLASS->path) {
        return object{
            object::pair{std:("component"), std:("environment")}, 
            object::pair{std:("description"), SWE_ENV_CLASS->description}
        };
    }
    auto inspector = INSPECTORS->find([=](auto i) mutable
    {
        return i->path == path;
    }
    );
    if (inspector) {
        return object{
            object::pair{std:("component"), std:("inspector")}, 
            object::pair{std:("description"), inspector->description}
        };
    }
    if (path->startsWith(std:("tools/"))) {
        return object{
            object::pair{std:("component"), std:("tool")}, 
            object::pair{std:("description"), std:("Tool bundle for agent actions")}
        };
    }
    return nullptr;
};


array<string> getPythonModules()
{
    auto modules = array<string>();
    modules->push(const_(ENTRY_POINTS->map([=](auto ep) mutable
    {
        return ep->path;
    }
    ))[0]);
    modules->push(MAIN_AGENT_CLASS->path);
    modules->push(SWE_ENV_CLASS->path);
    modules->push(const_(INSPECTORS->map([=](auto i) mutable
    {
        return i->path;
    }
    ))[0]);
    return modules;
};


array<std::shared_ptr<EntryPoint>> ENTRY_POINTS = array<std::shared_ptr<EntryPoint>>{ object{
    object::pair{std:("path"), std:("sweagent/run/run_single.py")}, 
    object::pair{std:("description"), std:("Main entry point for running single agent instances")}, 
    object::pair{std:("type"), std:("single")}
}, object{
    object::pair{std:("path"), std:("sweagent/run/run_batch.py")}, 
    object::pair{std:("description"), std:("Entry point for batch processing, used for benchmarking")}, 
    object::pair{std:("type"), std:("batch")}
} };
std::shared_ptr<ClassInfo> MAIN_AGENT_CLASS = object{
    object::pair{std:("name"), std:("Agent")}, 
    object::pair{std:("path"), std:("sweagent/agent/agents.py")}, 
    object::pair{std:("description"), std:("Main class that governs agent behavior")}
};
std::shared_ptr<ClassInfo> SWE_ENV_CLASS = object{
    object::pair{std:("name"), std:("SWEEnv")}, 
    object::pair{std:("path"), std:("sweagent/environment/swe_env.py")}, 
    object::pair{std:("description"), std:("Class that interfaces with the SWE-ReX project to interface with sandboxed docker containers")}
};
std::shared_ptr<ExecutionEnvironment> EXECUTION_ENVIRONMENT = object{
    object::pair{std:("type"), std:("docker")}, 
    object::pair{std:("description"), std:("AI agent proposes actions that are executed in sandboxed docker containers")}, 
    object::pair{std:("interfaceClass"), SWE_ENV_CLASS}, 
    object::pair{std:("interfaceProject"), std:("SWE-ReX")}
};
std::shared_ptr<ToolsInfo> TOOLS_INFO = object{
    object::pair{std:("location"), std:("tools/")}, 
    object::pair{std:("organization"), std:("bundles")}, 
    object::pair{std:("deployment"), std:("Copied to sandboxed container and made available in $PATH variable")}
};
array<std::shared_ptr<Inspector>> INSPECTORS = array<std::shared_ptr<Inspector>>{ object{
    object::pair{std:("name"), std:("CLI Inspector")}, 
    object::pair{std:("path"), std:("inspector_cli.py")}, 
    object::pair{std:("type"), std:("cli")}, 
    object::pair{std:("description"), std:("Command line interface for inspecting agent trajectories")}
}, object{
    object::pair{std:("name"), std:("Web Inspector")}, 
    object::pair{std:("path"), std:("sweagent/inspector/server.py")}, 
    object::pair{std:("type"), std:("web")}, 
    object::pair{std:("description"), std:("Web interface for inspecting agent trajectories")}
} };
std::shared_ptr<ProjectStructure> PROJECT_STRUCTURE = object{
    object::pair{std:("mainEntryPoints"), ENTRY_POINTS}, 
    object::pair{std:("mainClass"), MAIN_AGENT_CLASS}, 
    object::pair{std:("executionEnvironment"), EXECUTION_ENVIRONMENT}, 
    object::pair{std:("tools"), TOOLS_INFO}, 
    object::pair{std:("inspectors"), INSPECTORS}
};
string PROJECT_OVERVIEW = std:("SWE-agent implements an AI software engineering agent that uses language models to fix github issues.");
Record<string, string> TYPESCRIPT_EQUIVALENTS = object{
    object::pair{std:("sweagent/run/run_single.py"), std:("src/run/run-single.ts")}, 
    object::pair{std:("sweagent/run/run_batch.py"), std:("src/run/run-batch.ts")}, 
    object::pair{std:("sweagent/agent/agents.py"), std:("src/agent/agents.ts")}, 
    object::pair{std:("sweagent/environment/swe_env.py"), std:("src/environment/swe-env.ts")}, 
    object::pair{std:("inspector_cli.py"), std:("src/inspector/cli.ts")}, 
    object::pair{std:("sweagent/inspector/server.py"), std:("src/inspector/server.ts")}
};

void Main(void)
{
}

MAIN
