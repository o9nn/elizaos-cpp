#include "utils.hpp"
#include <string>

any getZodJsonSchema(std::shared_ptr<ZodType<any>> schema)
{
    return zodToJsonSchema(schema, std::string("schema"))->definitions->schema;
};


any extractXMLFromResponse(string output, string tag)
{
    auto start = output->indexOf(std::string("<") + tag + std::string(">"));
    auto end = output->indexOf(std::string("</") + tag + std::string(">")) + std::string("</") + tag + std::string(">")->get_length();
    if (OR((start == -1), (end == -1))) {
        return string_empty;
    }
    return output->slice(start, end);
};


any parseRecommendationsResponse(string xmlResponse)
{
    auto parser = std::make_shared<XMLParser>(object{
        object::pair{std::string("ignoreAttributes"), false}, 
        object::pair{std::string("isArray"), [=](auto name) mutable
        {
            return name == std::string("recommendation");
        }
        }
    });
    auto result = parser->parse(xmlResponse);
    return OR((result->new_recommendations->recommendation), (array<any>()));
};


any parseTokensResponse(string xmlResponse)
{
    auto parser = std::make_shared<XMLParser>(object{
        object::pair{std::string("ignoreAttributes"), false}, 
        object::pair{std::string("isArray"), [=](auto name) mutable
        {
            return name == std::string("tokenAddress");
        }
        }
    });
    auto result = parser->parse(xmlResponse);
    return OR((result->tokens->tokenAddress), (array<any>()));
};


any parseConfirmationResponse(string xmlResponse)
{
    auto parser = std::make_shared<XMLParser>(object{
        object::pair{std::string("ignoreAttributes"), false}
    });
    auto result = parser->parse(xmlResponse);
    return OR((result->message), (string_empty));
};


any parseSignalResponse(string xmlResponse)
{
    auto parser = std::make_shared<XMLParser>(object{
        object::pair{std::string("ignoreAttributes"), false}
    });
    auto result = parser->parse(xmlResponse);
    return result->signal;
};


any parseTokenResponse(string xmlResponse)
{
    auto parser = std::make_shared<XMLParser>(object{
        object::pair{std::string("ignoreAttributes"), false}
    });
    auto result = parser->parse(xmlResponse);
    return result->token;
};


