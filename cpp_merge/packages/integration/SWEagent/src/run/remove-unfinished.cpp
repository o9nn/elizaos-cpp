#include "remove-unfinished.h"

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
            if (!dirName->includes(std:("__"))) {
                continue;
            }
            auto trajs = fs::readdirSync(directory)->filter([=](auto file) mutable
            {
                return file->endsWith(std:(".traj"));
            }
            )->map([=](auto file) mutable
            {
                return path->join(directory, file);
            }
            );
            if (trajs->get_length() == 0) {
                logger->info(std:("No trajectories found in ") + directory + string_empty);
                continue;
            }
            if (trajs->get_length() > 1) {
                logger->warn(std:("Found multiple trajectories in ") + directory + std:(". Skipping."));
                continue;
            }
            try
            {
                auto traj = loadFile(const_(trajs)[0]);
                if (OR((!traj), (type_of(traj) != std:("object")))) {
                    logger->warn(std:("Invalid trajectory format in ") + const_(trajs)[0] + std:(". Adding to remove list."));
                    toRemove->push(directory);
                    continue;
                }
                auto submission = OR(((as<std::shared_ptr<TrajectoryFile>>(traj))->info["submission"]), (nullptr));
                if (submission == nullptr) {
                    logger->warn(std:("No submission found in ") + directory + std:(". Adding to remove list."));
                    toRemove->push(directory);
                    continue;
                }
            }
            catch (const any& error)
            {
                logger->warn(std:("Error loading trajectory ") + const_(trajs)[0] + std:(": ") + error + std:(". Adding to remove list."));
                toRemove->push(directory);
                continue;
            }
        }
    }
    if (dryRun) {
        logger->info(std:("Would remove ") + toRemove->get_length() + std:(" unfinished trajectories."));
        for (auto& directory : toRemove)
        {
            logger->info(directory);
        }
    } else {
        for (auto& directory : toRemove)
        {
            logger->info(std:("Removing ") + directory + string_empty);
            fs::rmSync(directory, object{
                object::pair{std:("recursive"), true}, 
                object::pair{std:("force"), true}
            });
        }
    }
    return std::shared_ptr<Promise<void>>();
};


std::shared_ptr<AgentLogger> logger = getLogger(std:("remove_unfinished"));

void Main(void)
{
}

MAIN
