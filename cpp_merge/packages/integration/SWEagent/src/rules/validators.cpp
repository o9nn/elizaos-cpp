#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/src/rules/validators.h"

PythonValidator::PythonValidator(array<std::shared_ptr<CodingRule>> _rules) {
}

std::shared_ptr<ValidationResult> PythonValidator::validate(string code, string filePath)
{
    auto violations = array<std::shared_ptr<Violation>>();
    auto warnings = array<string>();
    if (!this->hasTypeAnnotations(code)) {
        violations->push(object{
            object::pair{std:("rule"), std:("python-type-annotations")}, 
            object::pair{std:("message"), std:("Missing type annotations in function definitions")}, 
            object::pair{std:("severity"), std:("error")}
        });
    }
    auto osPathMatches = Array->from(code->matchAll((new RegExp(std:("import os\.path|from os import path")))));
    for (auto& match : osPathMatches)
    {
        auto line = this->getLineNumber(code, match->index);
        violations->push(object{
            object::pair{std:("rule"), std:("use-pathlib")}, 
            object::pair{std:("line"), std:("line")}, 
            object::pair{std:("message"), std:("Use pathlib instead of os.path")}, 
            object::pair{std:("severity"), std:("error")}
        });
    }
    auto openMatches = Array->from(code->matchAll((new RegExp(std:("with\s+open\s*\([^)]+\)\s*as")))));
    for (auto& match : openMatches)
    {
        auto line = this->getLineNumber(code, match->index);
        if (!code->slice(Math->max(0, match->index - 100), match->index)->includes(std:("Path"))) {
            violations->push(object{
                object::pair{std:("rule"), std:("use-pathlib")}, 
                object::pair{std:("line"), std:("line")}, 
                object::pair{std:("message"), std:("Use Path.read_text() or Path.write_text() instead of open()")}, 
                object::pair{std:("severity"), std:("error")}
            });
        }
    }
    if (AND((AND((filePath), (this->isMainScript(filePath)))), (!code->includes(std:("argparse"))))) {
        warnings->push(std:("Consider using argparse for command-line interfaces"));
    }
    auto commentDensity = this->calculateCommentDensity(code);
    if (commentDensity > 0.3) {
        warnings->push(std:("High comment density (") + (commentDensity * 100)->toFixed(1) + std:("%). Keep comments minimal and focused on complex logic."));
    }
    return object{
        object::pair{std:("valid"), violations->filter([=](auto v) mutable
        {
            return v->severity == std:("error");
        }
        )->get_length() == 0}, 
        object::pair{std:("file"), filePath}, 
        object::pair{std:("violations"), std:("violations")}, 
        object::pair{std:("warnings"), std:("warnings")}
    };
}

boolean PythonValidator::hasTypeAnnotations(string code)
{
    auto funcPattern = (new RegExp(std:("def\s+\w+\s*\([^)]*\)\s*(?:->.*?)?:")));
    auto funcs = OR((code->match(funcPattern)), (array<any>()));
    if (funcs->length == 0) {
        return true;
    }
    auto typedFuncs = funcs->filter([=](auto f) mutable
    {
        return f->includes(std:("->"));
    }
    );
    return typedFuncs->get_length() > funcs->length * 0.8;
}

double PythonValidator::getLineNumber(string code, double index)
{
    return code->slice(0, index)->split(std:("\
"))->get_length();
}

boolean PythonValidator::isMainScript(string filePath)
{
    auto filename = path->basename(filePath);
    return OR((OR((filename->startsWith(std:("run_"))), (filename->includes(std:("main"))))), (filename->includes(std:("cli"))));
}

