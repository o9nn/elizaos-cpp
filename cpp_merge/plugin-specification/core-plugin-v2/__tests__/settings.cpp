#include "/home/runner/work/elizaos-cpp/elizaos-cpp/plugin-specification/core-plugin-v2/__tests__/settings.test.h"

void Main(void)
{
    describe(std::string("settings utilities"), [=]() mutable
    {
        it(std::string("createSettingFromConfig copies fields"), [=]() mutable
        {
            auto cfg = as<any>(object{
                object::pair{std::string("name"), std::string("a")}, 
                object::pair{std::string("description"), std::string("d")}, 
                object::pair{std::string("required"), true}
            });
            auto setting = createSettingFromConfig(cfg);
            expect(setting->name)->toBe(std::string("a"));
            expect(setting->required)->toBe(true);
            expect(setting->value)->toBeNull();
        }
        );
        it(std::string("encrypt/decrypt round trip"), [=]() mutable
        {
            auto salt = getSalt();
            auto enc = encryptStringValue(std::string("secret"), salt);
            expect(enc)->not->toBe(std::string("secret"));
            auto dec = decryptStringValue(enc, salt);
            expect(dec)->toBe(std::string("secret"));
        }
        );
        it(std::string("salt and unsalt setting value"), [=]() mutable
        {
            auto salt = getSalt();
            auto setting = as<any>(object{
                object::pair{std::string("value"), std::string("v")}, 
                object::pair{std::string("secret"), true}
            });
            auto salted = saltSettingValue(setting, salt);
            expect(salted->value)->not->toBe(std::string("v"));
            auto unsalted = unsaltSettingValue(salted, salt);
            expect(unsalted->value)->toBe(std::string("v"));
        }
        );
        it(std::string("salt and unsalt world settings"), [=]() mutable
        {
            auto salt = getSalt();
            auto world = as<any>(object{
                object::pair{std::string("a"), object{
                    object::pair{std::string("value"), std::string("x")}, 
                    object::pair{std::string("secret"), true}
                }}
            });
            auto salted = saltWorldSettings(world, salt);
            expect(salted->a->value)->not->toBe(std::string("x"));
            auto unsalted = unsaltWorldSettings(salted, salt);
            expect(unsalted->a->value)->toBe(std::string("x"));
        }
        );
    }
    );
}

MAIN
