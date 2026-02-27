#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/autonomous-trader/actions/act_reg_start.h"

any generateRandomString(any length)
{
    auto chars = std:("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
    auto result = string_empty;
    auto charsLength = chars->get_length();
    for (auto i = 0; i < length; i++)
    {
        result += chars->charAt(Math->floor(Math->random() * charsLength));
    }
    return result;
};


any extractEmails(any text)
{
    auto emailRegex = (new RegExp(std:("[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}")));
    auto matches = text["match"](emailRegex);
    return OR((matches), (array<any>()));
};


void sendVerifyEmail(any address, any regCode)
{
    console->log(std:("sending verify email to"), address);
    auto mailOptions = object{
        object::pair{std:("from"), process->env->SMTP_FROM}, 
        object::pair{std:("to"), address}, 
        object::pair{std:("subject"), std:("Welcome to Spartan Services")}, 
        object::pair{std:("text"), std:("Please give Spartan the following code: ") + regCode}
    };
    try
    {
        auto info = std::async([=]() { transporter->sendMail(mailOptions); });
        console->log(std:("Email sent:"), info->envelope);
    }
    catch (const any& error)
    {
        console->error(std:("Error sending email:"), error);
    }
};


any transporter = nodemailer->createTransport(object{
    object::pair{std:("host"), process->env->SMTP_HOST}, 
    object::pair{std:("port"), OR((parseInt(process->env->SMTP_PORT)), (587))}, 
    object::pair{std:("secure"), false}, 
    object::pair{std:("auth"), object{
        object::pair{std:("user"), process->env->SMTP_USERNAME}, 
        object::pair{std:("pass"), process->env->SMTP_PASSWORD}
    }}
});
std::shared_ptr<Action> userRegistration = object{
    object::pair{std:("name"), std:("USER_REGISTRATION")}, 
    object::pair{std:("similes"), array<any>()}, 
    object::pair{std:("validate"), [=](auto runtime, auto message) mutable
    {
        if (!message->metadata->authorId) return false;
        auto entityId = createUniqueUuid(runtime, message->metadata->authorId);
        auto entity = std::async([=]() { runtime->getEntityById(entityId); });
        auto email = entity->components->find([=](auto c) mutable
        {
            return c["type"] == EMAIL_TYPE;
        }
        );
        console->log(std:("reg_start:validate - are signed up?"), !!email);
        return !email;
    }
    }, 
    object::pair{std:("description"), std:("Allows a user to start user registration")}, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto state, auto _options, auto callback = undefined, auto responses) mutable
    {
        console->log(std:("USER_REGISTRATION handler"));
        shared roomDetails = std::async([=]() { runtime->getRoom(message->roomId); });
        console->log(std:("roomDetails"), roomDetails);
        auto roomEntity = std::async([=]() { runtime->getEntityById(message->roomId); });
        console->log(std:("roomEntity"), roomEntity);
        auto agentEntityId = createUniqueUuid(runtime, runtime->agentId);
        auto agentEntity = std::async([=]() { runtime->getEntityById(agentEntityId); });
        console->log(std:("agentEntity"), agentEntity);
        auto spartanData = agentEntity->components->find([=](auto c) mutable
        {
            return c["type"] == SPARTAN_SERVICE_TYPE;
        }
        );
        shared spartanDataNew = false;
        auto spartanDataDelta = false;
        if (!spartanData) {
            spartanDataNew = true;
            spartanData->data = object{
                object::pair{std:("users"), array<any>()}
            };
        }
        shared entityId = createUniqueUuid(runtime, message->metadata->authorId);
        auto entity = std::async([=]() { runtime->getEntityById(entityId); });
        console->log(std:("entity"), entity);
        auto email = entity->components->find([=](auto c) mutable
        {
            return c["type"] == EMAIL_TYPE;
        }
        );
        console->log(std:("email"), email);
        auto emails = extractEmails(message->content->text);
        console->log(std:("would have responded"), responses);
        console->log(std:("emails in message"), emails["length"]);
        if (emails["length"] > 1) {
            if (email) {
                console->log(std:("Write overlap"));
            } else {
                takeItPrivate(runtime, message, std:("What email address would you like to use for registration"));
                responses->get_length() = 0;
            }
        } else if (emails["length"] == 1) {
            auto isLinking = spartanData->users->includes(const_(email)[0]);
            if (isLinking) {
                console->log(std:("this email is already used else where"), isLinking);
            } else {
                auto regCode = generateRandomString(16);
                console->log(std:("sending"), regCode, std:("to email"), const_(emails)[0]);
                std::async([=]() { runtime->createComponent(object{
                    object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                    object::pair{std:("agentId"), runtime->agentId}, 
                    object::pair{std:("worldId"), roomDetails->worldId}, 
                    object::pair{std:("roomId"), message->roomId}, 
                    object::pair{std:("sourceEntityId"), message->entityId}, 
                    object::pair{std:("entityId"), entityId}, 
                    object::pair{std:("type"), EMAIL_TYPE}, 
                    object::pair{std:("data"), object{
                        object::pair{std:("address"), const_(emails)[0]}, 
                        object::pair{std:("code"), regCode}, 
                        object::pair{std:("verified"), false}
                    }}
                }); });
                spartanDataDelta = true;
                spartanData->data->users->push(entityId);
                std::async([=]() { sendVerifyEmail(const_(emails)[0], regCode); });
                takeItPrivate(runtime, message, std:("I just sent you an email (might need to check your spam folder) to confirm ") + const_(emails)[0]);
                responses->get_length() = 0;
            }
            auto updateSpartanData = [=](auto agentEntityId, auto spartanData) mutable
            {
                if (spartanDataNew) {
                    std::async([=]() { runtime->createComponent(object{
                        object::pair{std:("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                        object::pair{std:("agentId"), runtime->agentId}, 
                        object::pair{std:("worldId"), roomDetails->worldId}, 
                        object::pair{std:("roomId"), message->roomId}, 
                        object::pair{std:("sourceEntityId"), entityId}, 
                        object::pair{std:("entityId"), agentEntityId}, 
                        object::pair{std:("type"), SPARTAN_SERVICE_TYPE}, 
                        object::pair{std:("data"), spartanData["data"]}
                    }); });
                } else {
                    std::async([=]() { runtime->updateComponent(object{
                        object::pair{std:("id"), spartanData["id"]}, 
                        object::pair{std:("data"), spartanData["data"]}
                    }); });
                }
            };

            if (spartanDataDelta) {
                updateSpartanData(agentEntityId, spartanData);
            }
        } else {
            if (email) {
                takeItPrivate(runtime, message, std:("Do you want to use ") + email + std:(" for registration?"));
                responses->get_length() = 0;
            } else {
                takeItPrivate(runtime, message, std:("What email address would you like to use for registration"));
                responses->get_length() = 0;
            }
        }
        return std::shared_ptr<Promise<boolean>>();
    }
    }, 
    object::pair{std:("examples"), as<array<array<std::shared_ptr<ActionExample>>>>(array<array<std::shared_ptr<ActionExample>>>{ array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I want to sign up for Spartan services")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I'll help you sign up")}, 
            object::pair{std:("actions"), array<string>{ std:("USER_REGISTRATION") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I want to register")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("What email u wanna use")}, 
            object::pair{std:("actions"), array<string>{ std:("USER_REGISTRATION") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I'd like to sign up")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("based. what email u want me to use")}, 
            object::pair{std:("actions"), array<string>{ std:("USER_REGISTRATION") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I'm thinking about signing up with openai")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("actions"), array<string>{ std:("IGNORE") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("email@email.com")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I'll help verify your email")}, 
            object::pair{std:("actions"), array<string>{ std:("USER_REGISTRATION") }}
        }}
    } } })}
};

void Main(void)
{
}

MAIN
