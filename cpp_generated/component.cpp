#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/client/cypress/support/component.h"

any mountWithProviders(std::shared_ptr<React::ReactNode> component, object options)
{
    auto queryClient = createTestQueryClient();
    auto wrapped = React->createElement(TooltipProvider, object{
        object::pair{std::string("children"), React->createElement(QueryClientProvider, object{
            object::pair{std::string("client"), queryClient}
        }, React->createElement(BrowserRouter, object{}, React->createElement(std::string("div"), object{
            object::pair{std::string("id"), std::string("root")}, 
            object::pair{std::string("style"), object{
                object::pair{std::string("width"), std::string("100%")}, 
                object::pair{std::string("height"), std::string("100%")}
            }}
        }, component, React->createElement(std::string("div"), object{
            object::pair{std::string("id"), std::string("radix-portal")}
        }))))}
    });
    return mount(wrapped, options);
};


any mountWithRouter(std::shared_ptr<React::ReactNode> component, object options)
{
    auto queryClient = createTestQueryClient();
    auto wrapped = React->createElement(TooltipProvider, object{
        object::pair{std::string("children"), React->createElement(QueryClientProvider, object{
            object::pair{std::string("client"), queryClient}
        }, React->createElement(BrowserRouter, object{}, React->createElement(std::string("div"), object{
            object::pair{std::string("id"), std::string("root")}, 
            object::pair{std::string("style"), object{
                object::pair{std::string("width"), std::string("100%")}, 
                object::pair{std::string("height"), std::string("100%")}
            }}
        }, component, React->createElement(std::string("div"), object{
            object::pair{std::string("id"), std::string("radix-portal")}
        }))))}
    });
    return mount(wrapped, options);
};


any mountRadix(std::shared_ptr<React::ReactNode> component, object options)
{
    auto queryClient = createTestQueryClient();
    auto wrapped = React->createElement(TooltipProvider, object{
        object::pair{std::string("children"), React->createElement(DirectionProvider, object{
            object::pair{std::string("dir"), std::string("ltr")}
        }, React->createElement(QueryClientProvider, object{
            object::pair{std::string("client"), queryClient}
        }, React->createElement(BrowserRouter, object{}, React->createElement(std::string("div"), object{
            object::pair{std::string("id"), std::string("root")}, 
            object::pair{std::string("style"), object{
                object::pair{std::string("width"), std::string("100%")}, 
                object::pair{std::string("height"), std::string("100%")}
            }}
        }, component, React->createElement(std::string("div"), object{
            object::pair{std::string("id"), std::string("radix-portal")}
        })))))}
    });
    return mount(wrapped, options);
};


std::function<any()> createTestQueryClient = [=]() mutable
{
    return std::make_shared<QueryClient>(object{
        object::pair{std::string("defaultOptions"), object{
            object::pair{std::string("queries"), object{
                object::pair{std::string("retry"), false}, 
                object::pair{std::string("refetchOnWindowFocus"), false}
            }}, 
            object::pair{std::string("mutations"), object{
                object::pair{std::string("retry"), false}
            }}
        }}
    });
};

void Main(void)
{
    (as<any>(globalThis))["process"] = process;
    (as<any>(globalThis))["process"]["env"] = utils::assign(object{
        object::pair{std::string("SENTRY_DSN"), string_empty}, 
        object::pair{std::string("NODE_ENV"), std::string("test")}
    }, process->env);
    (as<any>(globalThis))["process"]["versions"] = object{
        object::pair{std::string("node"), std::string("18.0.0")}
    };
    (as<any>(globalThis))["Buffer"] = Buffer;
    if (type_of(window) != std::string("undefined")) {
        (as<any>(window))["Buffer"] = Buffer;
    }
    if (type_of(window) != std::string("undefined")) {
        (as<any>(window))["React"] = React;
    }
    if (type_of(globalThis) != std::string("undefined")) {
        (as<any>(globalThis))["React"] = React;
    }
    Cypress->Commands->add(std::string("mount"), mountWithProviders);
    Cypress->Commands->add(std::string("mountWithRouter"), mountWithRouter);
    Cypress->Commands->add(std::string("mountRadix"), mountRadix);
}

MAIN
