#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/src/run/progress.h"

RunBatchProgressManager::RunBatchProgressManager(double _numInstances, string yamlReportPath) {
    this->yamlReportPath = yamlReportPath;
}

double RunBatchProgressManager::get_nCompleted()
{
    auto total = 0;
    auto& __array1348_1454 = this->instancesByExitStatus->values();
    for (auto __indx1348_1454 = 0_N; __indx1348_1454 < __array1348_1454->get_length(); __indx1348_1454++)
    {
        auto& instances = const_(__array1348_1454)[__indx1348_1454];
        {
            total += instances["length"];
        }
    }
    return total;
}

void RunBatchProgressManager::updateExitStatusTable()
{
    auto sorted = Array->from(this->instancesByExitStatus->entries())->sort([=](auto a, auto b) mutable
    {
        return std::get<1>(b)->length - std::get<1>(a)->length;
    }
    );
    console->log(std::string("\
=== Exit Status Summary ==="));
    auto& __array1782_1987 = sorted;
    for (auto __indx1782_1987 = 0_N; __indx1782_1987 < __array1782_1987->get_length(); __indx1782_1987++)
    {
        auto& [status, instances] = const_(__array1782_1987)[__indx1782_1987];
        {
            auto instancesStr = this->shortenStr(instances->reverse()->join(std::string(", ")), 55);
            console->log(string_empty + status + std::string(": ") + instances->get_length() + std::string(" - ") + instancesStr + string_empty);
        }
    }
}

string RunBatchProgressManager::shortenStr(string s, double maxLen, boolean shortenLeft)
{
    if (!shortenLeft) {
        if (s->get_length() > maxLen) {
            return s->substring(0, maxLen - 3) + std::string("...");
        }
    } else {
        if (s->get_length() > maxLen) {
            return std::string("...") + s->substring(s->get_length() - maxLen + 3);
        }
    }
    return s->padEnd(maxLen);
}

void RunBatchProgressManager::updateInstanceStatus(string instanceId, string message)
{
    console->log(std::string("[") + instanceId + std::string("] ") + message + string_empty);
}

void RunBatchProgressManager::onInstanceStart(string instanceId)
{
    this->spinnerTasks->set(instanceId, object{
        object::pair{std::string("status"), std::string("Task initialized")}, 
        object::pair{std::string("startTime"), Date->now()}
    });
}

void RunBatchProgressManager::onInstanceEnd(string instanceId, any exitStatus)
{
    if (!this->instancesByExitStatus->has(exitStatus)) {
        this->instancesByExitStatus->set(exitStatus, array<any>());
    }
    this->instancesByExitStatus->get(exitStatus)->push(instanceId);
    this->spinnerTasks->delete(instanceId);
    this->updateExitStatusTable();
    if (this->yamlReportPath) {
        this->saveOverviewDataYaml(this->yamlReportPath);
    }
}

void RunBatchProgressManager::onUncaughtException(string instanceId, std::shared_ptr<Error> exception)
{
    this->onInstanceEnd(instanceId, std::string("Uncaught ") + exception->constructor->name + string_empty);
}

void RunBatchProgressManager::printReport()
{
    console->log(std::string("\
=== Final Report ==="));
    auto& __array3391_3616 = this->instancesByExitStatus->entries();
    for (auto __indx3391_3616 = 0_N; __indx3391_3616 < __array3391_3616->get_length(); __indx3391_3616++)
    {
        auto& [status, instances] = const_(__array3391_3616)[__indx3391_3616];
        {
            console->log(string_empty + status + std::string(": ") + instances["length"] + string_empty);
            for (auto& instance : instances)
            {
                console->log(std::string("  ") + instance + string_empty);
            }
        }
    }
}

Record<string, any> RunBatchProgressManager::getOverviewData()
{
    auto instancesByStatus = object{};
    for (auto& [status, instances] : this->instancesByExitStatus->entries())
    {
        instancesByStatus[OR((status), (std::string("unknown")))] = instances;
    }
    return object{
        object::pair{std::string("instances_by_exit_status"), instancesByStatus}, 
        object::pair{std::string("total_cost"), 0}
    };
}

void RunBatchProgressManager::saveOverviewDataYaml(string filePath)
{
    auto data = this->getOverviewData();
    fs::writeFileSync(filePath, yaml->dump(data, object{
        object::pair{std::string("indent"), 4}
    }));
}

