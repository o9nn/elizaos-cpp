#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_SERVER_SRC_AUTH_SESSION_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_SERVER_SRC_AUTH_SESSION_H
#include "core.h"
#include "uuid.h"
using uuid = v4;
#include "../redis.h"

class SessionData;

extern double SESSION_TTL;
class SessionData : public object, public std::enable_shared_from_this<SessionData> {
public:
    using std::enable_shared_from_this<SessionData>::shared_from_this;
    string publicKey;

    array<string> privileges;

    double createdAt;
};

std::shared_ptr<Promise<string>> createSession(std::shared_ptr<SessionData> data);

std::shared_ptr<Promise<any>> getSession(string sid);

void destroySession(string sid);

#endif
