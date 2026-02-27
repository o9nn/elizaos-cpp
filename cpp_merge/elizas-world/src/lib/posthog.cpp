#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-world/src/lib/posthog.h"

void Main(void)
{
    std:("use client");
    if (type_of(window) != std:("undefined")) {
        posthog->init(OR((process->env->NEXT_PUBLIC_POSTHOG_KEY), (std:("phc_BrBxTeQxtL24ebiwkSKxLL93wvoCJ5ayJ7Q5PbeJQdp"))), object{
            object::pair{std:("api_host"), OR((process->env->NEXT_PUBLIC_POSTHOG_HOST), (std:("https://us.i.posthog.com")))}, 
            object::pair{std:("capture_pageview"), false}, 
            object::pair{std:("persistence"), std:("memory")}
        });
    }
}

MAIN
