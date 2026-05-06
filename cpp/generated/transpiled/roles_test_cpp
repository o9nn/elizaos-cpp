#include "/home/runner/work/elizaos-cpp/elizaos-cpp/plugin-specification/core-plugin-v2/__tests__/roles.test.h"

void Main(void)
{
    describe(std::string("roles utilities"), [=]() mutable
    {
        shared runtime = as<any>(object{
            object::pair{std::string("getWorld"), [=](auto id) mutable
            {
                return (object{
                    object::pair{std::string("id"), std::string("id")}, 
                    object::pair{std::string("metadata"), object{
                        object::pair{std::string("roles"), object{
                            object::pair{std::string("user"), Role::ADMIN}
                        }}
                    }}
                });
            }
            }, 
            object::pair{std::string("getAllWorlds"), [=]() mutable
            {
                return array<object>{ object{
                    object::pair{std::string("metadata"), object{
                        object::pair{std::string("ownership"), object{
                            object::pair{std::string("ownerId"), std::string("owner1")}
                        }}
                    }}
                }, object{
                    object::pair{std::string("metadata"), object{
                        object::pair{std::string("ownership"), object{
                            object::pair{std::string("ownerId"), std::string("other")}
                        }}
                    }}
                } };
            }
            }
        });
        it(std::string("getUserServerRole returns role from world metadata"), [=]() mutable
        {
            auto role = std::async([=]() { getUserServerRole(runtime, std::string("user"), std::string("server")); });
            expect(role)->toBe(Role::ADMIN);
        }
        );
        it(std::string("findWorldsForOwner finds owned worlds"), [=]() mutable
        {
            auto worlds = std::async([=]() { findWorldsForOwner(runtime, std::string("owner1")); });
            expect(worlds->get_length())->toBe(1);
        }
        );
    }
    );
}

MAIN
