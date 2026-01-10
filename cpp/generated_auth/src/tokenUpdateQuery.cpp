#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/server/src/routes/validators/tokenUpdateQuery.h"

object parseUpdateTokenRequest(object raw)
{
    auto mint = parseSolanaAddress(raw["mint"], std::string("mint address"));
    auto body = updateTokenBodySchema->parse(raw["body"]);
    if (OR((OR((type_of(raw["user"]) != std::string("object")), (raw["user"] == nullptr))), (type_of((as<any>(raw["user"]))["id"]) != std::string("string")))) {
        throw any(std::make_shared<Error>(std::string("Unauthorized: missing or invalid user")));
    }
    auto userId = (as<any>(raw["user"]))["id"];
    return object{
        object::pair{std::string("mint"), std::string("mint")}, 
        object::pair{std::string("body"), std::string("body")}, 
        object::pair{std::string("userId"), std::string("userId")}
    };
};


any updateTokenBodySchema = z->object(object{
    object::pair{std::string("website"), z->string()->url()->optional()}, 
    object::pair{std::string("twitter"), z->string()->url()->optional()}, 
    object::pair{std::string("telegram"), z->string()->url()->optional()}, 
    object::pair{std::string("discord"), z->string()->url()->optional()}, 
    object::pair{std::string("farcaster"), z->string()->optional()}
});

void Main(void)
{
}

MAIN
