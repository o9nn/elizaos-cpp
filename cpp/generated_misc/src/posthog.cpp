#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-world/src/lib/posthog.h"

void Main(void)
{
    std::string("use client");
    if (type_of(window) != std::string("undefined")) {
        posthog->init(OR((process->env->NEXT_PUBLIC_POSTHOG_KEY), (std::string("phc_BrBxTeQxtL24ebiwkSKxLL93wvoCJ5ayJ7Q5PbeJQdp"))), object{
            object::pair{std::string("api_host"), OR((process->env->NEXT_PUBLIC_POSTHOG_HOST), (std::string("https://us.i.posthog.com")))}, 
            object::pair{std::string("capture_pageview"), false}, 
            object::pair{std::string("persistence"), std::string("memory")}
        });
    }
}

MAIN
