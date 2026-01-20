#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/game/tests/tauri-integration-runner.h"

std::shared_ptr<Promise<void>> TauriIntegrationRunner::runAll()
{
    console->log(std::string("🚀 Starting Tauri Integration Test Runner"));
    console->log(std::string("=========================================="));
    {
        utils::finally __finally1197_1233([&]() mutable
        {
            std::async([=]() { this->cleanup(); });
        });
        try
        {
            std::async([=]() { this->buildTauriApp(); });
            std::async([=]() { this->startElizaServer(); });
            std::async([=]() { this->startTauriApp(); });
            std::async([=]() { this->runIntegrationTests(); });
            std::async([=]() { this->generateReport(); });
        }
        catch (const any& error)
        {
            console->error(std::string("❌ Test runner failed:"), error);
            process->exit(1);
        }
    }
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> TauriIntegrationRunner::buildTauriApp()
{
    console->log(std::string("📦 Building Tauri application..."));
    return std::make_shared<Promise>([=](auto resolve, auto reject) mutable
    {
        auto buildProcess = spawn(std::string("npm"), array<string>{ std::string("run"), std::string("build:tauri") }, object{
            object::pair{std::string("cwd"), process->cwd()}, 
            object::pair{std::string("stdio"), std::string("pipe")}
        });
        shared output = string_empty;
        buildProcess->stdout->on(std::string("data"), [=](auto data) mutable
        {
            output += data["toString"]();
            process->stdout->write(data);
        }
        );
        buildProcess->stderr->on(std::string("data"), [=](auto data) mutable
        {
            output += data["toString"]();
            process->stderr->write(data);
        }
        );
        buildProcess->on(std::string("close"), [=](auto code) mutable
        {
            if (code == 0) {
                console->log(std::string("✅ Tauri build completed successfully"));
                resolve();
            } else {
                reject(std::make_shared<Error>(std::string("Build failed with exit code ") + code + string_empty));
            }
        }
        );
    }
    );
}

std::shared_ptr<Promise<void>> TauriIntegrationRunner::startElizaServer()
{
    console->log(std::string("🌟 Starting ElizaOS server..."));
    return std::make_shared<Promise>([=](auto resolve, auto reject) mutable
    {
        this->serverProcess = spawn(std::string("elizaos"), array<string>{ std::string("start") }, object{
            object::pair{std::string("cwd"), path->join(process->cwd(), std::string("../.."))}, 
            object::pair{std::string("stdio"), std::string("pipe")}
        });
        shared serverReady = false;
        shared timeout = setTimeout([=]() mutable
        {
            if (!serverReady) {
                reject(std::make_shared<Error>(std::string("Server startup timeout")));
            }
        }
        , 60000);
        this->serverProcess["stdout"]["on"](std::string("data"), [=](auto data) mutable
        {
            auto output = data->toString();
            console->log(std::string("📡 Server:"), output->trim());
            if (OR((OR((output->includes(std::string("Server running"))), (output->includes(std::string("listening on"))))), (output->includes(std::string("started successfully"))))) {
                clearTimeout(timeout);
                if (!serverReady) {
                    serverReady = true;
                    console->log(std::string("✅ ElizaOS server is ready"));
                    setTimeout(resolve, 3000);
                }
            }
        }
        );
        this->serverProcess["stderr"]["on"](std::string("data"), [=](auto data) mutable
        {
            console->error(std::string("🚨 Server error:"), data->toString()->trim());
        }
        );
        this->serverProcess["on"](std::string("close"), [=](auto code) mutable
        {
            console->log(std::string("📡 Server process exited with code ") + code + string_empty);
        }
        );
    }
    );
}

std::shared_ptr<Promise<void>> TauriIntegrationRunner::startTauriApp()
{
    console->log(std::string("🎮 Starting Tauri application..."));
    return std::make_shared<Promise>([=](auto resolve, auto reject) mutable
    {
        auto appPath = path->join(process->cwd(), std::string("src-tauri"), std::string("target"), std::string("debug"), std::string("eliza-game"));
        this->tauriProcess = spawn(appPath, array<any>(), object{
            object::pair{std::string("stdio"), std::string("pipe")}, 
            object::pair{std::string("env"), utils::assign(object{
                , 
                object::pair{std::string("RUST_LOG"), std::string("info")}
            }, process->env)}
        });
        shared appReady = false;
        shared timeout = setTimeout([=]() mutable
        {
            if (!appReady) {
                reject(std::make_shared<Error>(std::string("Tauri app startup timeout")));
            }
        }
        , 30000);
        this->tauriProcess["stdout"]["on"](std::string("data"), [=](auto data) mutable
        {
            auto output = data->toString();
            console->log(std::string("🎮 Tauri:"), output->trim());
            if (OR((output->includes(std::string("Rust backend setup completed"))), (output->includes(std::string("initialization complete"))))) {
                clearTimeout(timeout);
                if (!appReady) {
                    appReady = true;
                    console->log(std::string("✅ Tauri application is ready"));
                    setTimeout(resolve, 2000);
                }
            }
        }
        );
        this->tauriProcess["stderr"]["on"](std::string("data"), [=](auto data) mutable
        {
            console->error(std::string("🚨 Tauri error:"), data->toString()->trim());
        }
        );
        this->tauriProcess["on"](std::string("close"), [=](auto code) mutable
        {
            console->log(std::string("🎮 Tauri process exited with code ") + code + string_empty);
        }
        );
    }
    );
}

std::shared_ptr<Promise<void>> TauriIntegrationRunner::runIntegrationTests()
{
    console->log(std::string("🧪 Running integration tests..."));
    std::async([=]() { this->runTest(std::string("Health Check"), [=]() mutable
    {
        auto response = std::async([=]() { fetch(std::string("http://localhost:7777/health")); });
        if (!response->ok) {
            throw any(std::make_shared<Error>(std::string("Health check failed: ") + response->status + string_empty));
        }
        auto data = std::async([=]() { response->json(); });
        if (data["status"] != std::string("healthy")) {
            throw any(std::make_shared<Error>(std::string("Server not healthy: ") + data["status"] + string_empty));
        }
    }
    ); });
    std::async([=]() { this->runTest(std::string("Agent Status"), [=]() mutable
    {
        auto response = std::async([=]() { fetch(std::string("http://localhost:7777/api/agents/default/status")); });
        if (!response->ok) {
            throw any(std::make_shared<Error>(std::string("Agent status failed: ") + response->status + string_empty));
        }
        auto data = std::async([=]() { response->json(); });
        if (!data["success"]) {
            throw any(std::make_shared<Error>(std::string("Agent not ready: ") + JSON->stringify(data) + string_empty));
        }
    }
    ); });
    std::async([=]() { this->runTest(std::string("Autonomy Toggle"), [=]() mutable
    {
        auto statusResponse = std::async([=]() { fetch(std::string("http://localhost:7777/autonomy/status")); });
        if (!statusResponse->ok) {
            throw any(std::make_shared<Error>(std::string("Failed to get autonomy status: ") + statusResponse->status + string_empty));
        }
        auto currentStatus = std::async([=]() { statusResponse->json(); });
        auto wasEnabled = OR((currentStatus["data"]["enabled"]), (false));
        auto toggleResponse = std::async([=]() { fetch(std::string("http://localhost:7777/autonomy/") + (wasEnabled) ? std::string("disable") : std::string("enable") + string_empty, object{
            object::pair{std::string("method"), std::string("POST")}
        }); });
        if (!toggleResponse->ok) {
            throw any(std::make_shared<Error>(std::string("Failed to toggle autonomy: ") + toggleResponse->status + string_empty));
        }
        auto newStatusResponse = std::async([=]() { fetch(std::string("http://localhost:7777/autonomy/status")); });
        if (!newStatusResponse->ok) {
            throw any(std::make_shared<Error>(std::string("Failed to verify autonomy status: ") + newStatusResponse->status + string_empty));
        }
        auto newStatus = std::async([=]() { newStatusResponse->json(); });
        auto isNowEnabled = OR((newStatus["data"]["enabled"]), (false));
        if (isNowEnabled == wasEnabled) {
            throw any(std::make_shared<Error>(std::string("Autonomy toggle failed - state did not change")));
        }
        auto restoreResponse = std::async([=]() { fetch(std::string("http://localhost:7777/autonomy/") + (wasEnabled) ? std::string("enable") : std::string("disable") + string_empty, object{
            object::pair{std::string("method"), std::string("POST")}
        }); });
        if (!restoreResponse->ok) {
            console->warn(std::string("Failed to restore original autonomy state"));
        }
    }
    ); });
    std::async([=]() { this->runTest(std::string("Capability Status"), [=]() mutable
    {
        auto capabilities = array<string>{ std::string("browser"), std::string("shell"), std::string("screen"), std::string("camera"), std::string("microphone"), std::string("speakers") };
        for (auto& capability : capabilities)
        {
            auto response = std::async([=]() { fetch(std::string("http://localhost:7777/api/agents/default/capabilities/") + capability + string_empty); });
            if (!response->ok) {
                throw any(std::make_shared<Error>(std::string("Failed to get ") + capability + std::string(" status: ") + response->status + string_empty));
            }
            auto data = std::async([=]() { response->json(); });
            console->log(std::string("  ") + capability + std::string(": ") + JSON->stringify(data) + string_empty);
        }
    }
    ); });
    std::async([=]() { this->runTest(std::string("Message Sending"), [=]() mutable
    {
        auto testMessage = std::string("Test message from integration runner");
        auto terminalResponse = std::async([=]() { fetch(std::string("http://localhost:7777/api/server/terminal-room")); });
        if (!terminalResponse->ok) {
            throw any(std::make_shared<Error>(std::string("Failed to get terminal room: ") + terminalResponse->status + string_empty));
        }
        auto terminalData = std::async([=]() { terminalResponse->json(); });
        auto terminalRoomId = terminalData["data"]["terminalRoomId"];
        if (!terminalRoomId) {
            throw any(std::make_shared<Error>(std::string("Terminal room ID not found")));
        }
        auto messageResponse = std::async([=]() { fetch(std::string("http://localhost:7777/api/messaging/ingest-external"), object{
            object::pair{std::string("method"), std::string("POST")}, 
            object::pair{std::string("headers"), object{
                object::pair{std::string("Content-Type"), std::string("application/json")}
            }}, 
            object::pair{std::string("body"), JSON->stringify(object{
                object::pair{std::string("channel_id"), terminalRoomId}, 
                object::pair{std::string("server_id"), std::string("00000000-0000-0000-0000-000000000000")}, 
                object::pair{std::string("author_id"), std::string("00000000-0000-0000-0000-000000000001")}, 
                object::pair{std::string("content"), testMessage}, 
                object::pair{std::string("source_type"), std::string("integration_test")}, 
                object::pair{std::string("raw_message"), object{
                    object::pair{std::string("text"), testMessage}, 
                    object::pair{std::string("type"), std::string("user_message")}
                }}, 
                object::pair{std::string("metadata"), object{
                    object::pair{std::string("source"), std::string("integration_runner")}, 
                    object::pair{std::string("userName"), std::string("TestRunner")}
                }}
            })}
        }); });
        if (!messageResponse->ok) {
            throw any(std::make_shared<Error>(std::string("Failed to send message: ") + messageResponse->status + string_empty));
        }
        auto messageData = std::async([=]() { messageResponse->json(); });
        if (!messageData["success"]) {
            throw any(std::make_shared<Error>(std::string("Message sending failed: ") + JSON->stringify(messageData) + string_empty));
        }
    }
    ); });
    std::async([=]() { this->runTest(std::string("Data Fetching"), [=]() mutable
    {
        auto endpoints = array<string>{ std::string("/api/goals"), std::string("/api/todos"), std::string("/knowledge/documents"), std::string("/api/plugin-config") };
        for (auto& endpoint : endpoints)
        {
            auto response = std::async([=]() { fetch(std::string("http://localhost:7777") + endpoint + string_empty); });
            if (!response->ok) {
                console->warn(std::string("  ") + endpoint + std::string(": ") + response->status + std::string(" (may be expected)"));
            } else {
                auto data = std::async([=]() { response->json(); });
                console->log(std::string("  ") + endpoint + std::string(": ") + (data["success"]) ? std::string("OK") : std::string("Error") + string_empty);
            }
        }
    }
    ); });
    console->log(std::string("\
📊 Test Results Summary:"));
    console->log(std::string("========================"));
    shared passed = 0;
    shared failed = 0;
    this->results->forEach([=](auto result) mutable
    {
        auto status = (result->passed) ? std::string("✅") : std::string("❌");
        auto duration = (result->duration) ? any(std::string(" (") + result->duration + std::string("ms)")) : any(string_empty);
        console->log(string_empty + status + std::string(" ") + result->name + string_empty + duration + string_empty);
        if (result->error) {
            console->log(std::string("   Error: ") + result->error + string_empty);
        }
        if (result->passed) {
            passed++;
        } else {
            failed++;
        }
    }
    );
    console->log(std::string("\
Passed: ") + passed + std::string(", Failed: ") + failed + string_empty);
    if (failed > 0) {
        throw any(std::make_shared<Error>(string_empty + failed + std::string(" tests failed")));
    }
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> TauriIntegrationRunner::runTest(string name, std::function<std::shared_ptr<Promise<void>>()> testFn)
{
    auto startTime = Date->now();
    try
    {
        console->log(std::string("🧪 Running: ") + name + string_empty);
        std::async([=]() { testFn(); });
        auto duration = Date->now() - startTime;
        this->results->push(object{
            object::pair{std::string("name"), std::string("name")}, 
            object::pair{std::string("passed"), true}, 
            object::pair{std::string("duration"), std::string("duration")}
        });
        console->log(std::string("✅ ") + name + std::string(" passed (") + duration + std::string("ms)"));
    }
    catch (const any& error)
    {
        auto duration = Date->now() - startTime;
        this->results->push(object{
            object::pair{std::string("name"), std::string("name")}, 
            object::pair{std::string("passed"), false}, 
            object::pair{std::string("error"), (is<Error>(error)) ? error->message : String(error)}, 
            object::pair{std::string("duration"), std::string("duration")}
        });
        console->log(std::string("❌ ") + name + std::string(" failed (") + duration + std::string("ms): ") + error + string_empty);
    }
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> TauriIntegrationRunner::generateReport()
{
    auto report = object{
        object::pair{std::string("timestamp"), ((std::make_shared<Date>()))->toISOString()}, 
        object::pair{std::string("results"), this->results}, 
        object::pair{std::string("summary"), object{
            object::pair{std::string("total"), this->results->get_length()}, 
            object::pair{std::string("passed"), this->results->filter([=](auto r) mutable
            {
                return r->passed;
            }
            )->get_length()}, 
            object::pair{std::string("failed"), this->results->filter([=](auto r) mutable
            {
                return !r->passed;
            }
            )->get_length()}, 
            object::pair{std::string("duration"), this->results->reduce([=](auto sum, auto r) mutable
            {
                return sum + (OR((r->duration), (0)));
            }
            , 0)}
        }}
    };
    auto reportPath = path->join(process->cwd(), std::string("test-reports"), std::string("tauri-integration-report.json"));
    std::async([=]() { fs::mkdir(path->dirname(reportPath), object{
        object::pair{std::string("recursive"), true}
    }); });
    std::async([=]() { fs::writeFile(reportPath, JSON->stringify(report, nullptr, 2)); });
    console->log(std::string("📄 Report saved to: ") + reportPath + string_empty);
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> TauriIntegrationRunner::cleanup()
{
    console->log(std::string("🧹 Cleaning up..."));
    if (this->tauriProcess) {
        this->tauriProcess["kill"](std::string("SIGTERM"));
    }
    if (this->serverProcess) {
        this->serverProcess["kill"](std::string("SIGTERM"));
    }
    std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
    {
        return setTimeout(resolve, 2000);
    }
    ); });
    return std::shared_ptr<Promise<void>>();
}


void Main(void)
{
    if (require->main == module) {
        std::shared_ptr<TauriIntegrationRunner> runner = std::make_shared<TauriIntegrationRunner>();
        runner->runAll()->_catch([=](auto error) mutable
        {
            console->error(std::string("❌ Integration tests failed:"), error);
            process->exit(1);
        }
        );
    }
}

MAIN
