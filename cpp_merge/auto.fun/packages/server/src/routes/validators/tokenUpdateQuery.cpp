#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/server/src/routes/validators/tokenUpdateQuery.h"

object parseUpdateTokenRequest(object raw)
{
    auto mint = parseSolanaAddress(raw["mint"], std:("mint address"));
    auto body = updateTokenBodySchema->parse(raw["body"]);
    if (OR((OR((type_of(raw["user"]) != std:("object")), (raw["user"] == nullptr))), (type_of((as<any>(raw["user"]))["id"]) != std:("string")))) {
        throw any(std::make_shared<Error>(std:("Unauthorized: missing or invalid user")));
    }
    auto userId = (as<any>(raw["user"]))["id"];
    return object{
        object::pair{std:("mint"), std:("mint")}, 
        object::pair{std:("body"), std:("body")}, 
        object::pair{std:("userId"), std:("userId")}
    };
};


any updateTokenBodySchema = z->object(object{
    object::pair{std:("website"), z->string()->url()->optional()}, 
    object::pair{std:("twitter"), z->string()->url()->optional()}, 
    object::pair{std:("telegram"), z->string()->url()->optional()}, 
    object::pair{std:("discord"), z->string()->url()->optional()}, 
    object::pair{std:("farcaster"), z->string()->optional()}
});

void Main(void)
{
}

MAIN
