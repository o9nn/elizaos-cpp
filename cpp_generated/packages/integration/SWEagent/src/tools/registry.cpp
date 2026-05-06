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
        catch (const any& error)
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
        catch (const any& error)
        {
        }
    }
}

any EnvRegistry::get(string key)
{
    this->load();
    return const_(this->data)[key];
}

void EnvRegistry::set(string key, any value)
{
    this->data[key] = value;
    this->save();
}

boolean EnvRegistry::has(string key)
{
    this->load();
    return in(key, this->data);
}

void EnvRegistry::delete(string key)
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
