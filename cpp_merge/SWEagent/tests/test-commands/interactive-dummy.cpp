#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/tests/test-commands/interactive-dummy.h"

string InteractiveDummyCommand::PROMPT = std:("(dummy) ");

InteractiveDummyCommand::InteractiveDummyCommand() {
    this->rl = readline::createInterface(object{
        object::pair{std:("input"), process->stdin}, 
        object::pair{std:("output"), process->stdout}, 
        object::pair{std:("prompt"), InteractiveDummyCommand::PROMPT}
    });
}

void InteractiveDummyCommand::start()
{
    console->log(std:("Started interactive dummy command"));
}

void InteractiveDummyCommand::send(string input)
{
    console->log(std:("Received input: ") + input + string_empty);
    setTimeout([=]() mutable
    {
    }
    , 500);
}

void InteractiveDummyCommand::stop()
{
    console->log(std:("Stopped interactive dummy command"));
    this->rl->close();
}

void InteractiveDummyCommand::run()
{
    this->start();
    this->rl->prompt();
    this->rl->on(std:("line"), [=](auto line) mutable
    {
        auto [cmd, args] = line->trim()->split(std:(" "));
        static switch_type __switch937_1251 = {
            { any(std:("stop")), 1 },
            { any(std:("send")), 2 }
        };
        switch (__switch937_1251[cmd])
        {
        case 1:
            this->stop();
            process->exit(0);
            break;
        case 2:
            this->send(args->join(std:(" ")));
            break;
        default:
            if (cmd) {
                console->log(std:("Unknown command: ") + cmd + string_empty);
            }
            break;
        }
        this->rl->prompt();
    }
    );
    this->rl->on(std:("close"), [=]() mutable
    {
        process->exit(0);
    }
    );
}


void Main(void)
{
    if (require->main == module) {
        std::shared_ptr<InteractiveDummyCommand> command = std::make_shared<InteractiveDummyCommand>();
        command->run();
    }
}

MAIN
