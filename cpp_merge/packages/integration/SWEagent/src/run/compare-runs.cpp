#include "compare-runs.h"

std::shared_ptr<Set<string>> getResolved(string filePath)
{
    auto data = JSON->parse(fs::readFileSync(filePath, std:("utf-8")));
    if (in(std:("resolved"), data)) {
        data["resolved_ids"] = data["resolved"];
    }
    return std::make_shared<Set>(OR((data["resolved_ids"]), (array<any>())));
};


std::shared_ptr<Set<string>> getSubmitted(string filePath)
{
    auto data = JSON->parse(fs::readFileSync(filePath, std:("utf-8")));
    return std::make_shared<Set>(OR((data["submitted_ids"]), (array<any>())));
};


void statsSingle(string filePath)
{
    auto evaluatedIds = Array->from(getSubmitted(filePath))->sort();
    auto resolvedIds = Array->from(getResolved(filePath))->sort();
    console->log(std:("Total evaluated: ") + evaluatedIds->get_length() + string_empty);
    console->log(std:("Total resolved: ") + resolvedIds->get_length() + string_empty);
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
    shared header = array<string>{ std:("ID"), paths->map([=](auto _, auto i) mutable
    {
        return String(i);
    }
    ), std:("Success rate") };
    auto table = array<array<any>>();
    auto getEmoji = [=](auto id, auto filePath) mutable
    {
        auto evaluated = OR((evaluatedIds->get(filePath)), (array<any>()));
        auto resolved = OR((resolvedIds->get(filePath)), (array<any>()));
        if (!evaluated->includes(id)) {
            return std:("❓");
        }
        if (resolved->includes(id)) {
            return std:("✅");
        }
        return std:("❌");
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
            row->push((nEvaluated > 0) ? any((nSuccess / nEvaluated)->toFixed(2)) (std:("0.00")));
            table->push(row);
        }
    }
    shared successes = array<any>{ std:("Successes") };
    shared successRates = array<any>{ std:("Success rates") };
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
            successRates->push((nEvaluated > 0) ? any((nSuccess / nEvaluated)->toFixed(2)) (std:("0.00")));
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
    console->log(std:("\
Summary:"));
    auto summaryTable = paths->map([=](auto p, auto i) mutable
    {
        return (object{
            object::pair{std:("#"), i}, 
            object::pair{std:("ID"), path->basename(path->dirname(p))}, 
            object::pair{std:("Successes"), const_(successes)[i + 1]}, 
            object::pair{std:("Success rate"), const_(successRates)[i + 1]}
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
    console->log(std:("Total evaluated: new ") + evaluatedIds->get_length() + std:(", old ") + oldEvaluatedIds->get_length() + string_empty);
    console->log(std:("Total resolved: new ") + resolvedIds->get_length() + std:(", old ") + oldResolvedIds->get_length() + string_empty);
    console->log(std:("-")->repeat(80));
    console->log(std:("Emoji legend:"));
    console->log(std:("❓: Not evaluated in old version, so guessing it's either 😀 or 👾"));
    console->log(std:("😀: Newly resolved in new version"));
    console->log(std:("✅: Resolved in both"));
    console->log(std:("❌: Resolved in old, not in new"));
    console->log(std:("👾: Unresolved in both"));
    console->log(std:("-")->repeat(80));
    for (auto& id : evaluatedIds)
    {
        auto resolvedNow = resolvedIds->includes(id);
        auto resolvedBefore = oldResolvedIds->includes(id);
        auto inOldEvaluated = oldEvaluatedIds->includes(id);
        string emoji;
        if (AND((!inOldEvaluated), (resolvedNow))) {
            emoji = std:("😀❓");
        } else if (AND((!inOldEvaluated), (!resolvedNow))) {
            emoji = std:("👾❓");
        } else if (AND((resolvedNow), (!resolvedBefore))) {
            emoji = std:("😀");
        } else if (AND((resolvedNow), (resolvedBefore))) {
            emoji = std:("✅");
            if (!showSame) {
                continue;
            }
        } else if (AND((!resolvedNow), (resolvedBefore))) {
            emoji = std:("❌");
        } else {
            emoji = std:("👾");
            if (!showSame) {
                continue;
            }
        }
        console->log(string_empty + emoji + std:(" ") + id + string_empty);
    }
};


std::shared_ptr<Promise<void>> compareRuns(array<string> paths, boolean showSame)
{
    auto resultPaths = paths->map([=](auto p) mutable
    {
        if (AND((fs::existsSync(p)), (fs::statSync(p)->isDirectory()))) {
            return path->join(p, std:("results.json"));
        }
        return p;
    }
    );
    for (auto& p : resultPaths)
    {
        if (!fs::existsSync(p)) {
            throw any(std::make_shared<Error>(std:("File not found: ") + p + string_empty));
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


