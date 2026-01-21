#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_FRONTEND_LIB_CDPUSER_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_FRONTEND_LIB_CDPUSER_H
#include "core.h"

class CdpAuthMethod;
class CdpSmsMethod;
class CdpAuthenticationMethods;
class CdpUser;
class CdpUserInfoOptions;
class CdpUserInfo;

class CdpAuthMethod : public object, public std::enable_shared_from_this<CdpAuthMethod> {
public:
    using std::enable_shared_from_this<CdpAuthMethod>::shared_from_this;
    string email;

    string name;
};

class CdpSmsMethod : public object, public std::enable_shared_from_this<CdpSmsMethod> {
public:
    using std::enable_shared_from_this<CdpSmsMethod>::shared_from_this;
    string phoneNumber;

    string countryCode;
};

class CdpAuthenticationMethods : public object, public std::enable_shared_from_this<CdpAuthenticationMethods> {
public:
    using std::enable_shared_from_this<CdpAuthenticationMethods>::shared_from_this;
    std::shared_ptr<CdpAuthMethod> email;

    std::shared_ptr<CdpAuthMethod> oauth;

    std::shared_ptr<CdpAuthMethod> google;

    std::shared_ptr<CdpSmsMethod> sms;
};

class CdpUser : public object, public std::enable_shared_from_this<CdpUser> {
public:
    using std::enable_shared_from_this<CdpUser>::shared_from_this;
    string userId;

    string email;

    string name;

    string displayName;

    std::shared_ptr<CdpAuthenticationMethods> authenticationMethods;
};

class CdpUserInfoOptions : public object, public std::enable_shared_from_this<CdpUserInfoOptions> {
public:
    using std::enable_shared_from_this<CdpUserInfoOptions>::shared_from_this;
    boolean isSignedIn;
};

class CdpUserInfo : public object, public std::enable_shared_from_this<CdpUserInfo> {
public:
    using std::enable_shared_from_this<CdpUserInfo>::shared_from_this;
    string email;

    string username;

    string phoneNumber;
};

any extractEmailFromCdpUser(any user, boolean isSignedIn);

any extractUsernameFromCdpUser(any user, string emailForFallback = undefined);

any extractPhoneFromCdpUser(any user);

any generateEmailFromPhone(string phone);

std::shared_ptr<CdpUserInfo> resolveCdpUserInfo(any user, std::shared_ptr<CdpUserInfoOptions> options = undefined);

#endif
