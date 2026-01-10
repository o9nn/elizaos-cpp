#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/src/commands/dev/utils/file-watcher.h"

array<string> findTsFiles(string dir, string watchDir)
{
    auto results = array<string>();
    try
    {
        auto entries = readdirSync(dir, object{
            object::pair{std::string("withFileTypes"), true}
        });
        for (auto& entry : entries)
        {
            auto fullPath = path->join(dir, entry->name);
            if (AND((AND((AND((entry->isDirectory()), (!entry->name->startsWith(std::string("."))))), (entry->name != std::string("node_modules")))), (entry->name != std::string("dist")))) {
                results = results->concat(findTsFiles(fullPath, watchDir));
            } else if (AND((entry->isFile()), ((OR((OR((OR((entry->name->endsWith(std::string(".ts"))), (entry->name->endsWith(std::string(".js"))))), (entry->name->endsWith(std::string(".tsx"))))), (entry->name->endsWith(std::string(".jsx")))))))) {
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
        auto srcDir = path->join(absoluteDir, std::string("src"));
        shared dirToWatch = (existsSync(srcDir)) ? srcDir : absoluteDir;
        auto watchOptions = utils::assign(object{
        }, DEFAULT_WATCHER_CONFIG, config);
        shared watcher = chokidar->watch(dirToWatch, watchOptions);
        auto tsFiles = findTsFiles(dirToWatch, dirToWatch);
        console->info(std::string("Found ") + tsFiles->get_length() + std::string(" TypeScript/JavaScript files in the watched directory"));
        if (tsFiles->get_length() > 0) {
            console->info(std::string("Sample files: ") + tsFiles->slice(0, 3)->join(std::string(", ")) + string_empty + (tsFiles->get_length() > 3) ? std::string("...") : string_empty + string_empty);
        }
        shared debounceTimer = nullptr;
        watcher->on(std::string("ready"), [=]() mutable
        {
            auto watchedPaths = watcher->getWatched();
            auto pathsCount = Object->keys(watchedPaths)->get_length();
            if (pathsCount == 0) {
                console->warn(std::string("No directories are being watched! File watching may not be working."));
                watcher->add(string_empty + dirToWatch + std::string("/**/*.{ts,js,tsx,jsx}"));
            }
            console->log(std::string("✓ Watching for file changes in ") + path->relative(process->cwd(), dirToWatch) + string_empty);
        }
        );
        watcher->on(std::string("all"), [=](auto _event, auto filePath) mutable
        {
            if (!(new RegExp(std::string("\.(ts|js|tsx|jsx)")))->test(filePath)) {
                return;
            }
            console->info(std::string("File changed: ") + path->relative(dirToWatch, filePath) + string_empty);
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
        watcher->on(std::string("error"), [=](auto error) mutable
        {
            console->error(std::string("Chokidar watcher error: ") + error + string_empty);
        }
        );
        process->on(std::string("SIGINT"), [=]() mutable
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
        console->error(std::string("Error setting up file watcher: ") + error["message"] + string_empty);
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
    object::pair{std::string("ignored"), array<string>{ std::string("**/node_modules/**"), std::string("**/dist/**"), std::string("**/.git/**") }}, 
    object::pair{std::string("ignoreInitial"), true}, 
    object::pair{std::string("persistent"), true}, 
    object::pair{std::string("followSymlinks"), false}, 
    object::pair{std::string("depth"), 99}, 
    object::pair{std::string("usePolling"), false}, 
    object::pair{std::string("interval"), 1000}
};

void Main(void)
{
}

MAIN
