#include "rate-limit.h"

std::function<any()> createApiRateLimit = [=]() mutable
{
    return rateLimit(object{
        object::pair{std:("windowMs"), 15 * 60 * 1000}, 
        object::pair{std:("max"), 1000}, 
        object::pair{std:("message"), object{
            object::pair{std:("success"), false}, 
            object::pair{std:("error"), object{
                object::pair{std:("code"), std:("RATE_LIMIT_EXCEEDED")}, 
                object::pair{std:("message"), std:("Too many requests. Please try again later.")}
            }}
        }}, 
        object::pair{std:("standardHeaders"), true}, 
        object::pair{std:("legacyHeaders"), false}, 
        object::pair{std:("handler"), [=](auto req, auto res) mutable
        {
            auto clientIp = OR((req["ip"]), (std:("unknown")));
            logger->warn(std:("[SECURITY] Rate limit exceeded for IP: ") + clientIp + string_empty);
            res["status"](429)["json"](object{
                object::pair{std:("success"), false}, 
                object::pair{std:("error"), object{
                    object::pair{std:("code"), std:("RATE_LIMIT_EXCEEDED")}, 
                    object::pair{std:("message"), std:("Too many requests. Please try again later.")}
                }}
            });
        }
        }
    });
};
std::function<any()> createFileSystemRateLimit = [=]() mutable
{
    return rateLimit(object{
        object::pair{std:("windowMs"), 5 * 60 * 1000}, 
        object::pair{std:("max"), 100}, 
        object::pair{std:("message"), object{
            object::pair{std:("success"), false}, 
            object::pair{std:("error"), object{
                object::pair{std:("code"), std:("FILE_RATE_LIMIT_EXCEEDED")}, 
                object::pair{std:("message"), std:("Too many file operations. Please try again later.")}
            }}
        }}, 
        object::pair{std:("standardHeaders"), true}, 
        object::pair{std:("legacyHeaders"), false}, 
        object::pair{std:("handler"), [=](auto req, auto res) mutable
        {
            auto clientIp = OR((req["ip"]), (std:("unknown")));
            logger->warn(std:("[SECURITY] File system rate limit exceeded for IP: ") + clientIp + std:(", endpoint: ") + req["path"] + string_empty);
            res["status"](429)["json"](object{
                object::pair{std:("success"), false}, 
                object::pair{std:("error"), object{
                    object::pair{std:("code"), std:("FILE_RATE_LIMIT_EXCEEDED")}, 
                    object::pair{std:("message"), std:("Too many file operations. Please try again later.")}
                }}
            });
        }
        }
    });
};
std::function<any()> createUploadRateLimit = [=]() mutable
{
    return rateLimit(object{
        object::pair{std:("windowMs"), 15 * 60 * 1000}, 
        object::pair{std:("max"), 50}, 
        object::pair{std:("message"), object{
            object::pair{std:("success"), false}, 
            object::pair{std:("error"), object{
                object::pair{std:("code"), std:("UPLOAD_RATE_LIMIT_EXCEEDED")}, 
                object::pair{std:("message"), std:("Too many upload attempts. Please try again later.")}
            }}
        }}, 
        object::pair{std:("standardHeaders"), true}, 
        object::pair{std:("legacyHeaders"), false}, 
        object::pair{std:("handler"), [=](auto req, auto res) mutable
        {
            auto clientIp = OR((req["ip"]), (std:("unknown")));
            logger->warn(std:("[SECURITY] Upload rate limit exceeded for IP: ") + clientIp + std:(", endpoint: ") + req["path"] + string_empty);
            res["status"](429)["json"](object{
                object::pair{std:("success"), false}, 
                object::pair{std:("error"), object{
                    object::pair{std:("code"), std:("UPLOAD_RATE_LIMIT_EXCEEDED")}, 
                    object::pair{std:("message"), std:("Too many upload attempts. Please try again later.")}
                }}
            });
        }
        }
    });
};
std::function<any()> createChannelValidationRateLimit = [=]() mutable
{
    return rateLimit(object{
        object::pair{std:("windowMs"), 10 * 60 * 1000}, 
        object::pair{std:("max"), 200}, 
        object::pair{std:("message"), object{
            object::pair{std:("success"), false}, 
            object::pair{std:("error"), object{
                object::pair{std:("code"), std:("CHANNEL_VALIDATION_RATE_LIMIT_EXCEEDED")}, 
                object::pair{std:("message"), std:("Too many channel validation attempts. Please try again later.")}
            }}
        }}, 
        object::pair{std:("standardHeaders"), true}, 
        object::pair{std:("legacyHeaders"), false}, 
        object::pair{std:("skip"), [=](auto req) mutable
        {
            auto channelId = req["params"]["channelId"];
            if (channelId) {
                auto validatedChannelId = validateChannelId(channelId);
                return !!validatedChannelId;
            }
            return false;
        }
        }, 
        object::pair{std:("handler"), [=](auto req, auto res) mutable
        {
            auto clientIp = OR((req["ip"]), (std:("unknown")));
            auto channelId = OR((req["params"]["channelId"]), (std:("unknown")));
            logger->warn(std:("[SECURITY] Channel validation rate limit exceeded for IP: ") + clientIp + std:(", attempted channel: ") + channelId + string_empty);
            res["status"](429)["json"](object{
                object::pair{std:("success"), false}, 
                object::pair{std:("error"), object{
                    object::pair{std:("code"), std:("CHANNEL_VALIDATION_RATE_LIMIT_EXCEEDED")}, 
                    object::pair{std:("message"), std:("Too many channel validation attempts. Please try again later.")}
                }}
            });
        }
        }
    });
};

void Main(void)
{
}

MAIN
