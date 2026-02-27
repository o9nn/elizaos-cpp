#include "validation.hpp"
#include <string>

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
                return AND((e["path"]->includes(std::string("type"))), (e["code"] == std::string("invalid_enum_value")));
            }
            );
            if (AND((typeError), (in(std::string("received"), typeError)))) {
                auto enumError = as<std::shared_ptr<z::ZodInvalidEnumValueIssue>>(typeError);
                throw any(std::make_shared<Error>(std::string("Invalid type '") + enumError->received + std::string("'. Expected: ") + enumError->options->join(std::string(", ")) + string_empty));
            }
        }
        throw any(error);
    }
};


object validateProjectName(string name)
{
    if (name == std::string(".")) {
        return object{
            object::pair{std::string("isValid"), true}
        };
    }
    try
    {
        ProjectNameSchema->parse(name);
        return object{
            object::pair{std::string("isValid"), true}
        };
    }
    catch (const any& error)
    {
        if (is<z->ZodError>(error)) {
            return object{
                object::pair{std::string("isValid"), false}, 
                object::pair{std::string("error"), const_(error["errors"])[0]["message"]}
            };
        }
        return object{
            object::pair{std::string("isValid"), false}, 
            object::pair{std::string("error"), std::string("Invalid project name")}
        };
    }
};


object processPluginName(string name)
{
    try
    {
        auto processedName = name->replace((new RegExp(std::string("^(eliza-?|elizaos-?|plugin-?)"))), string_empty)->replace((new RegExp(std::string("(-?plugin|-?eliza|-?elizaos)$"))), string_empty)->toLowerCase()->replace((new RegExp(std::string("[^a-z0-9-_]"))), std::string("-"))->replace((new RegExp(std::string("-+"))), std::string("-"))->replace((new RegExp(std::string("^-+|-+$"))), string_empty);
        if (!processedName) {
            return object{
                object::pair{std::string("isValid"), false}, 
                object::pair{std::string("error"), std::string("Plugin name cannot be empty after processing")}
            };
        }
        PluginNameSchema->parse(processedName);
        return object{
            object::pair{std::string("isValid"), true}, 
            object::pair{std::string("processedName"), std::string("processedName")}
        };
    }
    catch (const any& error)
    {
        if (is<z->ZodError>(error)) {
            return object{
                object::pair{std::string("isValid"), false}, 
                object::pair{std::string("error"), const_(error["errors"])[0]["message"]}
            };
        }
        return object{
            object::pair{std::string("isValid"), false}, 
            object::pair{std::string("error"), std::string("Invalid plugin name")}
        };
    }
};


std::shared_ptr<Promise<object>> validateTargetDirectory(string targetDir)
{
    try
    {
        if (!existsSync(targetDir)) {
            return object{
                object::pair{std::string("isValid"), true}
            };
        }
        auto entries = std::async([=]() { fs->readdir(targetDir); });
        if (entries->length > 0) {
            return object{
                object::pair{std::string("isValid"), false}, 
                object::pair{std::string("error"), std::string("Directory ") + targetDir + std::string(" already exists and is not empty. Please choose an empty directory or a new name.")}
            };
        }
        return object{
            object::pair{std::string("isValid"), true}
        };
    }
    catch (const any& error)
    {
        return object{
            object::pair{std::string("isValid"), false}, 
            object::pair{std::string("error"), std::string("Failed to validate directory: ") + (is<Error>(error)) ? any(error->message) (std::string("Unknown error")) + string_empty}
        };
    }
};


any ProjectNameSchema = z->string()->min(1, std::string("Invalid project name: cannot be empty"))->regex((new RegExp(std::string("^[a-z0-9-_]+"))), std::string("Invalid project name: must contain only lowercase letters, numbers, hyphens, and underscores"))->refine([=](auto name) mutable
{
    return AND((!name->startsWith(std::string("-"))), (!name->endsWith(std::string("-"))));
}
, std::string("Invalid project name: cannot start or end with a hyphen"))->refine([=](auto name) mutable
{
    return AND((!name->startsWith(std::string("_"))), (!name->endsWith(std::string("_"))));
}
, std::string("Invalid project name: cannot start or end with an underscore"));
any PluginNameSchema = z->string()->min(1, std::string("Plugin name cannot be empty"))->regex((new RegExp(std::string("^[a-z0-9-_]+"))), std::string("Plugin name must contain only lowercase letters, numbers, hyphens, and underscores"))->refine([=](auto name) mutable
{
    return AND((!name->startsWith(std::string("-"))), (!name->endsWith(std::string("-"))));
}
, std::string("Plugin name cannot start or end with a hyphen"))->refine([=](auto name) mutable
{
    return AND((!name->startsWith(std::string("_"))), (!name->endsWith(std::string("_"))));
}
, std::string("Plugin name cannot start or end with an underscore"));

void Main(void)
{
}

MAIN
