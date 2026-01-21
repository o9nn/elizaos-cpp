#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/src/run/quick-stats.h"

std::shared_ptr<Promise<string>> quickStats(string directory)
{
    auto dir = path->resolve(directory);
    auto trajFiles = findTrajFiles(dir);
    if (trajFiles->get_length() == 0) {
        logger->warn(std::string("No .traj files found in ") + dir + string_empty);
        return std::string("No .traj files found.");
    }
    auto apiCalls = array<double>();
    auto filesByExitStatus = std::make_shared<Map<string, array<string>>>();
    for (auto& filePath : trajFiles)
    {
        try
        {
            auto data = JSON->parse(fs::readFileSync(filePath, std::string("utf-8")));
            if (data["info"]["model_stats"]["api_calls"] != undefined) {
                apiCalls->push(data["info"]["model_stats"]["api_calls"]);
            }
            if (data["info"]["exit_status"]) {
                auto status = data["info"]["exit_status"];
                if (!filesByExitStatus->has(status)) {
                    filesByExitStatus->set(status, array<any>());
                }
                filesByExitStatus->get(status)->push(filePath);
            }
        }
        catch (const any& error)
        {
            logger->error(std::string("Error processing ") + filePath + std::string(": ") + error + string_empty);
        }
    }
    auto sortedStatuses = Array->from(filesByExitStatus->entries())->sort([=](auto a, auto b) mutable
    {
        return std::get<1>(b)->length - std::get<1>(a)->length;
    }
    );
    if (AND((sortedStatuses->get_length() == 0), (apiCalls->get_length() == 0))) {
        logger->warn(std::string("No valid api_calls data found in the .traj files"));
        return std::string("No valid api_calls data found in the .traj files.");
    }
    logger->info(std::string("Exit statuses:"));
    auto& __array1781_1882 = sortedStatuses;
    for (auto __indx1781_1882 = 0_N; __indx1781_1882 < __array1781_1882->get_length(); __indx1781_1882++)
    {
        auto& [status, files] = const_(__array1781_1882)[__indx1781_1882];
        {
            logger->info(string_empty + status + std::string(": ") + files->get_length() + string_empty);
        }
    }
    if (apiCalls->get_length() > 0) {
        auto averageApiCalls = apiCalls->reduce([=](auto a, auto b) mutable
        {
            return a + b;
        }
        , 0) / apiCalls->get_length();
        logger->info(std::string("Avg api calls: ") + averageApiCalls + string_empty);
    }
    auto result = array<string>();
    auto& __array2104_2405 = sortedStatuses;
    for (auto __indx2104_2405 = 0_N; __indx2104_2405 < __array2104_2405->get_length(); __indx2104_2405++)
    {
        auto& [status, files] = const_(__array2104_2405)[__indx2104_2405];
        {
            result->push(std::string("\
## `") + status + std::string("` - ") + files->get_length() + std::string(" trajectories"));
            auto subdirs = std::make_shared<Set>(files->map([=](auto file) mutable
            {
                return path->dirname(file);
            }
            ));
            result->push(Array->from(subdirs)->join(std::string(" ")));
        }
    }
    return result->join(std::string("\
"));
};


array<string> findTrajFiles(string directory)
{
    shared results = array<string>();
    auto walk = [=](auto dir) mutable
    {
        auto files = fs::readdirSync(dir);
        for (auto& file : files)
        {
            auto filePath = path->join(dir, file);
            auto stat = fs::statSync(filePath);
            if (stat->isDirectory()) {
                walk(filePath);
            } else if (file->endsWith(std::string(".traj"))) {
                results->push(filePath);
            }
        }
    };

    walk(directory);
    return results;
};


std::shared_ptr<AgentLogger> logger = getLogger(std::string("quick-stats"), std::string("📊"));

void Main(void)
{
}

MAIN
