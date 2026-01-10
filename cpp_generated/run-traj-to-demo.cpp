#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/src/run/run-traj-to-demo.h"

void saveDemo(any data, string file, string trajPath)
{
    auto content = yaml->dump(data, object{
        object::pair{std::string("lineWidth"), -1}, 
        object::pair{std::string("noRefs"), true}, 
        object::pair{std::string("quotingType"), std::string(""")}, 
        object::pair{std::string("forceQuotes"), false}
    });
    auto header = DEMO_COMMENT->replace(std::string("{traj_path}"), trajPath);
    fs::writeFileSync(file, string_empty + header + std::string("\
") + content + string_empty);
};


void convertTrajToActionDemo(string trajPath, string outputFile, boolean includeUser)
{
    auto traj = JSON->parse(fs::readFileSync(trajPath, std::string("utf-8")));
    auto replayConfig = traj["replay_config"];
    if (type_of(traj["replay_config"]) == std::string("string")) {
        replayConfig = JSON->parse(traj["replay_config"]);
    }
    auto history = OR((traj["history"]), (array<any>()));
    shared copyFields = std::make_shared<Set>(array<string>{ std::string("content"), std::string("role"), std::string("tool_calls"), std::string("agent"), std::string("message_type"), std::string("tool_call_ids") });
    shared admissibleRoles = (includeUser) ? std::make_shared<Set>(array<string>{ std::string("assistant"), std::string("user"), std::string("tool") }) : std::make_shared<Set>(array<string>{ std::string("assistant") });
    auto filteredHistory = history["filter"]([=](auto step) mutable
    {
        return AND((AND((admissibleRoles->has(as<string>(step->role))), ((OR((OR((!step->agent), (step->agent == std::string("main")))), (step->agent == std::string("primary"))))))), (!step->is_demo));
    }
    )["map"]([=](auto step) mutable
    {
        auto filtered = object{};
        for (auto& key : copyFields)
        {
            if (in(key, step)) {
                filtered[key] = const_(step)[key];
            }
        }
        return filtered;
    }
    );
    auto outputData = object{
        object::pair{std::string("history"), filteredHistory}, 
        object::pair{std::string("replay_config"), replayConfig}
    };
    saveDemo(outputData, outputFile, trajPath);
    logger->info(std::string("Saved demo to ") + outputFile + string_empty);
};


void trajToDemo(string trajPath, string outputDir, string suffix, boolean overwrite, boolean includeUser)
{
    auto trajDir = path->dirname(trajPath);
    auto trajName = path->basename(trajPath, std::string(".traj"));
    auto outputSubdir = path->basename(trajDir) + suffix;
    auto outputFile = path->join(outputDir, outputSubdir, string_empty + trajName + std::string(".demo.yaml"));
    if (AND((fs::existsSync(outputFile)), (!overwrite))) {
        throw any(std::make_shared<Error>(std::string("Output file already exists: ") + outputFile + std::string(". Use --overwrite to overwrite.")));
    }
    auto outputFileDir = path->dirname(outputFile);
    if (!fs::existsSync(outputFileDir)) {
        fs::mkdirSync(outputFileDir, object{
            object::pair{std::string("recursive"), true}
        });
    }
    convertTrajToActionDemo(trajPath, outputFile, includeUser);
};


std::shared_ptr<AgentLogger> logger = getLogger(std::string("traj2demo"));
string DEMO_COMMENT = std::string("# This is a demo file generated from trajectory file:\
# {traj_path}\
# You can use this demo file to replay the actions in the trajectory with run_replay.\
# You can edit the content of the actions in this file to modify the replay behavior.\
# NOTICE:\
#         Only the actions of the assistant will be replayed.\
#         You do not need to modify the observation's contents or any other fields.\
#         You can add or remove actions to modify the replay behavior.");

void Main(void)
{
}

MAIN
