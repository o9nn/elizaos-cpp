#include "cdpUser.hpp"
#include <string>

any extractEmailFromCdpUser(any user, boolean isSignedIn)
{
    if (!user) return std::nullopt;
    return (OR((OR((OR((OR((user->authenticationMethods->email->email), (user->authenticationMethods->oauth->email))), (user->authenticationMethods->google->email))), (user->email))), (((AND((isSignedIn), (user->userId))) ? any(string_empty + user->userId + std::string("@cdp.local")) (std::nullopt)))));
};


any extractUsernameFromCdpUser(any user, string emailForFallback)
{
    if (!user) return (emailForFallback) ? any(const_(emailForFallback->split(std::string("@")))[0]) (std::nullopt);
    return (OR((OR((OR((OR((OR((user->authenticationMethods->oauth->name), (user->authenticationMethods->google->name))), (user->authenticationMethods->email->name))), (user->name))), (user->displayName))), (((emailForFallback) ? any(const_(emailForFallback->split(std::string("@")))[0]) (std::nullopt)))));
};


any extractPhoneFromCdpUser(any user)
{
    if (!user) return std::nullopt;
    auto sms = user->authenticationMethods->sms;
    if (!sms) return std::nullopt;
    auto raw = sms->phoneNumber;
    auto cc = sms->countryCode;
    auto combined = (raw->startsWith(std::string("+"))) ? raw : ((AND((raw), (cc))) ? string_empty + cc + string_empty + raw + string_empty : raw);
    if (!combined) return std::nullopt;
    auto digits = combined->replace((new RegExp(std::string("[^0-9]"))), string_empty);
    return (digits) ? any(std::string("+") + digits + string_empty) (std::nullopt);
};


any generateEmailFromPhone(string phone)
{
    if (!phone) return std::nullopt;
    auto digits = phone->replace((new RegExp(std::string("[^0-9]"))), string_empty);
    if (!digits) return std::nullopt;
    return std::string("p") + digits + std::string("@cdp.local");
};


std::shared_ptr<CdpUserInfo> resolveCdpUserInfo(any user, std::shared_ptr<CdpUserInfoOptions> options)
{
    auto phoneNumber = extractPhoneFromCdpUser(user);
    auto email = OR((OR((extractEmailFromCdpUser(user, Boolean(options->isSignedIn))), (((phoneNumber) ? any(generateEmailFromPhone(phoneNumber)) (std::nullopt))))), (((AND((Boolean(options->isSignedIn)), (user->userId))) ? any(string_empty + user->userId + std::string("@cdp.local")) (std::nullopt))));
    auto username = extractUsernameFromCdpUser(user, email);
    return object{
        object::pair{std::string("email"), std::string("email")}, 
        object::pair{std::string("username"), std::string("username")}, 
        object::pair{std::string("phoneNumber"), std::string("phoneNumber")}
    };
};


