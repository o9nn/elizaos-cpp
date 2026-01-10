#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/server/src/prompts/create-token.h"

any createTokenPrompt(any validatedData)
{
    auto userInstructions = string_empty;
    if (AND((validatedData), (validatedData["prompt"]))) {
        userInstructions = std::string("The token should be based on this concept: "") + validatedData["prompt"] + std::string("". \
      Make sure the token name, symbol, description and image prompt directly incorporate elements from this concept.\
      For example, if the concept is "a halloween token about arnold schwarzenegger", the token might be named "Spooky Schwartz" with symbol "SPKS" and an image prompt that describes a muscular halloween figure resembling Arnold.\
      Be creative but stay faithful to the concept.");
    } else {
        auto randomIdea = const_(ideas)[Math->floor(Math->random() * ideas->get_length())];
        auto randomAdjective = const_(adjectives)[Math->floor(Math->random() * adjectives->get_length())];
        auto randomNoun = const_(nouns)[Math->floor(Math->random() * nouns->get_length())];
        auto randomIdea2 = const_(ideas)[Math->floor(Math->random() * ideas->get_length())];
        auto randomAdjective2 = const_(adjectives)[Math->floor(Math->random() * adjectives->get_length())];
        auto randomNoun2 = const_(nouns)[Math->floor(Math->random() * nouns->get_length())];
        userInstructions = std::string("The token should be based on this concept: "") + randomAdjective + std::string(" ") + randomIdea + std::string(" ") + randomNoun + std::string("".\
    Alternatively, you can use the following concept: "") + randomAdjective2 + std::string(" ") + randomIdea2 + std::string(" ") + randomNoun2 + std::string("".\
      Make it edgy, funny, and internet culture related. The name should be catchy and memorable.\
      The symbol should be 3-5 characters and easily recognizable.");
    }
    auto newsApiKey = process->env->NEWS_API_KEY;
    auto trendingTopics = array<any>();
    if (newsApiKey) {
        try
        {
            auto trendingTopicsResponse = std::async([=]() { fetch(std::string("https://newsapi.org/v2/top-headlines?country=us&apiKey=") + newsApiKey + string_empty, object{
                object::pair{std::string("headers"), object{
                    object::pair{std::string("Accept"), std::string("application/json")}
                }}
            }); });
            if (trendingTopicsResponse->ok) {
                auto trendingTopicsJson = std::async([=]() { trendingTopicsResponse->json(); });
                trendingTopics = trendingTopicsJson["articles"]["filter"]([=](auto article) mutable
                {
                    return article["title"];
                }
                )["map"]([=](auto article) mutable
                {
                    return article["title"]->replace((new RegExp(std::string("\s-\s.*"))), string_empty)->trim();
                }
                )["slice"](0, 5);
            }
        }
        catch (const any& error)
        {
            console->error(std::string("Error fetching trending topics:"), error);
        }
    }
    auto trendingTopicsPrompt = string_empty;
    if (trendingTopics->get_length() > 0) {
        trendingTopicsPrompt = std::string("\
        Here are some current trending topics for inspiration (optional):\
        - ") + trendingTopics->join(std::string("\
        - ")) + std::string("\
      ");
    }
    auto content = std::string("Generate prompt and engaging token metadata for a Solana token. The token should be fun, memorable, and captivating to crypto enthusiasts. ") + userInstructions + std::string("\
        \
        ") + trendingTopicsPrompt + std::string("\
        \
        The token should have meme potential and appeal to internet culture. Make it attention-grabbing and shareable.\
        \
        Return ONLY a JSON object with the following fields:\
        - name: A memorable name for the token that clearly reflects the concept (max 20 characters)\
        - symbol: A 3-5 character symbol for the token (preferably all caps)\
        - description: A compelling description of the token that incorporates the concept (100-150 characters)\
        - prompt: A detailed prompt for image generation that will create a visual representation of the concept\
        \
        Example format:\
        ```json\
        ") + const_(exampleFormats)[Math->floor(Math->random() * exampleFormats->get_length())] + std::string("\
        ```\
        \
        Only provide the JSON object. Do not include any other text, explanation, or formatting.");
    return content;
};


