#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SWEAGENT_SRC_TOOLS_COMMANDS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SWEAGENT_SRC_TOOLS_COMMANDS_H
#include "core.hpp"

class IArgument;
class ArgumentImpl;
class Command;

std::shared_ptr<Set<string>> extractKeys(string formatString);

class IArgument : public object, public std::enable_shared_from_this<IArgument> {
public:
    using std::enable_shared_from_this<IArgument>::shared_from_this;
    string name;

    string type;

    any items;

    string description;

    boolean required;

    any enum;

    string argumentFormat;
};

class ArgumentImpl : public IArgument, public std::enable_shared_from_this<ArgumentImpl> {
public:
    using std::enable_shared_from_this<ArgumentImpl>::shared_from_this;
    string name;

    string type;

    any items;

    string description;

    boolean required;

    any enum;

    string argumentFormat;

    ArgumentImpl(object config);
    virtual void validateArgumentFormat();
};

class Command : public object, public std::enable_shared_from_this<Command> {
public:
    using std::enable_shared_from_this<Command>::shared_from_this;
    string name;

    any docstring;

    any signature;

    string endName;

    array<std::shared_ptr<IArgument>> arguments;

    Command(object config);
    virtual string get_invokeFormat();
    virtual Record<string, any> getFunctionCallingTool();
    virtual void validateArguments();
};

#endif
