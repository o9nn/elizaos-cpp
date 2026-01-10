#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/src/rules/validators.h"

PythonValidator::PythonValidator(array<std::shared_ptr<CodingRule>> _rules) {
}

std::shared_ptr<ValidationResult> PythonValidator::validate(string code, string filePath)
{
    auto violations = array<std::shared_ptr<Violation>>();
    auto warnings = array<string>();
    if (!this->hasTypeAnnotations(code)) {
        violations->push(object{
            object::pair{std::string("rule"), std::string("python-type-annotations")}, 
            object::pair{std::string("message"), std::string("Missing type annotations in function definitions")}, 
            object::pair{std::string("severity"), std::string("error")}
        });
    }
    auto osPathMatches = Array->from(code->matchAll((new RegExp(std::string("import os\.path|from os import path")))));
    for (auto& match : osPathMatches)
    {
        auto line = this->getLineNumber(code, match->index);
        violations->push(object{
            object::pair{std::string("rule"), std::string("use-pathlib")}, 
            object::pair{std::string("line"), std::string("line")}, 
            object::pair{std::string("message"), std::string("Use pathlib instead of os.path")}, 
            object::pair{std::string("severity"), std::string("error")}
        });
    }
    auto openMatches = Array->from(code->matchAll((new RegExp(std::string("with\s+open\s*\([^)]+\)\s*as")))));
    for (auto& match : openMatches)
    {
        auto line = this->getLineNumber(code, match->index);
        if (!code->slice(Math->max(0, match->index - 100), match->index)->includes(std::string("Path"))) {
            violations->push(object{
                object::pair{std::string("rule"), std::string("use-pathlib")}, 
                object::pair{std::string("line"), std::string("line")}, 
                object::pair{std::string("message"), std::string("Use Path.read_text() or Path.write_text() instead of open()")}, 
                object::pair{std::string("severity"), std::string("error")}
            });
        }
    }
    if (AND((AND((filePath), (this->isMainScript(filePath)))), (!code->includes(std::string("argparse"))))) {
        warnings->push(std::string("Consider using argparse for command-line interfaces"));
    }
    auto commentDensity = this->calculateCommentDensity(code);
    if (commentDensity > 0.3) {
        warnings->push(std::string("High comment density (") + (commentDensity * 100)->toFixed(1) + std::string("%). Keep comments minimal and focused on complex logic."));
    }
    return object{
        object::pair{std::string("valid"), violations->filter([=](auto v) mutable
        {
            return v->severity == std::string("error");
        }
        )->get_length() == 0}, 
        object::pair{std::string("file"), filePath}, 
        object::pair{std::string("violations"), std::string("violations")}, 
        object::pair{std::string("warnings"), std::string("warnings")}
    };
}

boolean PythonValidator::hasTypeAnnotations(string code)
{
    auto funcPattern = (new RegExp(std::string("def\s+\w+\s*\([^)]*\)\s*(?:->.*?)?:")));
    auto funcs = OR((code->match(funcPattern)), (array<any>()));
    if (funcs->length == 0) {
        return true;
    }
    auto typedFuncs = funcs->filter([=](auto f) mutable
    {
        return f->includes(std::string("->"));
    }
    );
    return typedFuncs->get_length() > funcs->length * 0.8;
}

double PythonValidator::getLineNumber(string code, double index)
{
    return code->slice(0, index)->split(std::string("\
"))->get_length();
}

boolean PythonValidator::isMainScript(string filePath)
{
    auto filename = path->basename(filePath);
    return OR((OR((filename->startsWith(std::string("run_"))), (filename->includes(std::string("main"))))), (filename->includes(std::string("cli"))));
}

