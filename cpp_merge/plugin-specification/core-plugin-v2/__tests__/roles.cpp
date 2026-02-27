#include "/home/runner/work/elizaos-cpp/elizaos-cpp/plugin-specification/core-plugin-v2/__tests__/roles.test.h"

void Main(void)
{
    describe(std:("roles utilities"), [=]() mutable
    {
        shared runtime = as<any>(object{
            object::pair{std:("getWorld"), [=](auto id) mutable
            {
                return (object{
                    object::pair{std:("id"), std:("id")}, 
                    object::pair{std:("metadata"), object{
                        object::pair{std:("roles"), object{
                            object::pair{std:("user"), Role::ADMIN}
                        }}
                    }}
                });
            }
            }, 
            object::pair{std:("getAllWorlds"), [=]() mutable
            {
                return array<object>{ object{
                    object::pair{std:("metadata"), object{
                        object::pair{std:("ownership"), object{
                            object::pair{std:("ownerId"), std:("owner1")}
                        }}
                    }}
                }, object{
                    object::pair{std:("metadata"), object{
                        object::pair{std:("ownership"), object{
                            object::pair{std:("ownerId"), std:("other")}
                        }}
                    }}
                } };
            }
            }
        });
        it(std:("getUserServerRole returns role from world metadata"), [=]() mutable
        {
            auto role = std::async([=]() { getUserServerRole(runtime, std:("user"), std:("server")); });
            expect(role)->toBe(Role::ADMIN);
        }
        );
        it(std:("findWorldsForOwner finds owned worlds"), [=]() mutable
        {
            auto worlds = std::async([=]() { findWorldsForOwner(runtime, std:("owner1")); });
            expect(worlds->get_length())->toBe(1);
        }
        );
    }
    );
}

MAIN
