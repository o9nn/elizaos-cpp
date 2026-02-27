#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/autonomous-trader/actions/act_reg_start.h"

any generateRandomString(any length)
{
    auto chars = std::string("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
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
    auto emailRegex = (new RegExp(std::string("[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}")));
    auto matches = text["match"](emailRegex);
    return OR((matches), (array<any>()));
};


void sendVerifyEmail(any address, any regCode)
{
    console->log(std::string("sending verify email to"), address);
    auto mailOptions = object{
        object::pair{std::string("from"), process->env->SMTP_FROM}, 
        object::pair{std::string("to"), address}, 
        object::pair{std::string("subject"), std::string("Welcome to Spartan Services")}, 
        object::pair{std::string("text"), std::string("Please give Spartan the following code: ") + regCode}
    };
    try
    {
        auto info = std::async([=]() { transporter->sendMail(mailOptions); });
        console->log(std::string("Email sent:"), info->envelope);
    }
    catch (const any& error)
    {
        console->error(std::string("Error sending email:"), error);
    }
};


any transporter = nodemailer->createTransport(object{
    object::pair{std::string("host"), process->env->SMTP_HOST}, 
    object::pair{std::string("port"), OR((parseInt(process->env->SMTP_PORT)), (587))}, 
    object::pair{std::string("secure"), false}, 
    object::pair{std::string("auth"), object{
        object::pair{std::string("user"), process->env->SMTP_USERNAME}, 
        object::pair{std::string("pass"), process->env->SMTP_PASSWORD}
    }}
});
std::shared_ptr<Action> userRegistration = object{
    object::pair{std::string("name"), std::string("USER_REGISTRATION")}, 
    object::pair{std::string("similes"), array<any>()}, 
    object::pair{std::string("validate"), [=](auto runtime, auto message) mutable
    {
        if (!message->metadata->authorId) return false;
        auto entityId = createUniqueUuid(runtime, message->metadata->authorId);
        auto entity = std::async([=]() { runtime->getEntityById(entityId); });
        auto email = entity->components->find([=](auto c) mutable
        {
            return c["type"] == EMAIL_TYPE;
        }
        );
        console->log(std::string("reg_start:validate - are signed up?"), !!email);
        return !email;
    }
    }, 
    object::pair{std::string("description"), std::string("Allows a user to start user registration")}, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto state, auto _options, auto callback = undefined, auto responses) mutable
    {
        console->log(std::string("USER_REGISTRATION handler"));
        shared roomDetails = std::async([=]() { runtime->getRoom(message->roomId); });
        console->log(std::string("roomDetails"), roomDetails);
        auto roomEntity = std::async([=]() { runtime->getEntityById(message->roomId); });
        console->log(std::string("roomEntity"), roomEntity);
        auto agentEntityId = createUniqueUuid(runtime, runtime->agentId);
        auto agentEntity = std::async([=]() { runtime->getEntityById(agentEntityId); });
        console->log(std::string("agentEntity"), agentEntity);
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
                object::pair{std::string("users"), array<any>()}
            };
        }
        shared entityId = createUniqueUuid(runtime, message->metadata->authorId);
        auto entity = std::async([=]() { runtime->getEntityById(entityId); });
        console->log(std::string("entity"), entity);
        auto email = entity->components->find([=](auto c) mutable
        {
            return c["type"] == EMAIL_TYPE;
        }
        );
        console->log(std::string("email"), email);
        auto emails = extractEmails(message->content->text);
        console->log(std::string("would have responded"), responses);
        console->log(std::string("emails in message"), emails["length"]);
        if (emails["length"] > 1) {
            if (email) {
                console->log(std::string("Write overlap"));
            } else {
                takeItPrivate(runtime, message, std::string("What email address would you like to use for registration"));
                responses->get_length() = 0;
            }
        } else if (emails["length"] == 1) {
            auto isLinking = spartanData->users->includes(const_(email)[0]);
            if (isLinking) {
                console->log(std::string("this email is already used else where"), isLinking);
            } else {
                auto regCode = generateRandomString(16);
                console->log(std::string("sending"), regCode, std::string("to email"), const_(emails)[0]);
                std::async([=]() { runtime->createComponent(object{
                    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                    object::pair{std::string("agentId"), runtime->agentId}, 
                    object::pair{std::string("worldId"), roomDetails->worldId}, 
                    object::pair{std::string("roomId"), message->roomId}, 
                    object::pair{std::string("sourceEntityId"), message->entityId}, 
                    object::pair{std::string("entityId"), entityId}, 
                    object::pair{std::string("type"), EMAIL_TYPE}, 
                    object::pair{std::string("data"), object{
                        object::pair{std::string("address"), const_(emails)[0]}, 
                        object::pair{std::string("code"), regCode}, 
                        object::pair{std::string("verified"), false}
                    }}
                }); });
                spartanDataDelta = true;
                spartanData->data->users->push(entityId);
                std::async([=]() { sendVerifyEmail(const_(emails)[0], regCode); });
                takeItPrivate(runtime, message, std::string("I just sent you an email (might need to check your spam folder) to confirm ") + const_(emails)[0]);
                responses->get_length() = 0;
            }
            auto updateSpartanData = [=](auto agentEntityId, auto spartanData) mutable
            {
                if (spartanDataNew) {
                    std::async([=]() { runtime->createComponent(object{
                        object::pair{std::string("id"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                        object::pair{std::string("agentId"), runtime->agentId}, 
                        object::pair{std::string("worldId"), roomDetails->worldId}, 
                        object::pair{std::string("roomId"), message->roomId}, 
                        object::pair{std::string("sourceEntityId"), entityId}, 
                        object::pair{std::string("entityId"), agentEntityId}, 
                        object::pair{std::string("type"), SPARTAN_SERVICE_TYPE}, 
                        object::pair{std::string("data"), spartanData["data"]}
                    }); });
                } else {
                    std::async([=]() { runtime->updateComponent(object{
                        object::pair{std::string("id"), spartanData["id"]}, 
                        object::pair{std::string("data"), spartanData["data"]}
                    }); });
                }
            };

            if (spartanDataDelta) {
                updateSpartanData(agentEntityId, spartanData);
            }
        } else {
            if (email) {
                takeItPrivate(runtime, message, std::string("Do you want to use ") + email + std::string(" for registration?"));
                responses->get_length() = 0;
            } else {
                takeItPrivate(runtime, message, std::string("What email address would you like to use for registration"));
                responses->get_length() = 0;
            }
        }
        return std::shared_ptr<Promise<boolean>>();
    }
    }, 
    object::pair{std::string("examples"), as<array<array<std::shared_ptr<ActionExample>>>>(array<array<std::shared_ptr<ActionExample>>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I want to sign up for Spartan services")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I'll help you sign up")}, 
            object::pair{std::string("actions"), array<string>{ std::string("USER_REGISTRATION") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I want to register")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("What email u wanna use")}, 
            object::pair{std::string("actions"), array<string>{ std::string("USER_REGISTRATION") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I'd like to sign up")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("based. what email u want me to use")}, 
            object::pair{std::string("actions"), array<string>{ std::string("USER_REGISTRATION") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I'm thinking about signing up with openai")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("actions"), array<string>{ std::string("IGNORE") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("email@email.com")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I'll help verify your email")}, 
            object::pair{std::string("actions"), array<string>{ std::string("USER_REGISTRATION") }}
        }}
    } } })}
};

void Main(void)
{
}

MAIN
