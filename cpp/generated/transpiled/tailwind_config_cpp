#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizaos.github.io/tailwind.config.h"

object config = object{
    object::pair{std::string("darkMode"), array<string>{ std::string("class") }}, 
    object::pair{std::string("content"), array<string>{ std::string("./components/**/*.{ts,tsx}"), std::string("./app/**/*.{ts,tsx}"), std::string("./src/**/*.{ts,tsx}") }}, 
    object::pair{std::string("prefix"), string_empty}, 
    object::pair{std::string("theme"), object{
        object::pair{std::string("container"), object{
            object::pair{std::string("center"), true}, 
            object::pair{std::string("padding"), std::string("2rem")}, 
            object::pair{std::string("screens"), object{
                object::pair{std::string("2xl"), std::string("1400px")}
            }}
        }}, 
        object::pair{std::string("extend"), object{
            object::pair{std::string("colors"), object{
                object::pair{std::string("border"), std::string("hsl(var(--border))")}, 
                object::pair{std::string("input"), std::string("hsl(var(--input))")}, 
                object::pair{std::string("ring"), std::string("hsl(var(--ring))")}, 
                object::pair{std::string("background"), std::string("hsl(var(--background))")}, 
                object::pair{std::string("foreground"), std::string("hsl(var(--foreground))")}, 
                object::pair{std::string("primary"), object{
                    object::pair{std::string("DEFAULT"), std::string("hsl(var(--primary))")}, 
                    object::pair{std::string("foreground"), std::string("hsl(var(--primary-foreground))")}
                }}, 
                object::pair{std::string("secondary"), object{
                    object::pair{std::string("DEFAULT"), std::string("hsl(var(--secondary))")}, 
                    object::pair{std::string("foreground"), std::string("hsl(var(--secondary-foreground))")}
                }}, 
                object::pair{std::string("destructive"), object{
                    object::pair{std::string("DEFAULT"), std::string("hsl(var(--destructive))")}, 
                    object::pair{std::string("foreground"), std::string("hsl(var(--destructive-foreground))")}
                }}, 
                object::pair{std::string("muted"), object{
                    object::pair{std::string("DEFAULT"), std::string("hsl(var(--muted))")}, 
                    object::pair{std::string("foreground"), std::string("hsl(var(--muted-foreground))")}
                }}, 
                object::pair{std::string("accent"), object{
                    object::pair{std::string("DEFAULT"), std::string("hsl(var(--accent))")}, 
                    object::pair{std::string("foreground"), std::string("hsl(var(--accent-foreground))")}
                }}, 
                object::pair{std::string("popover"), object{
                    object::pair{std::string("DEFAULT"), std::string("hsl(var(--popover))")}, 
                    object::pair{std::string("foreground"), std::string("hsl(var(--popover-foreground))")}
                }}, 
                object::pair{std::string("card"), object{
                    object::pair{std::string("DEFAULT"), std::string("hsl(var(--card))")}, 
                    object::pair{std::string("foreground"), std::string("hsl(var(--card-foreground))")}
                }}
            }}, 
            object::pair{std::string("borderRadius"), object{
                object::pair{std::string("lg"), std::string("var(--radius)")}, 
                object::pair{std::string("md"), std::string("calc(var(--radius) - 2px)")}, 
                object::pair{std::string("sm"), std::string("calc(var(--radius) - 4px)")}
            }}, 
            object::pair{std::string("fontFamily"), object{
                object::pair{std::string("sans"), array<string>{ std::string("var(--font-sans)"), fontFamily->sans }}
            }}, 
            object::pair{std::string("typography"), [=](auto theme) mutable
            {
                return (object{
                    object::pair{std::string("DEFAULT"), object{
                        object::pair{std::string("css"), object{
                            object::pair{std::string("h2"), object{
                                object::pair{std::string("color"), theme(std::string("colors.primary.DEFAULT"))}
                            }}
                        }}
                    }}, 
                    object::pair{std::string("invert"), object{
                        object::pair{std::string("css"), object{
                            object::pair{std::string("h2"), object{
                                object::pair{std::string("color"), theme(std::string("colors.primary.DEFAULT"))}
                            }}
                        }}
                    }}
                });
            }
            }, 
            object::pair{std::string("keyframes"), object{
                object::pair{std::string("accordion-down"), object{
                    object::pair{std::string("from"), object{
                        object::pair{std::string("height"), std::string("0")}
                    }}, 
                    object::pair{std::string("to"), object{
                        object::pair{std::string("height"), std::string("var(--radix-accordion-content-height)")}
                    }}
                }}, 
                object::pair{std::string("accordion-up"), object{
                    object::pair{std::string("from"), object{
                        object::pair{std::string("height"), std::string("var(--radix-accordion-content-height)")}
                    }}, 
                    object::pair{std::string("to"), object{
                        object::pair{std::string("height"), std::string("0")}
                    }}
                }}
            }}, 
            object::pair{std::string("animation"), object{
                object::pair{std::string("accordion-down"), std::string("accordion-down 0.2s ease-out")}, 
                object::pair{std::string("accordion-up"), std::string("accordion-up 0.2s ease-out")}
            }}
        }}
    }}, 
    object::pair{std::string("plugins"), array<any>{ animate, typography }}
}, satisfies, Config;

void Main(void)
{
}

MAIN
