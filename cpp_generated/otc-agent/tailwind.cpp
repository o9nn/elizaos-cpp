#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otc-agent/tailwind.config.h"

void Main(void)
{
    module->exports = object{
        object::pair{std::string("content"), array<string>{ std::string("./pages/**/*.{js,ts,jsx,tsx,mdx}"), std::string("./components/**/*.{js,ts,jsx,tsx,mdx}"), std::string("./app/**/*.{js,ts,jsx,tsx,mdx}"), std::string("./src/**/*.{js,ts,jsx,tsx,mdx}") }}, 
        object::pair{std::string("theme"), object{
            object::pair{std::string("extend"), object{
                object::pair{std::string("colors"), object{
                    object::pair{std::string("brand"), object{
                        object::pair{std::string("50"), std::string("#fff7ed")}, 
                        object::pair{std::string("100"), std::string("#ffedd5")}, 
                        object::pair{std::string("200"), std::string("#fed7aa")}, 
                        object::pair{std::string("300"), std::string("#FFB79B")}, 
                        object::pair{std::string("400"), std::string("#fb923c")}, 
                        object::pair{std::string("500"), std::string("#F75B1E")}, 
                        object::pair{std::string("600"), std::string("#ea580c")}, 
                        object::pair{std::string("700"), std::string("#c2410c")}, 
                        object::pair{std::string("800"), std::string("#9a3412")}, 
                        object::pair{std::string("900"), std::string("#7c2d12")}, 
                        object::pair{std::string("950"), std::string("#431407")}
                    }}, 
                    object::pair{std::string("surface"), object{
                        object::pair{std::string("DEFAULT"), std::string("#020818")}, 
                        object::pair{std::string("50"), std::string("#f8fafc")}, 
                        object::pair{std::string("100"), std::string("#f1f5f9")}, 
                        object::pair{std::string("200"), std::string("#e2e8f0")}, 
                        object::pair{std::string("700"), std::string("#334155")}, 
                        object::pair{std::string("800"), std::string("#1e293b")}, 
                        object::pair{std::string("900"), std::string("#0f172a")}, 
                        object::pair{std::string("950"), std::string("#020818")}
                    }}, 
                    object::pair{std::string("solana"), object{
                        object::pair{std::string("purple"), std::string("#9945FF")}, 
                        object::pair{std::string("green"), std::string("#14F195")}
                    }}, 
                    object::pair{std::string("coinbase"), object{
                        object::pair{std::string("blue"), std::string("#0052ff")}
                    }}
                }}
            }}
        }}, 
        object::pair{std::string("plugins"), array<any>{ require(std::string("@tailwindcss/typography")) }}
    };
}

MAIN
