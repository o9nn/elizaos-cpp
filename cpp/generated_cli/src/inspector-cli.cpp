#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/src/run/inspector-cli.h"

TrajectoryInspector::TrajectoryInspector(string trajPath, string dataPath) {
    auto content = fs::readFileSync(trajPath, std::string("utf-8"));
    if (OR((trajPath->endsWith(std::string(".yaml"))), (trajPath->endsWith(std::string(".yml"))))) {
        this->trajData = as<std::shared_ptr<TrajectoryData>>(yaml->load(content));
    } else {
        this->trajData = JSON->parse(content);
    }
    if (dataPath) {
        auto data = as<any>(loadFile(dataPath));
        auto instanceId = path->basename(path->dirname(trajPath));
        this->goldPatch = (*const_(data))[instanceId]["patch"];
    }
}

double TrajectoryInspector::get_nSteps()
{
    return OR((this->trajData->trajectory->get_length()), (0));
}

void TrajectoryInspector::showStep()
{
    console->clear();
    if (OR((this->currentStep < 0), (this->currentStep >= this->get_nSteps()))) {
        this->showInfo();
        return;
    }
    auto step = const_(this->trajData->trajectory)[this->currentStep];
    console->log(std::string("=== Step ") + (this->currentStep + 1) + std::string("/") + this->get_nSteps() + std::string(" ===\
"));
    if (this->showFull) {
        console->log(yaml->dump(step, object{
            object::pair{std::string("lineWidth"), -1}
        }));
    } else {
        if (step->thought) {
            console->log(std::string("THOUGHT:"));
            console->log(step->thought);
            console->log();
        }
        if (step->action) {
            console->log(std::string("ACTION:"));
            console->log(step->action);
            console->log();
        }
        if (step->observation) {
            console->log(std::string("OBSERVATION:"));
            console->log(step->observation);
        }
    }
    this->showHelp();
}

void TrajectoryInspector::showInfo()
{
    console->clear();
    console->log(std::string("=== Trajectory Info ===\
"));
    auto info = utils::assign(object{
    }, this->trajData->info);
    if (this->goldPatch) {
        info["gold_patch"] = this->goldPatch;
    }
    console->log(yaml->dump(info, object{
        object::pair{std::string("lineWidth"), -1}
    }));
    this->showHelp();
}

void TrajectoryInspector::showHelp()
{
    console->log(std::string("\
") + std::string("=")->repeat(80));
    console->log(std::string("Commands:"));
    console->log(std::string("  l/right  - Next step"));
    console->log(std::string("  h/left   - Previous step"));
    console->log(std::string("  0        - Go to first step"));
    console->log(std::string("  $        - Go to last step"));
    console->log(std::string("  v        - Toggle view (simple/full)"));
    console->log(std::string("  i        - Show info"));
    console->log(std::string("  q        - Quit"));
    console->log(std::string("=")->repeat(80));
}

std::shared_ptr<Promise<void>> TrajectoryInspector::run()
{
    shared rl = readline::createInterface(object{
        object::pair{std::string("input"), process->stdin}, 
        object::pair{std::string("output"), process->stdout}
    });
    if (process->stdin->isTTY) {
        process->stdin->setRawMode(true);
    }
    readline::emitKeypressEvents(process->stdin, rl);
    this->showStep();
    process->stdin->on(std::string("keypress"), [=](auto _str, auto key) mutable
    {
        if (!key) {
            return;
        }
        static switch_type __switch3929_4918 = {
            { any(std::string("q")), 1 },
            { any(std::string("escape")), 2 },
            { any(std::string("l")), 3 },
            { any(std::string("right")), 4 },
            { any(std::string("h")), 5 },
            { any(std::string("left")), 6 },
            { any(std::string("0")), 7 },
            { any(std::string("$")), 8 },
            { any(std::string("v")), 9 },
            { any(std::string("i")), 10 }
        };
        switch (__switch3929_4918[key["name"]])
        {
        case 1:
        case 2:
            rl->close();
            if (process->stdin->isTTY) {
                process->stdin->setRawMode(false);
            }
            process->exit(0);
            break;
        case 3:
        case 4:
            if (this->currentStep < this->get_nSteps()) {
                this->currentStep++;
                this->showStep();
            }
            break;
        case 5:
        case 6:
            if (this->currentStep > -1) {
                this->currentStep--;
                this->showStep();
            }
            break;
        case 7:
            this->currentStep = 0;
            this->showStep();
            break;
        case 8:
            this->currentStep = this->get_nSteps() - 1;
            this->showStep();
            break;
        case 9:
            this->showFull = !this->showFull;
            this->showStep();
            break;
        case 10:
            this->currentStep = -1;
            this->showStep();
            break;
        }
    }
    );
    std::async([=]() { std::make_shared<Promise>([=]() mutable
    {
    }
    ); });
    return std::shared_ptr<Promise<void>>();
}

array<string> findTrajFiles(string dir)
{
    shared results = array<string>();
    auto walk = [=](auto directory) mutable
    {
        auto files = fs::readdirSync(directory);
        for (auto& file : files)
        {
            auto filePath = path->join(directory, file);
            auto stat = fs::statSync(filePath);
            if (stat->isDirectory()) {
                walk(filePath);
            } else if (file->endsWith(std::string(".traj"))) {
                results->push(filePath);
            }
        }
    };

    if (fs::statSync(dir)->isDirectory()) {
        walk(dir);
    } else {
        results->push(dir);
    }
    return results;
};


std::shared_ptr<Promise<void>> inspectorCli(string trajectoryPath, string dataPath)
{
    auto trajPath = path->resolve(trajectoryPath);
    if (!fs::existsSync(trajPath)) {
        throw any(std::make_shared<Error>(std::string("Path does not exist: ") + trajPath + string_empty));
    }
    auto trajFiles = findTrajFiles(trajPath);
    if (trajFiles->get_length() == 0) {
        throw any(std::make_shared<Error>(std::string("No trajectory files found")));
    }
    if (trajFiles->get_length() == 1) {
        auto inspector = std::make_shared<TrajectoryInspector>(const_(trajFiles)[0], dataPath);
        std::async([=]() { inspector->run(); });
    } else {
        console->log(std::string("Multiple trajectory files found:"));
        trajFiles->forEach([=](auto file, auto index) mutable
        {
            console->log(std::string("  ") + (index + 1) + std::string(". ") + file + string_empty);
        }
        );
        shared rl = readline::createInterface(object{
            object::pair{std::string("input"), process->stdin}, 
            object::pair{std::string("output"), process->stdout}
        });
        auto answer = std::async([=]() { std::make_shared<Promise<string>>([=](auto resolve) mutable
        {
            rl->question(std::string("\
Select file number (or q to quit): "), resolve);
        }
        ); });
        rl->close();
        if (answer == std::string("q")) {
            process->exit(0);
        }
        auto index = parseInt(answer, 10) - 1;
        if (AND((index >= 0), (index < trajFiles->get_length()))) {
            auto inspector = std::make_shared<TrajectoryInspector>(const_(trajFiles)[index], dataPath);
            std::async([=]() { inspector->run(); });
        } else {
            console->log(std::string("Invalid selection"));
            process->exit(1);
        }
    }
    return std::shared_ptr<Promise<void>>();
};