double PythonValidator::calculateCommentDensity(string code)
{
    auto lines = code->split(std:("\
"));
    auto commentLines = lines->filter([=](auto line) mutable
    {
        auto trimmed = line->trim();
        return OR((OR((trimmed->startsWith(std:("#"))), (trimmed->startsWith(std:("""""))))), (trimmed->startsWith(std:("'''"))));
    }
    );
    return (lines->get_length() > 0) ? any(commentLines->get_length() / lines->get_length()) (0);
}

TypeScriptValidator::TypeScriptValidator(array<std::shared_ptr<CodingRule>> _rules) {
}

std::shared_ptr<ValidationResult> TypeScriptValidator::validate(string code, string filePath)
{
    auto violations = array<std::shared_ptr<Violation>>();
    auto warnings = array<string>();
    auto anyMatches = Array->from(code->matchAll((new RegExp(std:(":\s*any(?:\s|$|[,\)])")))));
    for (auto& match : anyMatches)
    {
        auto line = this->getLineNumber(code, match->index);
        violations->push(object{
            object::pair{std:("rule"), std:("explicit-types")}, 
            object::pair{std:("line"), std:("line")}, 
            object::pair{std:("message"), std:("Avoid using any type, use explicit types instead")}, 
            object::pair{std:("severity"), std:("error")}
        });
    }
    auto funcPattern = (new RegExp(std:("(?:function\s+\w+|(?:const|let|var)\s+\w+\s*=\s*(?:async\s+)?(?:\([^)]*\)|[^=]+)\s*=>)[^{]*{")));
    auto funcs = OR((code->match(funcPattern)), (array<any>()));
    for (auto& func : funcs)
    {
        if (AND((!func->includes(std:(":"))), (!func->includes(std:("void"))))) {
            warnings->push(std:("Consider adding explicit return types to functions"));
            break;
        }
    }
    if (OR((code->includes(std:("fs.readFileSync"))), (code->includes(std:("fs.writeFileSync"))))) {
        violations->push(object{
            object::pair{std:("rule"), std:("node-fs-promises")}, 
            object::pair{std:("message"), std:("Use fs.promises API instead of synchronous fs methods")}, 
            object::pair{std:("severity"), std:("warning")}
        });
    }
    auto exportPattern = (new RegExp(std:("export\s+(?:async\s+)?function\s+(\w+)")));
    auto exports = Array->from(code->matchAll(exportPattern));
    for (auto& match : exports)
    {
        auto funcName = const_(match)[1];
        auto funcIndex = match->index;
        auto beforeFunc = code->slice(Math->max(0, funcIndex - 200), funcIndex);
        if (!beforeFunc->includes(std:("/**"))) {
            warnings->push(std:("Consider adding JSDoc comments for exported function: ") + funcName + string_empty);
        }
    }
    auto commentDensity = this->calculateCommentDensity(code);
    if (commentDensity > 0.25) {
        warnings->push(std:("High comment density (") + (commentDensity * 100)->toFixed(1) + std:("%). Keep comments minimal and focused on complex logic."));
    }
    return object{
        object::pair{std:("valid"), violations->filter([=](auto v) mutable
        {
            return v->severity == std:("error");
        }
        )->get_length() == 0}, 
        object::pair{std:("file"), filePath}, 
        object::pair{std:("violations"), std:("violations")}, 
        object::pair{std:("warnings"), std:("warnings")}
    };
}

double TypeScriptValidator::getLineNumber(string code, double index)
{
    return code->slice(0, index)->split(std:("\
"))->get_length();
}

double TypeScriptValidator::calculateCommentDensity(string code)
{
    auto lines = code->split(std:("\
"));
    auto commentLines = lines->filter([=](auto line) mutable
    {
        auto trimmed = line->trim();
        return OR((OR((trimmed->startsWith(std:("//"))), (trimmed->startsWith(std:("/*"))))), (trimmed->startsWith(std:("*"))));
    }
    );
    return (lines->get_length() > 0) ? any(commentLines->get_length() / lines->get_length()) (0);
}

std::shared_ptr<Promise<std::shared_ptr<ValidationResult>>> validateFile(string filePath)
{
    auto ext = path->extname(filePath);
    auto content = std::async([=]() { fs::promises::readFile(filePath, std:("utf-8")); });
    if (ext == std:(".py")) {
        auto validator = std::make_shared<PythonValidator>();
        return validator->validate(content, filePath);
    } else if (OR((ext == std:(".ts")), (ext == std:(".tsx")))) {
        auto validator = std::make_shared<TypeScriptValidator>();
        return validator->validate(content, filePath);
    } else {
        return object{
            object::pair{std:("valid"), true}, 
            object::pair{std:("file"), filePath}, 
            object::pair{std:("violations"), array<any>()}, 
            object::pair{std:("warnings"), array<string>{ std:("Unsupported file type: ") + ext + string_empty }}
        };
    }
};


std::shared_ptr<Promise<array<std::shared_ptr<ValidationResult>>>> validateFiles(array<string> filePaths)
{
    return Promise->all(filePaths->map(validateFile));
};


string formatValidationResults(array<std::shared_ptr<ValidationResult>> results)
{
    auto output = array<string>();
    auto& __array8040_8598 = results;
    for (auto __indx8040_8598 = 0_N; __indx8040_8598 < __array8040_8598->get_length(); __indx8040_8598++)
    {
        auto& result = const_(__array8040_8598)[__indx8040_8598];
        {
            if (AND((result->violations->get_length() == 0), (result->warnings->get_length() == 0))) {
                continue;
            }
            output->push(std:("\
") + (OR((result->file), (std:("Unknown file")))) + std:(":"));
            for (auto& violation : result->violations)
            {
                auto location = (violation->line) ? any(std:(":") + violation->line + string_empty) (string_empty);
                auto severity = violation->severity->toUpperCase();
                output->push(std:("  [") + severity + string_empty + location + std:("] ") + violation->rule + std:(": ") + violation->message + string_empty);
            }
            for (auto& warning : result->warnings)
            {
                output->push(std:("  [WARNING] ") + warning + string_empty);
            }
        }
    }
    if (output->get_length() == 0) {
        return std:("All files passed validation!");
    }
    return output->join(std:("\
"));
};


