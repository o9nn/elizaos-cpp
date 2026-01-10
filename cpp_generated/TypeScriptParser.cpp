#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/autodoc/src/TypeScriptParser.h"

any TypeScriptParser::parse(string file)
{
    try
    {
        auto content = fs->readFileSync(file, std::string("utf-8"));
        auto parserOptions = object{
            object::pair{std::string("sourceType"), std::string("module")}, 
            object::pair{std::string("ecmaVersion"), 2020}, 
            object::pair{std::string("ecmaFeatures"), object{
                object::pair{std::string("jsx"), true}
            }}, 
            object::pair{std::string("range"), true}, 
            object::pair{std::string("loc"), true}, 
            object::pair{std::string("tokens"), true}, 
            object::pair{std::string("comment"), true}, 
            object::pair{std::string("errorOnUnknownASTType"), false}, 
            object::pair{std::string("errorOnTypeScriptSyntacticAndSemanticIssues"), false}
        };
        auto ast = parse(content, parserOptions);
        if (OR((!ast), (type_of(ast) != std::string("object")))) {
            console->warn(std::string("Warning: Invalid AST generated for file ") + file + string_empty);
            return nullptr;
        }
        return ast;
    }
    catch (const any& error)
    {
        if (is<Error>(error)) {
            this->handleParseError(error);
        } else {
            console->error(std::string("Unknown error:"), error);
        }
        return nullptr;
    }
}

object TypeScriptParser::extractExports(string file)
{
    auto ast = this->parse(file);
    shared exports = object{
        object::pair{std::string("actions"), array<any>()}, 
        object::pair{std::string("providers"), array<any>()}, 
        object::pair{std::string("evaluators"), array<any>()}
    };
    if (ast) {
        ast["body"]["forEach"]([=](auto node) mutable
        {
            if (node["type"] == std::string("ImportDeclaration")) {
                auto source = node["source"]["value"];
                if (source["startsWith"](std::string("./actions/"))) {
                    exports["actions"]->push(source);
                } else if (source["startsWith"](std::string("./providers/"))) {
                    exports["providers"]->push(source);
                } else if (source["startsWith"](std::string("./evaluators/"))) {
                    exports["evaluators"]->push(source);
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
        if (node["typeAnnotation"]["typeAnnotation"]["typeName"]["name"] == std::string("Action")) {
            startLine = node["loc"]["start"]["line"];
        }
        if (node["typeAnnotation"]["elementType"]["elementType"]["typeName"]["name"] == std::string("ActionExample")) {
            endLine = node["loc"]["end"]["line"];
        }
        if (AND((AND((AND((node["type"] == std::string("Property")), (node["key"]["type"] == std::string("Identifier")))), (node["key"]["name"] == std::string("name")))), (node["value"]["type"] == std::string("Literal")))) {
            actionNameStartLine = node["loc"]["start"]["line"];
        }
        for (auto& key : keys_(node))
        {
            if (AND((const_(node)[key]), (type_of(const_(node)[key]) == std::string("object")))) {
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
        console->log(std::string("Using action name line as fallback"));
        startLine = actionNameStartLine;
    }
    if (AND((startLine), (endLine))) {
        return object{
            object::pair{std::string("startLine"), std::string("startLine")}, 
            object::pair{std::string("endLine"), std::string("endLine")}
        };
    }
    return nullptr;
}

string TypeScriptParser::extractActionCode(string filePath, std::shared_ptr<ActionBounds> bounds)
{
    auto fileContent = fs->readFileSync(filePath, std::string("utf-8"));
    auto lines = fileContent->split(std::string("\
"));
    return lines->slice(bounds->startLine - 1, bounds->endLine)->join(std::string("\
"));
}

void TypeScriptParser::handleParseError(std::shared_ptr<Error> error)
{
    console->error(std::string("Error parsing TypeScript file:"), error->message);
}

