#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-STAGEHAND_STAGEHAND-SERVER_SRC_SESSION-MANAGER_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-STAGEHAND_STAGEHAND-SERVER_SRC_SESSION-MANAGER_H
#include "core.h"
#include "@browserbasehq/stagehand.h"
#include "./logger.js.h"
#include "./playwright-installer.js.h"

class BrowserSession;
class SessionManager;

class BrowserSession : public object, public std::enable_shared_from_this<BrowserSession> {
public:
    using std::enable_shared_from_this<BrowserSession>::shared_from_this;
    string id;

    string clientId;

    std::shared_ptr<Stagehand> stagehand;

    std::shared_ptr<Date> createdAt;
};

class SessionManager : public object, public std::enable_shared_from_this<SessionManager> {
public:
    using std::enable_shared_from_this<SessionManager>::shared_from_this;
    std::shared_ptr<Logger> logger;

    std::shared_ptr<PlaywrightInstaller> playwrightInstaller;

    std::shared_ptr<Map<string, std::shared_ptr<BrowserSession>>> sessions = std::make_shared<Map>();

    double maxSessionsPerClient = 3;

    SessionManager(std::shared_ptr<Logger> logger_, std::shared_ptr<PlaywrightInstaller> playwrightInstaller_);
    virtual std::shared_ptr<Promise<std::shared_ptr<BrowserSession>>> createSession(string sessionId, string clientId);
    virtual any getSession(string sessionId);
    virtual std::shared_ptr<Promise<void>> destroySession(string sessionId);
    virtual array<std::shared_ptr<BrowserSession>> getClientSessions(string clientId);
    virtual std::shared_ptr<Promise<void>> cleanupClientSessions(string clientId);
    virtual std::shared_ptr<Promise<void>> cleanup();
};

#endif
