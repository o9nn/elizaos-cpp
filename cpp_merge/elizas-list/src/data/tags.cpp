#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-list/src/data/tags.h"

object PROJECT_TAGS = object{
    object::pair{std:("LANGUAGES"), array<string>{ std:("JavaScript"), std:("TypeScript"), std:("Python"), std:("Rust"), std:("Go"), std:("Solidity"), std:("C++"), std:("Java"), std:("Kotlin"), std:("Swift"), std:("PHP"), std:("Ruby") }}, 
    object::pair{std:("FRAMEWORKS"), array<string>{ std:("React"), std:("Next.js"), std:("Vue"), std:("Angular"), std:("Svelte"), std:("Express"), std:("Django"), std:("Flask"), std:("Spring Boot"), std:("Laravel") }}, 
    object::pair{std:("WEB3"), array<string>{ std:("Ethereum"), std:("Solana"), std:("DeFi"), std:("NFT"), std:("Smart Contracts"), std:("Web3"), std:("Cryptocurrency"), std:("Blockchain"), std:("DEX"), std:("DAO") }}, 
    object::pair{std:("AI_ML"), array<string>{ std:("AI"), std:("Machine Learning"), std:("Deep Learning"), std:("NLP"), std:("Computer Vision"), std:("Neural Networks"), std:("LLM"), std:("GPT"), std:("Stable Diffusion"), std:("MLOps") }}, 
    object::pair{std:("TOOLS"), array<string>{ std:("CLI"), std:("VSCode Extension"), std:("Chrome Extension"), std:("SDK"), std:("API"), std:("Database"), std:("ORM"), std:("Testing"), std:("Analytics"), std:("Monitoring") }}
};
object TAG_METADATA = object{
    object::pair{std:("JavaScript"), object{
        object::pair{std:("icon"), SiJavascript}, 
        object::pair{std:("color"), std:("#F7DF1E")}
    }}, 
    object::pair{std:("TypeScript"), object{
        object::pair{std:("icon"), SiTypescript}, 
        object::pair{std:("color"), std:("#3178C6")}
    }}, 
    object::pair{std:("Python"), object{
        object::pair{std:("icon"), SiPython}, 
        object::pair{std:("color"), std:("#3776AB")}
    }}, 
    object::pair{std:("React"), object{
        object::pair{std:("icon"), SiReact}, 
        object::pair{std:("color"), std:("#61DAFB")}
    }}, 
    object::pair{std:("Next.js"), object{
        object::pair{std:("icon"), SiNextdotjs}, 
        object::pair{std:("color"), std:("#000000")}
    }}, 
    object::pair{std:("AI"), object{
        object::pair{std:("icon"), FaBrain}, 
        object::pair{std:("color"), std:("#FF6B6B")}
    }}, 
    object::pair{std:("Machine Learning"), object{
        object::pair{std:("icon"), FaRobot}, 
        object::pair{std:("color"), std:("#FF6B6B")}
    }}
};
std::function<string(string)> getTagCategory = [=](auto tag) mutable
{
    for (auto& [category, tags] : Object->entries(PROJECT_TAGS))
    {
        if (tags->includes(tag)) {
            return category;
        }
    }
    return nullptr;
};
std::function<any()> getAllTags = [=]() mutable
{
    return Object->values(PROJECT_TAGS)->flat();
};

void Main(void)
{
}

MAIN
