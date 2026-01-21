#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/frontend/lib/cdpUser.h"

std::any extractEmailFromCdpUser(std::any user, boolean isSignedIn)
{
    if (!user) return undefined;
    return (OR((OR((OR((OR((user->authenticationMethods->email->email), (user->authenticationMethods->oauth->email))), (user->authenticationMethods->google->email))), (user->email))), (((AND((isSignedIn), (user->userId))) ? std::any(string_empty + user->userId + std::string("@cdp.local")) : std::any(undefined)))));
};


std::any extractUsernameFromCdpUser(std::any user, std::string emailForFallback)
{
    if (!user) return (emailForFallback) ? std::any(const_(emailForFallback->split(std::string("@")))[0]) : std::any(undefined);
    return (OR((OR((OR((OR((OR((user->authenticationMethods->oauth->name), (user->authenticationMethods->google->name))), (user->authenticationMethods->email->name))), (user->name))), (user->displayName))), (((emailForFallback) ? std::any(const_(emailForFallback->split(std::string("@")))[0]) : std::any(undefined)))));
};


std::any extractPhoneFromCdpUser(std::any user)
{
    if (!user) return undefined;
    auto sms = user->authenticationMethods->sms;
    if (!sms) return undefined;
    auto raw = sms->phoneNumber;
    auto cc = sms->countryCode;
    auto combined = (raw->startsWith(std::string("+"))) ? raw : ((AND((raw), (cc))) ? string_empty + cc + string_empty + raw + string_empty : raw);
    if (!combined) return undefined;
    auto digits = combined->replace((new RegExp(std::string("[^0-9]"))), string_empty);
    return (digits) ? std::any(std::string("+") + digits + string_empty) : std::any(undefined);
};


std::any generateEmailFromPhone(std::string phone)
{
    if (!phone) return undefined;
    auto digits = phone->replace((new RegExp(std::string("[^0-9]"))), string_empty);
    if (!digits) return undefined;
    return std::string("p") + digits + std::string("@cdp.local");
};


std::shared_ptr<CdpUserInfo> resolveCdpUserInfo(std::any user, std::shared_ptr<CdpUserInfoOptions> options)
{
    auto phoneNumber = extractPhoneFromCdpUser(user);
    auto email = OR((OR((extractEmailFromCdpUser(user, Boolean(options->isSignedIn))), (((phoneNumber) ? std::any(generateEmailFromPhone(phoneNumber)) : std::any(undefined))))), (((AND((Boolean(options->isSignedIn)), (user->userId))) ? std::any(string_empty + user->userId + std::string("@cdp.local")) : std::any(undefined))));
    auto username = extractUsernameFromCdpUser(user, email);
    return object{
        object::pair{std::string("email"), std::string("email")}, 
        object::pair{std::string("username"), std::string("username")}, 
        object::pair{std::string("phoneNumber"), std::string("phoneNumber")}
    };
};


