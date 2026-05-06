#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/src/run/remove-unfinished.h"

std::shared_ptr<Promise<void>> removeUnfinished(string baseDir, boolean dryRun)
{
    auto basePath = path->resolve(baseDir);
    auto toRemove = array<string>();
    auto directories = fs::readdirSync(basePath);
    auto& __array627_1915 = directories;
    for (auto __indx627_1915 = 0_N; __indx627_1915 < __array627_1915->get_length(); __indx627_1915++)
    {
        auto& dirName = const_(__array627_1915)[__indx627_1915];
        {
            shared directory = path->join(basePath, dirName);
            if (!fs::statSync(directory)->isDirectory()) {
                continue;
            }
            if (!dirName->includes(std::string("__"))) {
                continue;
            }
            auto trajs = fs::readdirSync(directory)->filter([=](auto file) mutable
            {
                return file->endsWith(std::string(".traj"));
            }
            )->map([=](auto file) mutable
            {
                return path->join(directory, file);
            }
            );
            if (trajs->get_length() == 0) {
                logger->info(std::string("No trajectories found in ") + directory + string_empty);
                continue;
            }
            if (trajs->get_length() > 1) {
                logger->warn(std::string("Found multiple trajectories in ") + directory + std::string(". Skipping."));
                continue;
            }
            try
            {
                auto traj = loadFile(const_(trajs)[0]);
                if (OR((!traj), (type_of(traj) != std::string("object")))) {
                    logger->warn(std::string("Invalid trajectory format in ") + const_(trajs)[0] + std::string(". Adding to remove list."));
                    toRemove->push(directory);
                    continue;
                }
                auto submission = OR(((as<std::shared_ptr<TrajectoryFile>>(traj))->info["submission"]), (nullptr));
                if (submission == nullptr) {
                    logger->warn(std::string("No submission found in ") + directory + std::string(". Adding to remove list."));
                    toRemove->push(directory);
                    continue;
                }
            }
            catch (const any& error)
            {
                logger->warn(std::string("Error loading trajectory ") + const_(trajs)[0] + std::string(": ") + error + std::string(". Adding to remove list."));
                toRemove->push(directory);
                continue;
            }
        }
    }
    if (dryRun) {
        logger->info(std::string("Would remove ") + toRemove->get_length() + std::string(" unfinished trajectories."));
        for (auto& directory : toRemove)
        {
            logger->info(directory);
        }
    } else {
        for (auto& directory : toRemove)
        {
            logger->info(std::string("Removing ") + directory + string_empty);
            fs::rmSync(directory, object{
                object::pair{std::string("recursive"), true}, 
                object::pair{std::string("force"), true}
            });
        }
    }
    return std::shared_ptr<Promise<void>>();
};


std::shared_ptr<AgentLogger> logger = getLogger(std::string("remove_unfinished"));

void Main(void)
{
}

MAIN
