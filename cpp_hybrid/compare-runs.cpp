#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/src/run/compare-runs.h"

std::shared_ptr<Set<string>> getResolved(string filePath)
{
    auto data = JSON->parse(fs::readFileSync(filePath, std::string("utf-8")));
    if (in(std::string("resolved"), data)) {
        data["resolved_ids"] = data["resolved"];
    }
    return std::make_shared<Set>(OR((data["resolved_ids"]), (array<any>())));
};


std::shared_ptr<Set<string>> getSubmitted(string filePath)
{
    auto data = JSON->parse(fs::readFileSync(filePath, std::string("utf-8")));
    return std::make_shared<Set>(OR((data["submitted_ids"]), (array<any>())));
};


void statsSingle(string filePath)
{
    auto evaluatedIds = Array->from(getSubmitted(filePath))->sort();
    auto resolvedIds = Array->from(getResolved(filePath))->sort();
    console->log(std::string("Total evaluated: ") + evaluatedIds->get_length() + string_empty);
    console->log(std::string("Total resolved: ") + resolvedIds->get_length() + string_empty);
};


void compareMany(array<string> paths)
{
    shared evaluatedIds = std::make_shared<Map>();
    shared resolvedIds = std::make_shared<Map>();
    for (auto& filePath : paths)
    {
        evaluatedIds->set(filePath, Array->from(getSubmitted(filePath))->sort());
        resolvedIds->set(filePath, Array->from(getResolved(filePath))->sort());
    }
    shared header = array<string>{ std::string("ID"), paths->map([=](auto _, auto i) mutable
    {
        return String(i);
    }
    ), std::string("Success rate") };
    auto table = array<array<any>>();
    auto getEmoji = [=](auto id, auto filePath) mutable
    {
        auto evaluated = OR((evaluatedIds->get(filePath)), (array<any>()));
        auto resolved = OR((resolvedIds->get(filePath)), (array<any>()));
        if (!evaluated->includes(id)) {
            return std::string("❓");
        }
        if (resolved->includes(id)) {
            return std::string("✅");
        }
        return std::string("❌");
    };

    auto idsToCompare = std::make_shared<Set>(OR((evaluatedIds->get(const_(paths)[0])), (array<any>())));
    auto& __array1884_2354 = Array->from(idsToCompare)->sort();
    for (auto __indx1884_2354 = 0_N; __indx1884_2354 < __array1884_2354->get_length(); __indx1884_2354++)
    {
        auto& id = const_(__array1884_2354)[__indx1884_2354];
        {
            auto row = array<any>{ id };
            for (auto& filePath : paths)
            {
                row->push(getEmoji(id, filePath));
            }
            auto nSuccess = paths->filter([=](auto p) mutable
            {
                return (OR((resolvedIds->get(p)), (array<any>())))->includes(id);
            }
            )->get_length();
            auto nEvaluated = paths->filter([=](auto p) mutable
            {
                return (OR((evaluatedIds->get(p)), (array<any>())))->includes(id);
            }
            )->get_length();
            row->push((nEvaluated > 0) ? any((nSuccess / nEvaluated)->toFixed(2)) : any(std::string("0.00")));
            table->push(row);
        }
    }
    shared successes = array<any>{ std::string("Successes") };
    shared successRates = array<any>{ std::string("Success rates") };
    auto& __array2502_2894 = paths;
    for (auto __indx2502_2894 = 0_N; __indx2502_2894 < __array2502_2894->get_length(); __indx2502_2894++)
    {
        auto& filePath = const_(__array2502_2894)[__indx2502_2894];
        {
            auto nSuccess = Array->from(idsToCompare)->filter([=](auto id) mutable
            {
                return (OR((resolvedIds->get(filePath)), (array<any>())))->includes(id);
            }
            )->get_length();
            auto nEvaluated = Array->from(idsToCompare)->filter([=](auto id) mutable
            {
                return (OR((evaluatedIds->get(filePath)), (array<any>())))->includes(id);
            }
            )->get_length();
            successes->push(nSuccess);
            successRates->push((nEvaluated > 0) ? any((nSuccess / nEvaluated)->toFixed(2)) : any(std::string("0.00")));
        }
    }
    table->push(successes);
    table->push(successRates);
    console->table(table->map([=](auto row) mutable
    {
        shared obj = object{};
        header->forEach([=](auto h, auto i) mutable
        {
            obj[h] = const_(row)[i];
        }
        );
        return obj;
    }
    ));
    console->log(std::string("\
Summary:"));
    auto summaryTable = paths->map([=](auto p, auto i) mutable
    {
        return (object{
            object::pair{std::string("#"), i}, 
            object::pair{std::string("ID"), path->basename(path->dirname(p))}, 
            object::pair{std::string("Successes"), const_(successes)[i + 1]}, 
            object::pair{std::string("Success rate"), const_(successRates)[i + 1]}
        });
    }
    );
    console->table(summaryTable);
};


