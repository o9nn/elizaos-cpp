#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/src/tools/registry.h"

EnvRegistry::EnvRegistry() {
    this->data = object{};
    this->envFile = process->env->SWE_AGENT_ENV_FILE;
    this->load();
}

void EnvRegistry::load()
{
    if (AND((this->envFile), (fs::existsSync(this->envFile)))) {
        try
        {
            auto content = fs::readFileSync(this->envFile, std::string("utf-8"));
            this->data = JSON->parse(content);
        }
        catch (const std::any& error)
        {
            this->data = object{};
        }
    }
}

void EnvRegistry::save()
{
    if (this->envFile) {
        try
        {
            fs::writeFileSync(this->envFile, JSON->stringify(this->data, nullptr, 2));
        }
        catch (const std::any& error)
        {
        }
    }
}

std::any EnvRegistry::get(std::string key)
{
    this->load();
    return const_(this->data)[key];
}

void EnvRegistry::std::set(std::string key, std::any value)
{
    this->data[key] = value;
    this->save();
}

boolean EnvRegistry::has(std::string key)
{
    this->load();
    return in(key, this->data);
}

void EnvRegistry::delete(std::string key)
{
    this->data.Delete(key);
    this->save();
}

void EnvRegistry::clear()
{
    this->data = object{};
    this->save();
}

std::shared_ptr<EnvRegistry> registry = std::make_shared<EnvRegistry>();

void Main(void)
{
}

MAIN
