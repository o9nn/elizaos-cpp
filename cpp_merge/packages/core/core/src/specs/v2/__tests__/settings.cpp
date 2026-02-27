#include "settings.test.h"

void Main(void)
{
    describe(std:("settings utilities"), [=]() mutable
    {
        it(std:("createSettingFromConfig copies fields"), [=]() mutable
        {
            auto cfg = as<any>(object{
                object::pair{std:("name"), std:("a")}, 
                object::pair{std:("description"), std:("d")}, 
                object::pair{std:("required"), true}
            });
            auto setting = createSettingFromConfig(cfg);
            expect(setting->name)->toBe(std:("a"));
            expect(setting->required)->toBe(true);
            expect(setting->value)->toBeNull();
        }
        );
        it(std:("encrypt/decrypt round trip"), [=]() mutable
        {
            auto salt = getSalt();
            auto enc = encryptStringValue(std:("secret"), salt);
            expect(enc)->not->toBe(std:("secret"));
            auto dec = decryptStringValue(enc, salt);
            expect(dec)->toBe(std:("secret"));
        }
        );
        it(std:("salt and unsalt setting value"), [=]() mutable
        {
            auto salt = getSalt();
            auto setting = as<any>(object{
                object::pair{std:("value"), std:("v")}, 
                object::pair{std:("secret"), true}
            });
            auto salted = saltSettingValue(setting, salt);
            expect(salted->value)->not->toBe(std:("v"));
            auto unsalted = unsaltSettingValue(salted, salt);
            expect(unsalted->value)->toBe(std:("v"));
        }
        );
        it(std:("salt and unsalt world settings"), [=]() mutable
        {
            auto salt = getSalt();
            auto world = as<any>(object{
                object::pair{std:("a"), object{
                    object::pair{std:("value"), std:("x")}, 
                    object::pair{std:("secret"), true}
                }}
            });
            auto salted = saltWorldSettings(world, salt);
            expect(salted->a->value)->not->toBe(std:("x"));
            auto unsalted = unsaltWorldSettings(salted, salt);
            expect(unsalted->a->value)->toBe(std:("x"));
        }
        );
    }
    );
}

MAIN
