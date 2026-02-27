#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/game/tests/tauri-integration-runner.h"

std::shared_ptr<Promise<void>> TauriIntegrationRunner::runAll()
{
    console->log(std:("🚀 Starting Tauri Integration Test Runner"));
    console->log(std:("============="));
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
            console->error(std:("❌ Test runner failed:"), error);
            process->exit(1);
        }
    }
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> TauriIntegrationRunner::buildTauriApp()
{
    console->log(std:("📦 Building Tauri application..."));
    return std::make_shared<Promise>([=](auto resolve, auto reject) mutable
    {
        auto buildProcess = spawn(std:("npm"), array<string>{ std:("run"), std:("build:tauri") }, object{
            object::pair{std:("cwd"), process->cwd()}, 
            object::pair{std:("stdio"), std:("pipe")}
        });
        shared output = string_empty;
        buildProcess->stdout->on(std:("data"), [=](auto data) mutable
        {
            output += data["toString"]();
            process->stdout->write(data);
        }
        );
        buildProcess->stderr->on(std:("data"), [=](auto data) mutable
        {
            output += data["toString"]();
            process->stderr->write(data);
        }
        );
        buildProcess->on(std:("close"), [=](auto code) mutable
        {
            if (code == 0) {
                console->log(std:("✅ Tauri build completed successfully"));
                resolve();
            } else {
                reject(std::make_shared<Error>(std:("Build failed with exit code ") + code + string_empty));
            }
        }
        );
    }
    );
}

std::shared_ptr<Promise<void>> TauriIntegrationRunner::startElizaServer()
{
    console->log(std:("🌟 Starting ElizaOS server..."));
    return std::make_shared<Promise>([=](auto resolve, auto reject) mutable
    {
        this->serverProcess = spawn(std:("elizaos"), array<string>{ std:("start") }, object{
            object::pair{std:("cwd"), path->join(process->cwd(), std:("../.."))}, 
            object::pair{std:("stdio"), std:("pipe")}
        });
        shared serverReady = false;
        shared timeout = setTimeout([=]() mutable
        {
            if (!serverReady) {
                reject(std::make_shared<Error>(std:("Server startup timeout")));
            }
        }
        , 60000);
        this->serverProcess["stdout"]["on"](std:("data"), [=](auto data) mutable
        {
            auto output = data->toString();
            console->log(std:("📡 Server:"), output->trim());
            if (OR((OR((output->includes(std:("Server running"))), (output->includes(std:("listening on"))))), (output->includes(std:("started successfully"))))) {
                clearTimeout(timeout);
                if (!serverReady) {
                    serverReady = true;
                    console->log(std:("✅ ElizaOS server is ready"));
                    setTimeout(resolve, 3000);
                }
            }
        }
        );
        this->serverProcess["stderr"]["on"](std:("data"), [=](auto data) mutable
        {
            console->error(std:("🚨 Server error:"), data->toString()->trim());
        }
        );
        this->serverProcess["on"](std:("close"), [=](auto code) mutable
        {
            console->log(std:("📡 Server process exited with code ") + code + string_empty);
        }
        );
    }
    );
}

std::shared_ptr<Promise<void>> TauriIntegrationRunner::startTauriApp()
{
    console->log(std:("🎮 Starting Tauri application..."));
    return std::make_shared<Promise>([=](auto resolve, auto reject) mutable
    {
        auto appPath = path->join(process->cwd(), std:("src-tauri"), std:("target"), std:("debug"), std:("eliza-game"));
        this->tauriProcess = spawn(appPath, array<any>(), object{
            object::pair{std:("stdio"), std:("pipe")}, 
            object::pair{std:("env"), utils::assign(object{
                , 
                object::pair{std:("RUST_LOG"), std:("info")}
            }, process->env)}
        });
        shared appReady = false;
        shared timeout = setTimeout([=]() mutable
        {
            if (!appReady) {
                reject(std::make_shared<Error>(std:("Tauri app startup timeout")));
            }
        }
        , 30000);
        this->tauriProcess["stdout"]["on"](std:("data"), [=](auto data) mutable
        {
            auto output = data->toString();
            console->log(std:("🎮 Tauri:"), output->trim());
            if (OR((output->includes(std:("Rust backend setup completed"))), (output->includes(std:("initialization complete"))))) {
                clearTimeout(timeout);
                if (!appReady) {
                    appReady = true;
                    console->log(std:("✅ Tauri application is ready"));
                    setTimeout(resolve, 2000);
                }
            }
        }
        );
        this->tauriProcess["stderr"]["on"](std:("data"), [=](auto data) mutable
        {
            console->error(std:("🚨 Tauri error:"), data->toString()->trim());
        }
        );
        this->tauriProcess["on"](std:("close"), [=](auto code) mutable
        {
            console->log(std:("🎮 Tauri process exited with code ") + code + string_empty);
        }
        );
    }
    );
}

