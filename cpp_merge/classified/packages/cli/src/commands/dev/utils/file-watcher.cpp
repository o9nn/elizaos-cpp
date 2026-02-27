#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/cli/src/commands/dev/utils/file-watcher.h"

array<string> findTsFiles(string dir, string watchDir)
{
    auto results = array<string>();
    try
    {
        auto entries = readdirSync(dir, object{
            object::pair{std:("withFileTypes"), true}
        });
        for (auto& entry : entries)
        {
            auto fullPath = path->join(dir, entry->name);
            if (AND((AND((AND((entry->isDirectory()), (!entry->name->startsWith(std:("."))))), (entry->name != std:("node_modules")))), (entry->name != std:("dist")))) {
                results = results->concat(findTsFiles(fullPath, watchDir));
            } else if (AND((entry->isFile()), ((OR((OR((OR((entry->name->endsWith(std:(".ts"))), (entry->name->endsWith(std:(".js"))))), (entry->name->endsWith(std:(".tsx"))))), (entry->name->endsWith(std:(".jsx")))))))) {
                results->push(path->relative(watchDir, fullPath));
            }
        }
    }
    catch (const any& error)
    {
    }
    return results;
};


std::shared_ptr<Promise<void>> watchDirectory(string dir, std::function<void()> onChange, Partial<std::shared_ptr<WatcherConfig>> config)
{
    try
    {
        auto absoluteDir = path->resolve(dir);
        auto srcDir = path->join(absoluteDir, std:("src"));
        shared dirToWatch = (existsSync(srcDir)) ? srcDir : absoluteDir;
        auto watchOptions = utils::assign(object{
        }, DEFAULT_WATCHER_CONFIG, config);
        shared watcher = chokidar->watch(dirToWatch, watchOptions);
        auto tsFiles = findTsFiles(dirToWatch, dirToWatch);
        console->info(std:("Found ") + tsFiles->get_length() + std:(" TypeScript/JavaScript files in the watched directory"));
        if (tsFiles->get_length() > 0) {
            console->info(std:("Sample files: ") + tsFiles->slice(0, 3)->join(std:(", ")) + string_empty + (tsFiles->get_length() > 3) ? std:("...") : string_empty + string_empty);
        }
        shared debounceTimer = nullptr;
        watcher->on(std:("ready"), [=]() mutable
        {
            auto watchedPaths = watcher->getWatched();
            auto pathsCount = Object->keys(watchedPaths)->get_length();
            if (pathsCount == 0) {
                console->warn(std:("No directories are being watched! File watching may not be working."));
                watcher->add(string_empty + dirToWatch + std:("/**/*.{ts,js,tsx,jsx}"));
            }
            console->log(std:("✓ Watching for file changes in ") + path->relative(process->cwd(), dirToWatch) + string_empty);
        }
        );
        watcher->on(std:("all"), [=](auto _event, auto filePath) mutable
        {
            if (!(new RegExp(std:("\.(ts|js|tsx|jsx)")))->test(filePath)) {
                return;
            }
            console->info(std:("File changed: ") + path->relative(dirToWatch, filePath) + string_empty);
            if (debounceTimer) {
                clearTimeout(debounceTimer);
            }
            debounceTimer = setTimeout([=]() mutable
            {
                onChange();
                debounceTimer = nullptr;
            }
            , 300);
        }
        );
        watcher->on(std:("error"), [=](auto error) mutable
        {
            console->error(std:("Chokidar watcher error: ") + error + string_empty);
        }
        );
        process->on(std:("SIGINT"), [=]() mutable
        {
            watcher->close()->then([=]() mutable
            {
                return process->exit(0);
            }
            );
        }
        );
    }
    catch (const any& error)
    {
        console->error(std:("Error setting up file watcher: ") + error["message"] + string_empty);
    }
    return std::shared_ptr<Promise<void>>();
};


std::function<void()> createDebouncedHandler(std::function<void()> handler, double delay)
{
    shared timer = nullptr;
    return [=]() mutable
    {
        if (timer) {
            clearTimeout(timer);
        }
        timer = setTimeout([=]() mutable
        {
            handler();
            timer = nullptr;
        }
        , delay);
    };
};


std::shared_ptr<WatcherConfig> DEFAULT_WATCHER_CONFIG = object{
    object::pair{std:("ignored"), array<string>{ std:("**/node_modules/**"), std:("**/dist/**"), std:("**/.git/**") }}, 
    object::pair{std:("ignoreInitial"), true}, 
    object::pair{std:("persistent"), true}, 
    object::pair{std:("followSymlinks"), false}, 
    object::pair{std:("depth"), 99}, 
    object::pair{std:("usePolling"), false}, 
    object::pair{std:("interval"), 1000}
};

void Main(void)
{
}

MAIN
