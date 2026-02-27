#include "status.hpp"

SetStatusAgentHook::SetStatusAgentHook(string id, std::function<void(string, string)> callable) : AbstractAgentHook() {
    this->id = id;
    this->callable = callable;
}

void SetStatusAgentHook::onSetupAttempt()
{
    this->iAttempt++;
    this->iStep = 0;
    this->previousCost += this->cost;
    this->cost = 0;
}

void SetStatusAgentHook::update(string message)
{
    this->callable(this->id, message);
}

void SetStatusAgentHook::onStepStart()
{
    this->iStep++;
    auto attemptStr = (this->iAttempt > 1) ? any(std:("Attempt ") + this->iAttempt + std:(" ")) (string_empty);
    this->update(string_empty + attemptStr + std:("Step ") + this->iStep->toString()->padStart(3) + std:(" ($") + (this->previousCost + this->cost)->toFixed(2) + std:(")"));
}

void SetStatusAgentHook::onStepDone(std::shared_ptr<StepOutput> _step, std::shared_ptr<AgentInfo> info)
{
    if (AND((info->modelStats), (type_of(info->modelStats->instanceCost) == std:("number")))) {
        this->cost = info->modelStats->instanceCost;
    }
}

void SetStatusAgentHook::onToolsInstallationStarted()
{
    this->update(std:("Installing tools"));
}

