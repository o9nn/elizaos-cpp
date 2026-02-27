#include "component.hpp"

any mountWithProviders(std::shared_ptr<React::ReactNode> component, object options)
{
    auto queryClient = createTestQueryClient();
    auto wrapped = React->createElement(TooltipProvider, object{
        object::pair{std:("children"), React->createElement(QueryClientProvider, object{
            object::pair{std:("client"), queryClient}
        }, React->createElement(BrowserRouter, object{}, React->createElement(std:("div"), object{
            object::pair{std:("id"), std:("root")}, 
            object::pair{std:("style"), object{
                object::pair{std:("width"), std:("100%")}, 
                object::pair{std:("height"), std:("100%")}
            }}
        }, component, React->createElement(std:("div"), object{
            object::pair{std:("id"), std:("radix-portal")}
        }))))}
    });
    return mount(wrapped, options);
};


any mountWithRouter(std::shared_ptr<React::ReactNode> component, object options)
{
    auto queryClient = createTestQueryClient();
    auto wrapped = React->createElement(TooltipProvider, object{
        object::pair{std:("children"), React->createElement(QueryClientProvider, object{
            object::pair{std:("client"), queryClient}
        }, React->createElement(BrowserRouter, object{}, React->createElement(std:("div"), object{
            object::pair{std:("id"), std:("root")}, 
            object::pair{std:("style"), object{
                object::pair{std:("width"), std:("100%")}, 
                object::pair{std:("height"), std:("100%")}
            }}
        }, component, React->createElement(std:("div"), object{
            object::pair{std:("id"), std:("radix-portal")}
        }))))}
    });
    return mount(wrapped, options);
};


any mountRadix(std::shared_ptr<React::ReactNode> component, object options)
{
    auto queryClient = createTestQueryClient();
    auto wrapped = React->createElement(TooltipProvider, object{
        object::pair{std:("children"), React->createElement(DirectionProvider, object{
            object::pair{std:("dir"), std:("ltr")}
        }, React->createElement(QueryClientProvider, object{
            object::pair{std:("client"), queryClient}
        }, React->createElement(BrowserRouter, object{}, React->createElement(std:("div"), object{
            object::pair{std:("id"), std:("root")}, 
            object::pair{std:("style"), object{
                object::pair{std:("width"), std:("100%")}, 
                object::pair{std:("height"), std:("100%")}
            }}
        }, component, React->createElement(std:("div"), object{
            object::pair{std:("id"), std:("radix-portal")}
        })))))}
    });
    return mount(wrapped, options);
};


std::function<any()> createTestQueryClient = [=]() mutable
{
    return std::make_shared<QueryClient>(object{
        object::pair{std:("defaultOptions"), object{
            object::pair{std:("queries"), object{
                object::pair{std:("retry"), false}, 
                object::pair{std:("refetchOnWindowFocus"), false}
            }}, 
            object::pair{std:("mutations"), object{
                object::pair{std:("retry"), false}
            }}
        }}
    });
};

void Main(void)
{
    (as<any>(globalThis))["process"] = process;
    (as<any>(globalThis))["process"]["env"] = utils::assign(object{
        object::pair{std:("SENTRY_DSN"), string_empty}, 
        object::pair{std:("NODE_ENV"), std:("test")}
    }, process->env);
    (as<any>(globalThis))["process"]["versions"] = object{
        object::pair{std:("node"), std:("18.0.0")}
    };
    (as<any>(globalThis))["Buffer"] = Buffer;
    if (type_of(window) != std:("undefined")) {
        (as<any>(window))["Buffer"] = Buffer;
    }
    if (type_of(window) != std:("undefined")) {
        (as<any>(window))["React"] = React;
    }
    if (type_of(globalThis) != std:("undefined")) {
        (as<any>(globalThis))["React"] = React;
    }
    Cypress->Commands->add(std:("mount"), mountWithProviders);
    Cypress->Commands->add(std:("mountWithRouter"), mountWithRouter);
    Cypress->Commands->add(std:("mountRadix"), mountRadix);
}

MAIN
