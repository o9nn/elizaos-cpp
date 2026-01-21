#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza-nextjs-starter/src/lib/uuid-utils.h"

string generateUUIDFromString(string inputString)
{
    if (type_of(inputString) != std::string("string")) {
        throw any(std::make_shared<TypeError>(std::string("Value must be a string")));
    }
    auto hashBytes = sha1->array(inputString);
    auto hashBuffer = std::make_shared<Uint8Array>(hashBytes);
    (*hashBuffer)[6] = std::bit_or()((std::bit_and()((*const_(hashBuffer))[6], 15)), 80);
    (*hashBuffer)[8] = std::bit_or()((std::bit_and()((*const_(hashBuffer))[8], 63)), 128);
    return string_empty + uint8ArrayToHex(hashBuffer->slice(0, 4)) + std::string("-") + uint8ArrayToHex(hashBuffer->slice(4, 6)) + std::string("-") + uint8ArrayToHex(hashBuffer->slice(6, 8)) + std::string("-") + uint8ArrayToHex(hashBuffer->slice(8, 10)) + std::string("-") + uint8ArrayToHex(hashBuffer->slice(10, 16)) + string_empty;
};


string generateQueryRoomId(string seed, string query)
{
    auto sanitizedQuery = query->trim()->toLowerCase()->substring(0, 100);
    auto combinedString = string_empty + seed + std::string("::query::") + sanitizedQuery + string_empty;
    return generateUUIDFromString(combinedString);
};


std::function<string(std::shared_ptr<Uint8Array>)> uint8ArrayToHex = [=](auto buf) mutable
{
    auto out = string_empty;
    for (auto i = 0; i < buf->length; i++)
    {
        auto h = const_(buf)[i]->toString(16);
        if (h->get_length() < 2) {
            h = std::string("0") + h;
        }
        out += h;
    }
    return out;
};

void Main(void)
{
}

MAIN
