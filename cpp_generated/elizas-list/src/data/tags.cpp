#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-list/src/data/tags.h"

object PROJECT_TAGS = object{
    object::pair{std::string("LANGUAGES"), array<string>{ std::string("JavaScript"), std::string("TypeScript"), std::string("Python"), std::string("Rust"), std::string("Go"), std::string("Solidity"), std::string("C++"), std::string("Java"), std::string("Kotlin"), std::string("Swift"), std::string("PHP"), std::string("Ruby") }}, 
    object::pair{std::string("FRAMEWORKS"), array<string>{ std::string("React"), std::string("Next.js"), std::string("Vue"), std::string("Angular"), std::string("Svelte"), std::string("Express"), std::string("Django"), std::string("Flask"), std::string("Spring Boot"), std::string("Laravel") }}, 
    object::pair{std::string("WEB3"), array<string>{ std::string("Ethereum"), std::string("Solana"), std::string("DeFi"), std::string("NFT"), std::string("Smart Contracts"), std::string("Web3"), std::string("Cryptocurrency"), std::string("Blockchain"), std::string("DEX"), std::string("DAO") }}, 
    object::pair{std::string("AI_ML"), array<string>{ std::string("AI"), std::string("Machine Learning"), std::string("Deep Learning"), std::string("NLP"), std::string("Computer Vision"), std::string("Neural Networks"), std::string("LLM"), std::string("GPT"), std::string("Stable Diffusion"), std::string("MLOps") }}, 
    object::pair{std::string("TOOLS"), array<string>{ std::string("CLI"), std::string("VSCode Extension"), std::string("Chrome Extension"), std::string("SDK"), std::string("API"), std::string("Database"), std::string("ORM"), std::string("Testing"), std::string("Analytics"), std::string("Monitoring") }}
};
object TAG_METADATA = object{
    object::pair{std::string("JavaScript"), object{
        object::pair{std::string("icon"), SiJavascript}, 
        object::pair{std::string("color"), std::string("#F7DF1E")}
    }}, 
    object::pair{std::string("TypeScript"), object{
        object::pair{std::string("icon"), SiTypescript}, 
        object::pair{std::string("color"), std::string("#3178C6")}
    }}, 
    object::pair{std::string("Python"), object{
        object::pair{std::string("icon"), SiPython}, 
        object::pair{std::string("color"), std::string("#3776AB")}
    }}, 
    object::pair{std::string("React"), object{
        object::pair{std::string("icon"), SiReact}, 
        object::pair{std::string("color"), std::string("#61DAFB")}
    }}, 
    object::pair{std::string("Next.js"), object{
        object::pair{std::string("icon"), SiNextdotjs}, 
        object::pair{std::string("color"), std::string("#000000")}
    }}, 
    object::pair{std::string("AI"), object{
        object::pair{std::string("icon"), FaBrain}, 
        object::pair{std::string("color"), std::string("#FF6B6B")}
    }}, 
    object::pair{std::string("Machine Learning"), object{
        object::pair{std::string("icon"), FaRobot}, 
        object::pair{std::string("color"), std::string("#FF6B6B")}
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
