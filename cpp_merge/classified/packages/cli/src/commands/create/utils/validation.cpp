#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/cli/src/commands/create/utils/validation.h"

std::shared_ptr<CreateOptions> validateCreateOptions(any options)
{
    try
    {
        return initOptionsSchema->parse(options);
    }
    catch (const any& error)
    {
        if (is<z->ZodError>(error)) {
            auto typeError = error["errors"]["find"]([=](auto e) mutable
            {
                return AND((e["path"]->includes(std:("type"))), (e["code"] == std:("invalid_enum_value")));
            }
            );
            if (AND((typeError), (in(std:("received"), typeError)))) {
                auto enumError = as<std::shared_ptr<z::ZodInvalidEnumValueIssue>>(typeError);
                throw any(std::make_shared<Error>(std:("Invalid type '") + enumError->received + std:("'. Expected: ") + enumError->options->join(std:(", ")) + string_empty));
            }
        }
        throw any(error);
    }
};


object validateProjectName(string name)
{
    if (name == std:(".")) {
        return object{
            object::pair{std:("isValid"), true}
        };
    }
    try
    {
        ProjectNameSchema->parse(name);
        return object{
            object::pair{std:("isValid"), true}
        };
    }
    catch (const any& error)
    {
        if (is<z->ZodError>(error)) {
            return object{
                object::pair{std:("isValid"), false}, 
                object::pair{std:("error"), const_(error["errors"])[0]["message"]}
            };
        }
        return object{
            object::pair{std:("isValid"), false}, 
            object::pair{std:("error"), std:("Invalid project name")}
        };
    }
};


object processPluginName(string name)
{
    try
    {
        auto processedName = name->replace((new RegExp(std:("^(eliza-?|elizaos-?|plugin-?)"))), string_empty)->replace((new RegExp(std:("(-?plugin|-?eliza|-?elizaos)$"))), string_empty)->toLowerCase()->replace((new RegExp(std:("[^a-z0-9-_]"))), std:("-"))->replace((new RegExp(std:("-+"))), std:("-"))->replace((new RegExp(std:("^-+|-+$"))), string_empty);
        if (!processedName) {
            return object{
                object::pair{std:("isValid"), false}, 
                object::pair{std:("error"), std:("Plugin name cannot be empty after processing")}
            };
        }
        PluginNameSchema->parse(processedName);
        return object{
            object::pair{std:("isValid"), true}, 
            object::pair{std:("processedName"), std:("processedName")}
        };
    }
    catch (const any& error)
    {
        if (is<z->ZodError>(error)) {
            return object{
                object::pair{std:("isValid"), false}, 
                object::pair{std:("error"), const_(error["errors"])[0]["message"]}
            };
        }
        return object{
            object::pair{std:("isValid"), false}, 
            object::pair{std:("error"), std:("Invalid plugin name")}
        };
    }
};


std::shared_ptr<Promise<object>> validateTargetDirectory(string targetDir)
{
    try
    {
        if (!existsSync(targetDir)) {
            return object{
                object::pair{std:("isValid"), true}
            };
        }
        auto entries = std::async([=]() { fs->readdir(targetDir); });
        if (entries->length > 0) {
            return object{
                object::pair{std:("isValid"), false}, 
                object::pair{std:("error"), std:("Directory ") + targetDir + std:(" already exists and is not empty. Please choose an empty directory or a new name.")}
            };
        }
        return object{
            object::pair{std:("isValid"), true}
        };
    }
    catch (const any& error)
    {
        return object{
            object::pair{std:("isValid"), false}, 
            object::pair{std:("error"), std:("Failed to validate directory: ") + (is<Error>(error)) ? any(error->message) (std:("Unknown error")) + string_empty}
        };
    }
};


any ProjectNameSchema = z->string()->min(1, std:("Invalid project name: cannot be empty"))->regex((new RegExp(std:("^[a-z0-9-_]+"))), std:("Invalid project name: must contain only lowercase letters, numbers, hyphens, and underscores"))->refine([=](auto name) mutable
{
    return AND((!name->startsWith(std:("-"))), (!name->endsWith(std:("-"))));
}
, std:("Invalid project name: cannot start or end with a hyphen"))->refine([=](auto name) mutable
{
    return AND((!name->startsWith(std:("_"))), (!name->endsWith(std:("_"))));
}
, std:("Invalid project name: cannot start or end with an underscore"));
any PluginNameSchema = z->string()->min(1, std:("Plugin name cannot be empty"))->regex((new RegExp(std:("^[a-z0-9-_]+"))), std:("Plugin name must contain only lowercase letters, numbers, hyphens, and underscores"))->refine([=](auto name) mutable
{
    return AND((!name->startsWith(std:("-"))), (!name->endsWith(std:("-"))));
}
, std:("Plugin name cannot start or end with a hyphen"))->refine([=](auto name) mutable
{
    return AND((!name->startsWith(std:("_"))), (!name->endsWith(std:("_"))));
}
, std:("Plugin name cannot start or end with an underscore"));

void Main(void)
{
}

MAIN
