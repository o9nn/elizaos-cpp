#include "client.hpp"

ElizaClient::ElizaClient(std::shared_ptr<ApiClientConfig> config) {
    this->agents = std::make_shared<AgentsService>(config);
    this->messaging = std::make_shared<MessagingService>(config);
    this->memory = std::make_shared<MemoryService>(config);
    this->audio = std::make_shared<AudioService>(config);
    this->media = std::make_shared<MediaService>(config);
    this->server = std::make_shared<ServerService>(config);
    this->system = std::make_shared<SystemService>(config);
    this->sessions = std::make_shared<SessionsService>(config);
    this->runs = std::make_shared<RunsService>(config);
    this->entities = std::make_shared<EntitiesService>(config);
    this->cdp = std::make_shared<CdpService>(config);
    this->auth = std::make_shared<AuthService>(config);
    this->jobs = std::make_shared<JobsService>(config);
    this->gamification = std::make_shared<GamificationService>(config);
    this->services = array<std::shared_ptr<AgentsService>>{ this->agents, this->messaging, this->memory, this->audio, this->media, this->server, this->system, this->sessions, this->runs, this->entities, this->cdp, this->auth, this->jobs, this->gamification };
}

void ElizaClient::setAuthToken(string token)
{
    for (auto& service : this->services)
    {
        if (AND((service), (type_of(service["setAuthToken"]) == std:("function")))) {
            service["setAuthToken"](token);
        }
    }
}

void ElizaClient::clearAuthToken()
{
    for (auto& service : this->services)
    {
        if (AND((service), (type_of(service["clearAuthToken"]) == std:("function")))) {
            service["clearAuthToken"]();
        }
    }
}

std::shared_ptr<ElizaClient> ElizaClient::create(std::shared_ptr<ApiClientConfig> config)
{
    return std::make_shared<ElizaClient>(config);
}