double PythonValidator::calculateCommentDensity(string code)
{
    auto lines = code->split(std::string("\
"));
    auto commentLines = lines->filter([=](auto line) mutable
    {
        auto trimmed = line->trim();
        return OR((OR((trimmed->startsWith(std::string("#"))), (trimmed->startsWith(std::string("""""))))), (trimmed->startsWith(std::string("'''"))));
    }
    );
    return (lines->get_length() > 0) ? any(commentLines->get_length() / lines->get_length()) : any(0);
}

TypeScriptValidator::TypeScriptValidator(array<std::shared_ptr<CodingRule>> _rules) {
}

std::shared_ptr<ValidationResult> TypeScriptValidator::validate(string code, string filePath)
{
    auto violations = array<std::shared_ptr<Violation>>();
    auto warnings = array<string>();
    auto anyMatches = Array->from(code->matchAll((new RegExp(std::string(":\s*any(?:\s|$|[,\)])")))));
    for (auto& match : anyMatches)
    {
        auto line = this->getLineNumber(code, match->index);
        violations->push(object{
            object::pair{std::string("rule"), std::string("explicit-types")}, 
            object::pair{std::string("line"), std::string("line")}, 
            object::pair{std::string("message"), std::string("Avoid using any type, use explicit types instead")}, 
            object::pair{std::string("severity"), std::string("error")}
        });
    }
    auto funcPattern = (new RegExp(std::string("(?:function\s+\w+|(?:const|let|var)\s+\w+\s*=\s*(?:async\s+)?(?:\([^)]*\)|[^=]+)\s*=>)[^{]*{")));
    auto funcs = OR((code->match(funcPattern)), (array<any>()));
    for (auto& func : funcs)
    {
        if (AND((!func->includes(std::string(":"))), (!func->includes(std::string("void"))))) {
            warnings->push(std::string("Consider adding explicit return types to functions"));
            break;
        }
    }
    if (OR((code->includes(std::string("fs.readFileSync"))), (code->includes(std::string("fs.writeFileSync"))))) {
        violations->push(object{
            object::pair{std::string("rule"), std::string("node-fs-promises")}, 
            object::pair{std::string("message"), std::string("Use fs.promises API instead of synchronous fs methods")}, 
            object::pair{std::string("severity"), std::string("warning")}
        });
    }
    auto exportPattern = (new RegExp(std::string("export\s+(?:async\s+)?function\s+(\w+)")));
    auto exports = Array->from(code->matchAll(exportPattern));
    for (auto& match : exports)
    {
        auto funcName = const_(match)[1];
        auto funcIndex = match->index;
        auto beforeFunc = code->slice(Math->max(0, funcIndex - 200), funcIndex);
        if (!beforeFunc->includes(std::string("/**"))) {
            warnings->push(std::string("Consider adding JSDoc comments for exported function: ") + funcName + string_empty);
        }
    }
    auto commentDensity = this->calculateCommentDensity(code);
    if (commentDensity > 0.25) {
        warnings->push(std::string("High comment density (") + (commentDensity * 100)->toFixed(1) + std::string("%). Keep comments minimal and focused on complex logic."));
    }
    return object{
        object::pair{std::string("valid"), violations->filter([=](auto v) mutable
        {
            return v->severity == std::string("error");
        }
        )->get_length() == 0}, 
        object::pair{std::string("file"), filePath}, 
        object::pair{std::string("violations"), std::string("violations")}, 
        object::pair{std::string("warnings"), std::string("warnings")}
    };
}

double TypeScriptValidator::getLineNumber(string code, double index)
{
    return code->slice(0, index)->split(std::string("\
"))->get_length();
}

double TypeScriptValidator::calculateCommentDensity(string code)
{
    auto lines = code->split(std::string("\
"));
    auto commentLines = lines->filter([=](auto line) mutable
    {
        auto trimmed = line->trim();
        return OR((OR((trimmed->startsWith(std::string("//"))), (trimmed->startsWith(std::string("/*"))))), (trimmed->startsWith(std::string("*"))));
    }
    );
    return (lines->get_length() > 0) ? any(commentLines->get_length() / lines->get_length()) : any(0);
}

std::shared_ptr<Promise<std::shared_ptr<ValidationResult>>> validateFile(string filePath)
{
    auto ext = path->extname(filePath);
    auto content = std::async([=]() { fs::promises::readFile(filePath, std::string("utf-8")); });
    if (ext == std::string(".py")) {
        auto validator = std::make_shared<PythonValidator>();
        return validator->validate(content, filePath);
    } else if (OR((ext == std::string(".ts")), (ext == std::string(".tsx")))) {
        auto validator = std::make_shared<TypeScriptValidator>();
        return validator->validate(content, filePath);
    } else {
        return object{
            object::pair{std::string("valid"), true}, 
            object::pair{std::string("file"), filePath}, 
            object::pair{std::string("violations"), array<any>()}, 
            object::pair{std::string("warnings"), array<string>{ std::string("Unsupported file type: ") + ext + string_empty }}
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
            output->push(std::string("\
") + (OR((result->file), (std::string("Unknown file")))) + std::string(":"));
            for (auto& violation : result->violations)
            {
                auto location = (violation->line) ? any(std::string(":") + violation->line + string_empty) : any(string_empty);
                auto severity = violation->severity->toUpperCase();
                output->push(std::string("  [") + severity + string_empty + location + std::string("] ") + violation->rule + std::string(": ") + violation->message + string_empty);
            }
            for (auto& warning : result->warnings)
            {
                output->push(std::string("  [WARNING] ") + warning + string_empty);
            }
        }
    }
    if (output->get_length() == 0) {
        return std::string("All files passed validation!");
    }
    return output->join(std::string("\
"));
};


