#include "tailwind.config.h"

object config = object{
    object::pair{std:("darkMode"), array<string>{ std:("class") }}, 
    object::pair{std:("content"), array<string>{ std:("./components/**/*.{ts,tsx}"), std:("./app/**/*.{ts,tsx}"), std:("./src/**/*.{ts,tsx}") }}, 
    object::pair{std:("prefix"), string_empty}, 
    object::pair{std:("theme"), object{
        object::pair{std:("container"), object{
            object::pair{std:("center"), true}, 
            object::pair{std:("padding"), std:("2rem")}, 
            object::pair{std:("screens"), object{
                object::pair{std:("2xl"), std:("1400px")}
            }}
        }}, 
        object::pair{std:("extend"), object{
            object::pair{std:("colors"), object{
                object::pair{std:("border"), std:("hsl(var(--border))")}, 
                object::pair{std:("input"), std:("hsl(var(--input))")}, 
                object::pair{std:("ring"), std:("hsl(var(--ring))")}, 
                object::pair{std:("background"), std:("hsl(var(--background))")}, 
                object::pair{std:("foreground"), std:("hsl(var(--foreground))")}, 
                object::pair{std:("primary"), object{
                    object::pair{std:("DEFAULT"), std:("hsl(var(--primary))")}, 
                    object::pair{std:("foreground"), std:("hsl(var(--primary-foreground))")}
                }}, 
                object::pair{std:("secondary"), object{
                    object::pair{std:("DEFAULT"), std:("hsl(var(--secondary))")}, 
                    object::pair{std:("foreground"), std:("hsl(var(--secondary-foreground))")}
                }}, 
                object::pair{std:("destructive"), object{
                    object::pair{std:("DEFAULT"), std:("hsl(var(--destructive))")}, 
                    object::pair{std:("foreground"), std:("hsl(var(--destructive-foreground))")}
                }}, 
                object::pair{std:("muted"), object{
                    object::pair{std:("DEFAULT"), std:("hsl(var(--muted))")}, 
                    object::pair{std:("foreground"), std:("hsl(var(--muted-foreground))")}
                }}, 
                object::pair{std:("accent"), object{
                    object::pair{std:("DEFAULT"), std:("hsl(var(--accent))")}, 
                    object::pair{std:("foreground"), std:("hsl(var(--accent-foreground))")}
                }}, 
                object::pair{std:("popover"), object{
                    object::pair{std:("DEFAULT"), std:("hsl(var(--popover))")}, 
                    object::pair{std:("foreground"), std:("hsl(var(--popover-foreground))")}
                }}, 
                object::pair{std:("card"), object{
                    object::pair{std:("DEFAULT"), std:("hsl(var(--card))")}, 
                    object::pair{std:("foreground"), std:("hsl(var(--card-foreground))")}
                }}
            }}, 
            object::pair{std:("borderRadius"), object{
                object::pair{std:("lg"), std:("var(--radius)")}, 
                object::pair{std:("md"), std:("calc(var(--radius) - 2px)")}, 
                object::pair{std:("sm"), std:("calc(var(--radius) - 4px)")}
            }}, 
            object::pair{std:("fontFamily"), object{
                object::pair{std:("sans"), array<string>{ std:("var(--font-sans)"), fontFamily->sans }}
            }}, 
            object::pair{std:("typography"), [=](auto theme) mutable
            {
                return (object{
                    object::pair{std:("DEFAULT"), object{
                        object::pair{std:("css"), object{
                            object::pair{std:("h2"), object{
                                object::pair{std:("color"), theme(std:("colors.primary.DEFAULT"))}
                            }}
                        }}
                    }}, 
                    object::pair{std:("invert"), object{
                        object::pair{std:("css"), object{
                            object::pair{std:("h2"), object{
                                object::pair{std:("color"), theme(std:("colors.primary.DEFAULT"))}
                            }}
                        }}
                    }}
                });
            }
            }, 
            object::pair{std:("keyframes"), object{
                object::pair{std:("accordion-down"), object{
                    object::pair{std:("from"), object{
                        object::pair{std:("height"), std:("0")}
                    }}, 
                    object::pair{std:("to"), object{
                        object::pair{std:("height"), std:("var(--radix-accordion-content-height)")}
                    }}
                }}, 
                object::pair{std:("accordion-up"), object{
                    object::pair{std:("from"), object{
                        object::pair{std:("height"), std:("var(--radix-accordion-content-height)")}
                    }}, 
                    object::pair{std:("to"), object{
                        object::pair{std:("height"), std:("0")}
                    }}
                }}
            }}, 
            object::pair{std:("animation"), object{
                object::pair{std:("accordion-down"), std:("accordion-down 0.2s ease-out")}, 
                object::pair{std:("accordion-up"), std:("accordion-up 0.2s ease-out")}
            }}
        }}
    }}, 
    object::pair{std:("plugins"), array<any>{ animate, typography }}
}, satisfies, Config;

void Main(void)
{
}

MAIN
