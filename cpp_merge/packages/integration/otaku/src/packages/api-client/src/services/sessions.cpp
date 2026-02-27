#include "sessions.hpp"

any validateRequiredParam(any value, string paramName)
{
    if (OR((!value), (value->trim() == string_empty))) {
        throw any(std::make_shared<Error>(string_empty + paramName + std:(" is required and cannot be empty")));
    }
    return any();
};


std::shared_ptr<Promise<std::shared_ptr<SessionsHealthResponse>>> SessionsService::checkHealth()
{
    return this->get<std::shared_ptr<SessionsHealthResponse>>(std:("/api/messaging/sessions/health"));
}

std::shared_ptr<Promise<std::shared_ptr<CreateSessionResponse>>> SessionsService::createSession(std::shared_ptr<CreateSessionParams> params)
{
    return this->post<std::shared_ptr<CreateSessionResponse>>(std:("/api/messaging/sessions"), params);
}

std::shared_ptr<Promise<std::shared_ptr<SessionInfoResponse>>> SessionsService::getSession(string sessionId)
{
    validateRequiredParam(sessionId, std:("sessionId"));
    return this->get<std::shared_ptr<SessionInfoResponse>>(std:("/api/messaging/sessions/") + sessionId + string_empty);
}

std::shared_ptr<Promise<std::shared_ptr<MessageResponse>>> SessionsService::sendMessage(string sessionId, std::shared_ptr<SendMessageParams> params)
{
    validateRequiredParam(sessionId, std:("sessionId"));
    validateRequiredParam(params->content, std:("content"));
    return this->post<std::shared_ptr<MessageResponse>>(std:("/api/messaging/sessions/") + sessionId + std:("/messages"), params);
}

std::shared_ptr<Promise<std::shared_ptr<GetMessagesResponse>>> SessionsService::getMessages(string sessionId, std::shared_ptr<GetMessagesParams> params)
{
    validateRequiredParam(sessionId, std:("sessionId"));
    auto queryParams = object{};
    if (params->limit) {
        queryParams->limit = params->limit->toString();
    }
    auto beforeTimestamp = toTimestampString(params->before, std:("before"));
    if (beforeTimestamp) {
        queryParams->before = beforeTimestamp;
    }
    auto afterTimestamp = toTimestampString(params->after, std:("after"));
    if (afterTimestamp) {
        queryParams->after = afterTimestamp;
    }
    return this->get<std::shared_ptr<GetMessagesResponse>>(std:("/api/messaging/sessions/") + sessionId + std:("/messages"), object{
        object::pair{std:("params"), queryParams}
    });
}

std::shared_ptr<Promise<object>> SessionsService::deleteSession(string sessionId)
{
    validateRequiredParam(sessionId, std:("sessionId"));
    return this->delete<object>(std:("/api/messaging/sessions/") + sessionId + string_empty);
}

std::shared_ptr<Promise<std::shared_ptr<ListSessionsResponse>>> SessionsService::listSessions()
{
    return this->get<std::shared_ptr<ListSessionsResponse>>(std:("/api/messaging/sessions"));
}

SessionsService::SessionsService(std::shared_ptr<ApiClientConfig> config) : BaseApiClient(config) {
}