std::shared_ptr<Promise<void>> TauriIntegrationRunner::runIntegrationTests()
{
    console->log(std:("🧪 Running integration tests..."));
    std::async([=]() { this->runTest(std:("Health Check"), [=]() mutable
    {
        auto response = std::async([=]() { fetch(std:("http://localhost:7777/health")); });
        if (!response->ok) {
            throw any(std::make_shared<Error>(std:("Health check failed: ") + response->status + string_empty));
        }
        auto data = std::async([=]() { response->json(); });
        if (data["status"] != std:("healthy")) {
            throw any(std::make_shared<Error>(std:("Server not healthy: ") + data["status"] + string_empty));
        }
    }
    ); });
    std::async([=]() { this->runTest(std:("Agent Status"), [=]() mutable
    {
        auto response = std::async([=]() { fetch(std:("http://localhost:7777/api/agents/default/status")); });
        if (!response->ok) {
            throw any(std::make_shared<Error>(std:("Agent status failed: ") + response->status + string_empty));
        }
        auto data = std::async([=]() { response->json(); });
        if (!data["success"]) {
            throw any(std::make_shared<Error>(std:("Agent not ready: ") + JSON->stringify(data) + string_empty));
        }
    }
    ); });
    std::async([=]() { this->runTest(std:("Autonomy Toggle"), [=]() mutable
    {
        auto statusResponse = std::async([=]() { fetch(std:("http://localhost:7777/autonomy/status")); });
        if (!statusResponse->ok) {
            throw any(std::make_shared<Error>(std:("Failed to get autonomy status: ") + statusResponse->status + string_empty));
        }
        auto currentStatus = std::async([=]() { statusResponse->json(); });
        auto wasEnabled = OR((currentStatus["data"]["enabled"]), (false));
        auto toggleResponse = std::async([=]() { fetch(std:("http://localhost:7777/autonomy/") + (wasEnabled) ? std:("disable") : std:("enable") + string_empty, object{
            object::pair{std:("method"), std:("POST")}
        }); });
        if (!toggleResponse->ok) {
            throw any(std::make_shared<Error>(std:("Failed to toggle autonomy: ") + toggleResponse->status + string_empty));
        }
        auto newStatusResponse = std::async([=]() { fetch(std:("http://localhost:7777/autonomy/status")); });
        if (!newStatusResponse->ok) {
            throw any(std::make_shared<Error>(std:("Failed to verify autonomy status: ") + newStatusResponse->status + string_empty));
        }
        auto newStatus = std::async([=]() { newStatusResponse->json(); });
        auto isNowEnabled = OR((newStatus["data"]["enabled"]), (false));
        if (isNowEnabled == wasEnabled) {
            throw any(std::make_shared<Error>(std:("Autonomy toggle failed - state did not change")));
        }
        auto restoreResponse = std::async([=]() { fetch(std:("http://localhost:7777/autonomy/") + (wasEnabled) ? std:("enable") : std:("disable") + string_empty, object{
            object::pair{std:("method"), std:("POST")}
        }); });
        if (!restoreResponse->ok) {
            console->warn(std:("Failed to restore original autonomy state"));
        }
    }
    ); });
    std::async([=]() { this->runTest(std:("Capability Status"), [=]() mutable
    {
        auto capabilities = array<string>{ std:("browser"), std:("shell"), std:("screen"), std:("camera"), std:("microphone"), std:("speakers") };
        for (auto& capability : capabilities)
        {
            auto response = std::async([=]() { fetch(std:("http://localhost:7777/api/agents/default/capabilities/") + capability + string_empty); });
            if (!response->ok) {
                throw any(std::make_shared<Error>(std:("Failed to get ") + capability + std:(" status: ") + response->status + string_empty));
            }
            auto data = std::async([=]() { response->json(); });
            console->log(std:("  ") + capability + std:(": ") + JSON->stringify(data) + string_empty);
        }
    }
    ); });
    std::async([=]() { this->runTest(std:("Message Sending"), [=]() mutable
    {
        auto testMessage = std:("Test message from integration runner");
        auto terminalResponse = std::async([=]() { fetch(std:("http://localhost:7777/api/server/terminal-room")); });
        if (!terminalResponse->ok) {
            throw any(std::make_shared<Error>(std:("Failed to get terminal room: ") + terminalResponse->status + string_empty));
        }
        auto terminalData = std::async([=]() { terminalResponse->json(); });
        auto terminalRoomId = terminalData["data"]["terminalRoomId"];
        if (!terminalRoomId) {
            throw any(std::make_shared<Error>(std:("Terminal room ID not found")));
        }
        auto messageResponse = std::async([=]() { fetch(std:("http://localhost:7777/api/messaging/ingest-external"), object{
            object::pair{std:("method"), std:("POST")}, 
            object::pair{std:("headers"), object{
                object::pair{std:("Content-Type"), std:("application/json")}
            }}, 
            object::pair{std:("body"), JSON->stringify(object{
                object::pair{std:("channel_id"), terminalRoomId}, 
                object::pair{std:("server_id"), std:("00000000-0000-0000-0000-000000000000")}, 
                object::pair{std:("author_id"), std:("00000000-0000-0000-0000-000000000001")}, 
                object::pair{std:("content"), testMessage}, 
                object::pair{std:("source_type"), std:("integration_test")}, 
                object::pair{std:("raw_message"), object{
                    object::pair{std:("text"), testMessage}, 
                    object::pair{std:("type"), std:("user_message")}
                }}, 
                object::pair{std:("metadata"), object{
                    object::pair{std:("source"), std:("integration_runner")}, 
                    object::pair{std:("userName"), std:("TestRunner")}
                }}
            })}
        }); });
        if (!messageResponse->ok) {
            throw any(std::make_shared<Error>(std:("Failed to send message: ") + messageResponse->status + string_empty));
        }
        auto messageData = std::async([=]() { messageResponse->json(); });
        if (!messageData["success"]) {
            throw any(std::make_shared<Error>(std:("Message sending failed: ") + JSON->stringify(messageData) + string_empty));
        }
    }
    ); });
    std::async([=]() { this->runTest(std:("Data Fetching"), [=]() mutable
    {
        auto endpoints = array<string>{ std:("/api/goals"), std:("/api/todos"), std:("/knowledge/documents"), std:("/api/plugin-config") };
        for (auto& endpoint : endpoints)
        {
            auto response = std::async([=]() { fetch(std:("http://localhost:7777") + endpoint + string_empty); });
            if (!response->ok) {
                console->warn(std:("  ") + endpoint + std:(": ") + response->status + std:(" (may be expected)"));
            } else {
                auto data = std::async([=]() { response->json(); });
                console->log(std:("  ") + endpoint + std:(": ") + (data["success"]) ? std:("OK") : std:("Error") + string_empty);
            }
        }
    }
    ); });
    console->log(std:("\
📊 Test Results Summary:"));
    console->log(std:("========"));
    shared passed = 0;
    shared failed = 0;
    this->results->forEach([=](auto result) mutable
    {
        auto status = (result->passed) ? std:("✅") : std:("❌");
        auto duration = (result->duration) ? any(std:(" (") + result->duration + std:("ms)")) (string_empty);
        console->log(string_empty + status + std:(" ") + result->name + string_empty + duration + string_empty);
        if (result->error) {
            console->log(std:("   Error: ") + result->error + string_empty);
        }
        if (result->passed) {
            passed++;
        } else {
            failed++;
        }
    }
    );
    console->log(std:("\
Passed: ") + passed + std:(", Failed: ") + failed + string_empty);
    if (failed > 0) {
        throw any(std::make_shared<Error>(string_empty + failed + std:(" tests failed")));
    }
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> TauriIntegrationRunner::runTest(string name, std::function<std::shared_ptr<Promise<void>>()> testFn)
{
    auto startTime = Date->now();
    try
    {
        console->log(std:("🧪 Running: ") + name + string_empty);
        std::async([=]() { testFn(); });
        auto duration = Date->now() - startTime;
        this->results->push(object{
            object::pair{std:("name"), std:("name")}, 
            object::pair{std:("passed"), true}, 
            object::pair{std:("duration"), std:("duration")}
        });
        console->log(std:("✅ ") + name + std:(" passed (") + duration + std:("ms)"));
    }
    catch (const any& error)
    {
        auto duration = Date->now() - startTime;
        this->results->push(object{
            object::pair{std:("name"), std:("name")}, 
            object::pair{std:("passed"), false}, 
            object::pair{std:("error"), (is<Error>(error)) ? error->message : String(error)}, 
            object::pair{std:("duration"), std:("duration")}
        });
        console->log(std:("❌ ") + name + std:(" failed (") + duration + std:("ms): ") + error + string_empty);
    }
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> TauriIntegrationRunner::generateReport()
{
    auto report = object{
        object::pair{std:("timestamp"), ((std::make_shared<Date>()))->toISOString()}, 
        object::pair{std:("results"), this->results}, 
        object::pair{std:("summary"), object{
            object::pair{std:("total"), this->results->get_length()}, 
            object::pair{std:("passed"), this->results->filter([=](auto r) mutable
            {
                return r->passed;
            }
            )->get_length()}, 
            object::pair{std:("failed"), this->results->filter([=](auto r) mutable
            {
                return !r->passed;
            }
            )->get_length()}, 
            object::pair{std:("duration"), this->results->reduce([=](auto sum, auto r) mutable
            {
                return sum + (OR((r->duration), (0)));
            }
            , 0)}
        }}
    };
    auto reportPath = path->join(process->cwd(), std:("test-reports"), std:("tauri-integration-report.json"));
    std::async([=]() { fs::mkdir(path->dirname(reportPath), object{
        object::pair{std:("recursive"), true}
    }); });
    std::async([=]() { fs::writeFile(reportPath, JSON->stringify(report, nullptr, 2)); });
    console->log(std:("📄 Report saved to: ") + reportPath + string_empty);
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> TauriIntegrationRunner::cleanup()
{
    console->log(std:("🧹 Cleaning up..."));
    if (this->tauriProcess) {
        this->tauriProcess["kill"](std:("SIGTERM"));
    }
    if (this->serverProcess) {
        this->serverProcess["kill"](std:("SIGTERM"));
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
            console->error(std:("❌ Integration tests failed:"), error);
            process->exit(1);
        }
        );
    }
}

MAIN
