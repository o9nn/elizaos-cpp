#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/tests/test-commands/interactive-dummy.h"

string InteractiveDummyCommand::PROMPT = std::string("(dummy) ");

InteractiveDummyCommand::InteractiveDummyCommand() {
    this->rl = readline::createInterface(object{
        object::pair{std::string("input"), process->stdin}, 
        object::pair{std::string("output"), process->stdout}, 
        object::pair{std::string("prompt"), InteractiveDummyCommand::PROMPT}
    });
}

void InteractiveDummyCommand::start()
{
    console->log(std::string("Started interactive dummy command"));
}

void InteractiveDummyCommand::send(string input)
{
    console->log(std::string("Received input: ") + input + string_empty);
    setTimeout([=]() mutable
    {
    }
    , 500);
}

void InteractiveDummyCommand::stop()
{
    console->log(std::string("Stopped interactive dummy command"));
    this->rl->close();
}

void InteractiveDummyCommand::run()
{
    this->start();
    this->rl->prompt();
    this->rl->on(std::string("line"), [=](auto line) mutable
    {
        auto [cmd, args] = line->trim()->split(std::string(" "));
        static switch_type __switch937_1251 = {
            { any(std::string("stop")), 1 },
            { any(std::string("send")), 2 }
        };
        switch (__switch937_1251[cmd])
        {
        case 1:
            this->stop();
            process->exit(0);
            break;
        case 2:
            this->send(args->join(std::string(" ")));
            break;
        default:
            if (cmd) {
                console->log(std::string("Unknown command: ") + cmd + string_empty);
            }
            break;
        }
        this->rl->prompt();
    }
    );
    this->rl->on(std::string("close"), [=]() mutable
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
