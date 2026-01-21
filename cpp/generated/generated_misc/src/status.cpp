#include "SWEagent/src/environment/hooks/status.h"

SetStatusEnvironmentHook::SetStatusEnvironmentHook(std::string id, StatusCallback callable) : EnvHook() {
    this->id = id;
    this->callable = callable;
}

void SetStatusEnvironmentHook::update(std::string message)
{
    this->callable(this->id, message);
}

void SetStatusEnvironmentHook::onStartDeployment()
{
    this->update(std::string("Starting deployment"));
}

void SetStatusEnvironmentHook::onInstallEnvStarted()
{
    this->update(std::string("Installing environment"));
}

void SetStatusEnvironmentHook::onEnvironmentStartup()
{
    this->update(std::string("Starting environment"));
}

void SetStatusEnvironmentHook::onClose()
{
    this->update(std::string("Closing environment"));
}

