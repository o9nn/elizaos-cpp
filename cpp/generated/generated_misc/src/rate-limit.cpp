#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/packages/server/src/middleware/rate-limit.h"

std::function<any()> createApiRateLimit = [=]() mutable
{
    return rateLimit(object{
        object::pair{std::string("windowMs"), 15 * 60 * 1000}, 
        object::pair{std::string("max"), 1000}, 
        object::pair{std::string("message"), object{
            object::pair{std::string("success"), false}, 
            object::pair{std::string("error"), object{
                object::pair{std::string("code"), std::string("RATE_LIMIT_EXCEEDED")}, 
                object::pair{std::string("message"), std::string("Too many requests. Please try again later.")}
            }}
        }}, 
        object::pair{std::string("standardHeaders"), true}, 
        object::pair{std::string("legacyHeaders"), false}, 
        object::pair{std::string("handler"), [=](auto req, auto res) mutable
        {
            auto clientIp = OR((req["ip"]), (std::string("unknown")));
            logger->warn(std::string("[SECURITY] Rate limit exceeded for IP: ") + clientIp + string_empty);
            res["status"](429)["json"](object{
                object::pair{std::string("success"), false}, 
                object::pair{std::string("error"), object{
                    object::pair{std::string("code"), std::string("RATE_LIMIT_EXCEEDED")}, 
                    object::pair{std::string("message"), std::string("Too many requests. Please try again later.")}
                }}
            });
        }
        }
    });
};
std::function<any()> createFileSystemRateLimit = [=]() mutable
{
    return rateLimit(object{
        object::pair{std::string("windowMs"), 5 * 60 * 1000}, 
        object::pair{std::string("max"), 100}, 
        object::pair{std::string("message"), object{
            object::pair{std::string("success"), false}, 
            object::pair{std::string("error"), object{
                object::pair{std::string("code"), std::string("FILE_RATE_LIMIT_EXCEEDED")}, 
                object::pair{std::string("message"), std::string("Too many file operations. Please try again later.")}
            }}
        }}, 
        object::pair{std::string("standardHeaders"), true}, 
        object::pair{std::string("legacyHeaders"), false}, 
        object::pair{std::string("handler"), [=](auto req, auto res) mutable
        {
            auto clientIp = OR((req["ip"]), (std::string("unknown")));
            logger->warn(std::string("[SECURITY] File system rate limit exceeded for IP: ") + clientIp + std::string(", endpoint: ") + req["path"] + string_empty);
            res["status"](429)["json"](object{
                object::pair{std::string("success"), false}, 
                object::pair{std::string("error"), object{
                    object::pair{std::string("code"), std::string("FILE_RATE_LIMIT_EXCEEDED")}, 
                    object::pair{std::string("message"), std::string("Too many file operations. Please try again later.")}
                }}
            });
        }
        }
    });
};
std::function<any()> createUploadRateLimit = [=]() mutable
{
    return rateLimit(object{
        object::pair{std::string("windowMs"), 15 * 60 * 1000}, 
        object::pair{std::string("max"), 50}, 
        object::pair{std::string("message"), object{
            object::pair{std::string("success"), false}, 
            object::pair{std::string("error"), object{
                object::pair{std::string("code"), std::string("UPLOAD_RATE_LIMIT_EXCEEDED")}, 
                object::pair{std::string("message"), std::string("Too many upload attempts. Please try again later.")}
            }}
        }}, 
        object::pair{std::string("standardHeaders"), true}, 
        object::pair{std::string("legacyHeaders"), false}, 
        object::pair{std::string("handler"), [=](auto req, auto res) mutable
        {
            auto clientIp = OR((req["ip"]), (std::string("unknown")));
            logger->warn(std::string("[SECURITY] Upload rate limit exceeded for IP: ") + clientIp + std::string(", endpoint: ") + req["path"] + string_empty);
            res["status"](429)["json"](object{
                object::pair{std::string("success"), false}, 
                object::pair{std::string("error"), object{
                    object::pair{std::string("code"), std::string("UPLOAD_RATE_LIMIT_EXCEEDED")}, 
                    object::pair{std::string("message"), std::string("Too many upload attempts. Please try again later.")}
                }}
            });
        }
        }
    });
};
std::function<any()> createChannelValidationRateLimit = [=]() mutable
{
    return rateLimit(object{
        object::pair{std::string("windowMs"), 10 * 60 * 1000}, 
        object::pair{std::string("max"), 200}, 
        object::pair{std::string("message"), object{
            object::pair{std::string("success"), false}, 
            object::pair{std::string("error"), object{
                object::pair{std::string("code"), std::string("CHANNEL_VALIDATION_RATE_LIMIT_EXCEEDED")}, 
                object::pair{std::string("message"), std::string("Too many channel validation attempts. Please try again later.")}
            }}
        }}, 
        object::pair{std::string("standardHeaders"), true}, 
        object::pair{std::string("legacyHeaders"), false}, 
        object::pair{std::string("skip"), [=](auto req) mutable
        {
            auto channelId = req["params"]["channelId"];
            if (channelId) {
                auto validatedChannelId = validateChannelId(channelId);
                return !!validatedChannelId;
            }
            return false;
        }
        }, 
        object::pair{std::string("handler"), [=](auto req, auto res) mutable
        {
            auto clientIp = OR((req["ip"]), (std::string("unknown")));
            auto channelId = OR((req["params"]["channelId"]), (std::string("unknown")));
            logger->warn(std::string("[SECURITY] Channel validation rate limit exceeded for IP: ") + clientIp + std::string(", attempted channel: ") + channelId + string_empty);
            res["status"](429)["json"](object{
                object::pair{std::string("success"), false}, 
                object::pair{std::string("error"), object{
                    object::pair{std::string("code"), std::string("CHANNEL_VALIDATION_RATE_LIMIT_EXCEEDED")}, 
                    object::pair{std::string("message"), std::string("Too many channel validation attempts. Please try again later.")}
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
