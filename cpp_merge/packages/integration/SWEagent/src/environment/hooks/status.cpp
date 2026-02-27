#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/src/environment/hooks/status.h"

SetStatusEnvironmentHook::SetStatusEnvironmentHook(string id, StatusCallback callable) : EnvHook() {
    this->id = id;
    this->callable = callable;
}

void SetStatusEnvironmentHook::update(string message)
{
    this->callable(this->id, message);
}

void SetStatusEnvironmentHook::onStartDeployment()
{
    this->update(std:("Starting deployment"));
}

void SetStatusEnvironmentHook::onInstallEnvStarted()
{
    this->update(std:("Installing environment"));
}

void SetStatusEnvironmentHook::onEnvironmentStartup()
{
    this->update(std:("Starting environment"));
}

void SetStatusEnvironmentHook::onClose()
{
    this->update(std:("Closing environment"));
}

