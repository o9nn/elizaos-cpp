#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_PACKAGES_API_CLIENT_SRC_SERVICES_AUTH_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_PACKAGES_API_CLIENT_SRC_SERVICES_AUTH_H
#include "core.hpp"
#include "../lib/base-client.h"
#include "../types/auth.h"

class AuthService;

class AuthService : public BaseApiClient, public std::enable_shared_from_this<AuthService> {
public:
    using std::enable_shared_from_this<AuthService>::shared_from_this;
    virtual std::shared_ptr<Promise<std::shared_ptr<LoginResponse>>> login(std::shared_ptr<LoginRequest> request);
    virtual std::shared_ptr<Promise<std::shared_ptr<RefreshTokenResponse>>> refreshToken();
    virtual std::shared_ptr<Promise<std::shared_ptr<CurrentUserResponse>>> getCurrentUser();
    AuthService(std::shared_ptr<ApiClientConfig> config);
};

#endif
