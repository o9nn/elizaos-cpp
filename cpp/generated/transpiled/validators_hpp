#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_SRC_RULES_VALIDATORS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_SRC_RULES_VALIDATORS_H
#include "core.h"
#include "fs.h"
#include "path.h"
#include "./types.h"
#include "./general.h"

class ValidationResult;
class Violation;
class PythonValidator;
class TypeScriptValidator;

class ValidationResult : public object, public std::enable_shared_from_this<ValidationResult> {
public:
    using std::enable_shared_from_this<ValidationResult>::shared_from_this;
    boolean valid;

    string file;

    array<std::shared_ptr<Violation>> violations;

    array<string> warnings;
};

class Violation : public object, public std::enable_shared_from_this<Violation> {
public:
    using std::enable_shared_from_this<Violation>::shared_from_this;
    string rule;

    double line;

    double column;

    string message;

    any severity;
};

class PythonValidator : public object, public std::enable_shared_from_this<PythonValidator> {
public:
    using std::enable_shared_from_this<PythonValidator>::shared_from_this;
    PythonValidator(array<std::shared_ptr<CodingRule>> _rules = PYTHON_CODING_RULES);
    virtual std::shared_ptr<ValidationResult> validate(string code, string filePath = undefined);
    virtual boolean hasTypeAnnotations(string code);
    virtual double getLineNumber(string code, double index);
    virtual boolean isMainScript(string filePath);
    virtual double calculateCommentDensity(string code);
};

class TypeScriptValidator : public object, public std::enable_shared_from_this<TypeScriptValidator> {
public:
    using std::enable_shared_from_this<TypeScriptValidator>::shared_from_this;
    TypeScriptValidator(array<std::shared_ptr<CodingRule>> _rules = TYPESCRIPT_CODING_RULES);
    virtual std::shared_ptr<ValidationResult> validate(string code, string filePath = undefined);
    virtual double getLineNumber(string code, double index);
    virtual double calculateCommentDensity(string code);
};

template <typename RET, typename P0>
RET getValidator(P0 language);

std::shared_ptr<Promise<std::shared_ptr<ValidationResult>>> validateFile(string filePath);

std::shared_ptr<Promise<array<std::shared_ptr<ValidationResult>>>> validateFiles(array<string> filePaths);

string formatValidationResults(array<std::shared_ptr<ValidationResult>> results);

template <typename RET, typename P0>
RET getValidator(P0 language)
{
    return (language == std::string("python")) ? std::make_shared<PythonValidator>() : std::make_shared<TypeScriptValidator>();
};


#endif
