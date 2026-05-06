#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_PACKAGES_API_CLIENT_SRC_SERVICES_SESSIONS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_PACKAGES_API_CLIENT_SRC_SERVICES_SESSIONS_H
#include "core.hpp"
#include <string>
#include "../lib/base-client.h"
#include "../types/sessions.h"

class SessionMessageQueryParams;
class SessionsService;

class SessionMessageQueryParams : public object, public std::enable_shared_from_this<SessionMessageQueryParams> {
public:
    using std::enable_shared_from_this<SessionMessageQueryParams>::shared_from_this;
    string limit;

    string before;

    string after;
};

template <typename P0>
any toTimestampString(P0 value, string paramName);

any validateRequiredParam(any value, string paramName);

class SessionsService : public BaseApiClient, public std::enable_shared_from_this<SessionsService> {
public:
    using std::enable_shared_from_this<SessionsService>::shared_from_this;
    virtual std::shared_ptr<Promise<std::shared_ptr<SessionsHealthResponse>>> checkHealth();
    virtual std::shared_ptr<Promise<std::shared_ptr<CreateSessionResponse>>> createSession(std::shared_ptr<CreateSessionParams> params);
    virtual std::shared_ptr<Promise<std::shared_ptr<SessionInfoResponse>>> getSession(string sessionId);
    virtual std::shared_ptr<Promise<std::shared_ptr<MessageResponse>>> sendMessage(string sessionId, std::shared_ptr<SendMessageParams> params);
    virtual std::shared_ptr<Promise<std::shared_ptr<GetMessagesResponse>>> getMessages(string sessionId, std::shared_ptr<GetMessagesParams> params = std::nullopt);
    virtual std::shared_ptr<Promise<object>> deleteSession(string sessionId);
    virtual std::shared_ptr<Promise<std::shared_ptr<ListSessionsResponse>>> listSessions();
    SessionsService(std::shared_ptr<ApiClientConfig> config);
};

template <typename P0>
any toTimestampString(P0 value, string paramName)
{
    if (!value) return std::nullopt;
    double timestamp;
    if (is<Date>(value)) {
        timestamp = value->getTime();
    } else if (type_of(value) == std::string("string")) {
        auto date = std::make_shared<Date>(value);
        timestamp = date->getTime();
        if (isNaN(timestamp)) {
            console->warn(std::string("Invalid date string for ") + paramName + std::string(": ") + value + string_empty);
            return std::nullopt;
        }
    } else if (type_of(value) == std::string("number")) {
        timestamp = value;
    } else {
        console->warn(std::string("Invalid type for ") + paramName + std::string(": ") + type_of(value) + string_empty);
        return std::nullopt;
    }
    return timestamp->toString();
};


#endif