void comparePair(string newPath, string oldPath, boolean showSame)
{
    auto evaluatedIds = Array->from(getSubmitted(newPath))->sort();
    auto resolvedIds = Array->from(getResolved(newPath))->sort();
    auto oldEvaluatedIds = Array->from(getSubmitted(oldPath))->sort();
    auto oldResolvedIds = Array->from(getResolved(oldPath))->sort();
    console->log(std::string("Total evaluated: new ") + evaluatedIds->get_length() + std::string(", old ") + oldEvaluatedIds->get_length() + string_empty);
    console->log(std::string("Total resolved: new ") + resolvedIds->get_length() + std::string(", old ") + oldResolvedIds->get_length() + string_empty);
    console->log(std::string("-")->repeat(80));
    console->log(std::string("Emoji legend:"));
    console->log(std::string("❓: Not evaluated in old version, so guessing it's either 😀 or 👾"));
    console->log(std::string("😀: Newly resolved in new version"));
    console->log(std::string("✅: Resolved in both"));
    console->log(std::string("❌: Resolved in old, not in new"));
    console->log(std::string("👾: Unresolved in both"));
    console->log(std::string("-")->repeat(80));
    for (auto& id : evaluatedIds)
    {
        auto resolvedNow = resolvedIds->includes(id);
        auto resolvedBefore = oldResolvedIds->includes(id);
        auto inOldEvaluated = oldEvaluatedIds->includes(id);
        string emoji;
        if (AND((!inOldEvaluated), (resolvedNow))) {
            emoji = std::string("😀❓");
        } else if (AND((!inOldEvaluated), (!resolvedNow))) {
            emoji = std::string("👾❓");
        } else if (AND((resolvedNow), (!resolvedBefore))) {
            emoji = std::string("😀");
        } else if (AND((resolvedNow), (resolvedBefore))) {
            emoji = std::string("✅");
            if (!showSame) {
                continue;
            }
        } else if (AND((!resolvedNow), (resolvedBefore))) {
            emoji = std::string("❌");
        } else {
            emoji = std::string("👾");
            if (!showSame) {
                continue;
            }
        }
        console->log(string_empty + emoji + std::string(" ") + id + string_empty);
    }
};


std::shared_ptr<Promise<void>> compareRuns(array<string> paths, boolean showSame)
{
    auto resultPaths = paths->map([=](auto p) mutable
    {
        if (AND((fs::existsSync(p)), (fs::statSync(p)->isDirectory()))) {
            return path->join(p, std::string("results.json"));
        }
        return p;
    }
    );
    for (auto& p : resultPaths)
    {
        if (!fs::existsSync(p)) {
            throw any(std::make_shared<Error>(std::string("File not found: ") + p + string_empty));
        }
    }
    if (resultPaths->get_length() == 1) {
        statsSingle(const_(resultPaths)[0]);
    } else if (resultPaths->get_length() == 2) {
        comparePair(const_(resultPaths)[0], const_(resultPaths)[1], showSame);
    } else {
        compareMany(resultPaths);
    }
    return std::shared_ptr<Promise<void>>();
};


