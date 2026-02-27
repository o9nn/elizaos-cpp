#include "TypeScriptParser.hpp"

any TypeScriptParser::parse(string file)
{
    try
    {
        auto content = fs->readFileSync(file, std:("utf-8"));
        auto isTsxFile = OR((file->endsWith(std:(".tsx"))), ((AND((AND((content->includes(std:("<"))), (content->includes(std:(">"))))), (content->includes(std:("React")))))));
        auto parserOptions = object{
            object::pair{std:("sourceType"), std:("module")}, 
            object::pair{std:("ecmaVersion"), std:("latest")}, 
            object::pair{std:("ecmaFeatures"), object{
                object::pair{std:("jsx"), true}, 
                object::pair{std:("globalReturn"), false}
            }}, 
            object::pair{std:("range"), true}, 
            object::pair{std:("loc"), true}, 
            object::pair{std:("tokens"), true}, 
            object::pair{std:("comment"), true}, 
            object::pair{std:("errorOnUnknownASTType"), false}, 
            object::pair{std:("errorOnTypeScriptSyntacticAndSemanticIssues"), false}, 
            object::pair{std:("project"), undefined}, 
            object::pair{std:("extraFileExtensions"), array<string>{ std:(".tsx") }}
        };
        auto ast = parse(content, parserOptions);
        if (OR((!ast), (type_of(ast) != std:("object")))) {
            console->warn(std:("Warning: Invalid AST generated for file ") + file + string_empty);
            return nullptr;
        }
        return ast;
    }
    catch (const any& error)
    {
        if (is<Error>(error)) {
            this->handleParseError(error, file);
        } else {
            console->error(std:("Unknown error:"), error);
        }
        return nullptr;
    }
}

object TypeScriptParser::extractExports(string file)
{
    auto ast = this->parse(file);
    shared exports = object{
        object::pair{std:("actions"), array<any>()}, 
        object::pair{std:("providers"), array<any>()}, 
        object::pair{std:("evaluators"), array<any>()}
    };
    if (AND((ast), (ast["body"]))) {
        ast["body"]["forEach"]([=](auto node) mutable
        {
            if (node["type"] == std:("ImportDeclaration")) {
                auto source = node["source"]["value"];
                if (type_of(source) == std:("string")) {
                    if (source->startsWith(std:("./actions/"))) {
                        exports["actions"]->push(source);
                    } else if (source->startsWith(std:("./providers/"))) {
                        exports["providers"]->push(source);
                    } else if (source->startsWith(std:("./evaluators/"))) {
                        exports["evaluators"]->push(source);
                    }
                }
            }
        }
        );
    }
    return exports;
}

any TypeScriptParser::findActionBounds(any ast)
{
    shared startLine = nullptr;
    shared endLine = nullptr;
    shared actionNameStartLine = nullptr;
    shared findActionTypeAnnotation = [=](auto node) mutable
    {
        if (node["typeAnnotation"]["typeAnnotation"]["typeName"]["name"] == std:("Action")) {
            startLine = node["loc"]["start"]["line"];
        }
        if (node["typeAnnotation"]["elementType"]["elementType"]["typeName"]["name"] == std:("ActionExample")) {
            endLine = node["loc"]["end"]["line"];
        }
        if (AND((AND((AND((node["type"] == std:("Property")), (node["key"]["type"] == std:("Identifier")))), (node["key"]["name"] == std:("name")))), (node["value"]["type"] == std:("Literal")))) {
            actionNameStartLine = node["loc"]["start"]["line"];
        }
        for (auto& key : keys_(node))
        {
            if (AND((const_(node)[key]), (type_of(const_(node)[key]) == std:("object")))) {
                if (Array->isArray(const_(node)[key])) {
                    const_(node)[key]["forEach"](findActionTypeAnnotation);
                } else {
                    findActionTypeAnnotation(const_(node)[key]);
                }
            }
        }
    };
    findActionTypeAnnotation(ast);
    if (AND((AND((!startLine), (actionNameStartLine))), (endLine))) {
        console->log(std:("Using action name line as fallback"));
        startLine = actionNameStartLine;
    }
    if (AND((startLine), (endLine))) {
        return object{
            object::pair{std:("startLine"), std:("startLine")}, 
            object::pair{std:("endLine"), std:("endLine")}
        };
    }
    return nullptr;
}

string TypeScriptParser::extractActionCode(string filePath, std::shared_ptr<ActionBounds> bounds)
{
    auto fileContent = fs->readFileSync(filePath, std:("utf-8"));
    auto lines = fileContent->split(std:("\
"));
    return lines->slice(bounds->startLine - 1, bounds->endLine)->join(std:("\
"));
}

void TypeScriptParser::handleParseError(std::shared_ptr<Error> error, string file)
{
    auto fileInfo = (file) ? any(std:(" in file ") + file + string_empty) (string_empty);
    console->error(std:("Error parsing TypeScript file") + fileInfo + std:(":"), error->message);
    if (error->message->includes(std:("Unexpected token"))) {
        console->warn(std:("Skipping file due to parsing error") + fileInfo + std:(". This might be due to unsupported syntax."));
    }
}

