#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/tools/src/registry/index.h"

EnvRegistry::EnvRegistry(string envFile) {
    this->envFile = OR((OR((envFile), (process->env->SWE_AGENT_ENV_FILE))), (path->join(os::homedir(), std:(".swe-agent-env"))));
    this->loadData();
}

void EnvRegistry::loadData()
{
    if (fs::existsSync(this->envFile)) {
        try
        {
            auto content = fs::readFileSync(this->envFile, std:("utf-8"));
            this->data = JSON->parse(content);
        }
        catch (const any& error)
        {
            console->error(std:("Error reading registry file: ") + error + string_empty);
            this->data = object{};
        }
    } else {
        this->data = object{};
    }
}

void EnvRegistry::saveData()
{
    try
    {
        auto dir = path->dirname(this->envFile);
        if (!fs::existsSync(dir)) {
            fs::mkdirSync(dir, object{
                object::pair{std:("recursive"), true}
            });
        }
        fs::writeFileSync(this->envFile, JSON->stringify(this->data, nullptr, 2));
    }
    catch (const any& error)
    {
        console->error(std:("Error writing registry file: ") + error + string_empty);
    }
}

any EnvRegistry::get(string key, any defaultValue, boolean fallbackToEnv)
{
    if (this->data->hasOwnProperty(key)) {
        return const_(this->data)[key];
    }
    if (AND((fallbackToEnv), (const_(process->env)[key]))) {
        return const_(process->env)[key];
    }
    return defaultValue;
}

void EnvRegistry::set(string key, any value)
{
    this->data[key] = value;
    this->saveData();
}

void EnvRegistry::delete(string key)
{
    this->data.Delete(key);
    this->saveData();
}

Record<string, any> EnvRegistry::getAll()
{
    return utils::assign(object{
    }, this->data);
}

void setupCLI()
{
    program->name(std:("registry"))->description(std:("Environment registry management"))->version(std:("1.0.0"));
    program->command(std:("get <key>"))->description(std:("Get a value from the registry"))->option(std:("-d, --default <value>"), std:("Default value if key not found"))->action([=](auto key, auto options) mutable
    {
        auto value = registry->get(key, OR((options["default"]), (string_empty)));
        console->log(value);
    }
    );
    program->command(std:("set <key> <value>"))->description(std:("Set a value in the registry"))->action([=](auto key, auto value) mutable
    {
        registry->set(key, value);
        console->log(std:("Set ") + key + std:(" = ") + value + string_empty);
    }
    );
    program->command(std:("delete <key>"))->description(std:("Delete a key from the registry"))->action([=](auto key) mutable
    {
        registry->delete(key);
        console->log(std:("Deleted ") + key + string_empty);
    }
    );
    program->command(std:("list"))->description(std:("List all registry entries"))->action([=]() mutable
    {
        auto all = registry->getAll();
        console->log(JSON->stringify(all, nullptr, 2));
    }
    );
    program->parse(process->argv);
};


std::shared_ptr<EnvRegistry> registry = std::make_shared<EnvRegistry>();

void Main(void)
{
    if (OR((require->main == module), (require->main->filename->endsWith(std:("/bin/registry"))))) {
        setupCLI();
    }
}

MAIN
