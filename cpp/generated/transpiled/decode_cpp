#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizaos.github.io/src/lib/decode.h"

string decodeBase64(string encodedString)
{
    try
    {
        auto binaryString = atob(encodedString);
        auto bytes = std::make_shared<Uint8Array>(binaryString->get_length());
        for (auto i = 0; i < binaryString->get_length(); i++)
        {
            (*bytes)[i] = binaryString->charCodeAt(i);
        }
        auto decodedContent = ((std::make_shared<TextDecoder>()))->decode(bytes);
        return decodedContent;
    }
    catch (const any& error)
    {
        throw any(std::make_shared<Error>(std::string("Failed to decode base64 string: ") + (is<Error>(error)) ? any(error->message) : any(std::string("Unknown error")) + string_empty));
    }
};


