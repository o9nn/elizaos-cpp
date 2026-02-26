#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PACKAGES_API-CLIENT_SRC_TYPES_AUTH_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PACKAGES_API-CLIENT_SRC_TYPES_AUTH_H
#include "core.h"

class LoginRequest;
class LoginResponse;
class RefreshTokenResponse;
class CurrentUserResponse;

class LoginRequest : public object, public std::enable_shared_from_this<LoginRequest> {
public:
    using std::enable_shared_from_this<LoginRequest>::shared_from_this;
    string email;

    string username;

    string cdpUserId;
};

class LoginResponse : public object, public std::enable_shared_from_this<LoginResponse> {
public:
    using std::enable_shared_from_this<LoginResponse>::shared_from_this;
    string token;

    string userId;

    string username;

    string expiresIn;
};

class RefreshTokenResponse : public object, public std::enable_shared_from_this<RefreshTokenResponse> {
public:
    using std::enable_shared_from_this<RefreshTokenResponse>::shared_from_this;
    string token;

    string userId;

    string username;

    string expiresIn;
};

class CurrentUserResponse : public object, public std::enable_shared_from_this<CurrentUserResponse> {
public:
    using std::enable_shared_from_this<CurrentUserResponse>::shared_from_this;
    string userId;

    string email;

    string username;
};

#endif
