#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/src/rules/project-overview.h"

any getComponentByPath(string path)
{
    auto entryPoint = ENTRY_POINTS->find([=](auto ep) mutable
    {
        return ep->path == path;
    }
    );
    if (entryPoint) {
        return object{
            object::pair{std::string("component"), std::string("entry-point")}, 
            object::pair{std::string("description"), entryPoint->description}
        };
    }
    if (path == MAIN_AGENT_CLASS->path) {
        return object{
            object::pair{std::string("component"), std::string("main-agent")}, 
            object::pair{std::string("description"), MAIN_AGENT_CLASS->description}
        };
    }
    if (path == SWE_ENV_CLASS->path) {
        return object{
            object::pair{std::string("component"), std::string("environment")}, 
            object::pair{std::string("description"), SWE_ENV_CLASS->description}
        };
    }
    auto inspector = INSPECTORS->find([=](auto i) mutable
    {
        return i->path == path;
    }
    );
    if (inspector) {
        return object{
            object::pair{std::string("component"), std::string("inspector")}, 
            object::pair{std::string("description"), inspector->description}
        };
    }
    if (path->startsWith(std::string("tools/"))) {
        return object{
            object::pair{std::string("component"), std::string("tool")}, 
            object::pair{std::string("description"), std::string("Tool bundle for agent actions")}
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
    object::pair{std::string("path"), std::string("sweagent/run/run_single.py")}, 
    object::pair{std::string("description"), std::string("Main entry point for running single agent instances")}, 
    object::pair{std::string("type"), std::string("single")}
}, object{
    object::pair{std::string("path"), std::string("sweagent/run/run_batch.py")}, 
    object::pair{std::string("description"), std::string("Entry point for batch processing, used for benchmarking")}, 
    object::pair{std::string("type"), std::string("batch")}
} };
std::shared_ptr<ClassInfo> MAIN_AGENT_CLASS = object{
    object::pair{std::string("name"), std::string("Agent")}, 
    object::pair{std::string("path"), std::string("sweagent/agent/agents.py")}, 
    object::pair{std::string("description"), std::string("Main class that governs agent behavior")}
};
std::shared_ptr<ClassInfo> SWE_ENV_CLASS = object{
    object::pair{std::string("name"), std::string("SWEEnv")}, 
    object::pair{std::string("path"), std::string("sweagent/environment/swe_env.py")}, 
    object::pair{std::string("description"), std::string("Class that interfaces with the SWE-ReX project to interface with sandboxed docker containers")}
};
std::shared_ptr<ExecutionEnvironment> EXECUTION_ENVIRONMENT = object{
    object::pair{std::string("type"), std::string("docker")}, 
    object::pair{std::string("description"), std::string("AI agent proposes actions that are executed in sandboxed docker containers")}, 
    object::pair{std::string("interfaceClass"), SWE_ENV_CLASS}, 
    object::pair{std::string("interfaceProject"), std::string("SWE-ReX")}
};
std::shared_ptr<ToolsInfo> TOOLS_INFO = object{
    object::pair{std::string("location"), std::string("tools/")}, 
    object::pair{std::string("organization"), std::string("bundles")}, 
    object::pair{std::string("deployment"), std::string("Copied to sandboxed container and made available in $PATH variable")}
};
array<std::shared_ptr<Inspector>> INSPECTORS = array<std::shared_ptr<Inspector>>{ object{
    object::pair{std::string("name"), std::string("CLI Inspector")}, 
    object::pair{std::string("path"), std::string("inspector_cli.py")}, 
    object::pair{std::string("type"), std::string("cli")}, 
    object::pair{std::string("description"), std::string("Command line interface for inspecting agent trajectories")}
}, object{
    object::pair{std::string("name"), std::string("Web Inspector")}, 
    object::pair{std::string("path"), std::string("sweagent/inspector/server.py")}, 
    object::pair{std::string("type"), std::string("web")}, 
    object::pair{std::string("description"), std::string("Web interface for inspecting agent trajectories")}
} };
std::shared_ptr<ProjectStructure> PROJECT_STRUCTURE = object{
    object::pair{std::string("mainEntryPoints"), ENTRY_POINTS}, 
    object::pair{std::string("mainClass"), MAIN_AGENT_CLASS}, 
    object::pair{std::string("executionEnvironment"), EXECUTION_ENVIRONMENT}, 
    object::pair{std::string("tools"), TOOLS_INFO}, 
    object::pair{std::string("inspectors"), INSPECTORS}
};
string PROJECT_OVERVIEW = std::string("SWE-agent implements an AI software engineering agent that uses language models to fix github issues.");
Record<string, string> TYPESCRIPT_EQUIVALENTS = object{
    object::pair{std::string("sweagent/run/run_single.py"), std::string("src/run/run-single.ts")}, 
    object::pair{std::string("sweagent/run/run_batch.py"), std::string("src/run/run-batch.ts")}, 
    object::pair{std::string("sweagent/agent/agents.py"), std::string("src/agent/agents.ts")}, 
    object::pair{std::string("sweagent/environment/swe_env.py"), std::string("src/environment/swe-env.ts")}, 
    object::pair{std::string("inspector_cli.py"), std::string("src/inspector/cli.ts")}, 
    object::pair{std::string("sweagent/inspector/server.py"), std::string("src/inspector/server.ts")}
};

void Main(void)
{
}

MAIN
