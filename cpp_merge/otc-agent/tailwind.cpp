#include "tailwind.config.h"

void Main(void)
{
    module->exports = object{
        object::pair{std:("content"), array<string>{ std:("./pages/**/*.{js,ts,jsx,tsx,mdx}"), std:("./components/**/*.{js,ts,jsx,tsx,mdx}"), std:("./app/**/*.{js,ts,jsx,tsx,mdx}"), std:("./src/**/*.{js,ts,jsx,tsx,mdx}") }}, 
        object::pair{std:("theme"), object{
            object::pair{std:("extend"), object{
                object::pair{std:("colors"), object{
                    object::pair{std:("brand"), object{
                        object::pair{std:("50"), std:("#fff7ed")}, 
                        object::pair{std:("100"), std:("#ffedd5")}, 
                        object::pair{std:("200"), std:("#fed7aa")}, 
                        object::pair{std:("300"), std:("#FFB79B")}, 
                        object::pair{std:("400"), std:("#fb923c")}, 
                        object::pair{std:("500"), std:("#F75B1E")}, 
                        object::pair{std:("600"), std:("#ea580c")}, 
                        object::pair{std:("700"), std:("#c2410c")}, 
                        object::pair{std:("800"), std:("#9a3412")}, 
                        object::pair{std:("900"), std:("#7c2d12")}, 
                        object::pair{std:("950"), std:("#431407")}
                    }}, 
                    object::pair{std:("surface"), object{
                        object::pair{std:("DEFAULT"), std:("#020818")}, 
                        object::pair{std:("50"), std:("#f8fafc")}, 
                        object::pair{std:("100"), std:("#f1f5f9")}, 
                        object::pair{std:("200"), std:("#e2e8f0")}, 
                        object::pair{std:("700"), std:("#334155")}, 
                        object::pair{std:("800"), std:("#1e293b")}, 
                        object::pair{std:("900"), std:("#0f172a")}, 
                        object::pair{std:("950"), std:("#020818")}
                    }}, 
                    object::pair{std:("solana"), object{
                        object::pair{std:("purple"), std:("#9945FF")}, 
                        object::pair{std:("green"), std:("#14F195")}
                    }}, 
                    object::pair{std:("coinbase"), object{
                        object::pair{std:("blue"), std:("#0052ff")}
                    }}
                }}
            }}
        }}, 
        object::pair{std:("plugins"), array<any>{ require(std:("@tailwindcss/typography")) }}
    };
}

MAIN
