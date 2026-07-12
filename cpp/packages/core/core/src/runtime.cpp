// LocalRuntime - real implementation that executes via popen / std::filesystem.
// Replaces the previous broken transpiled stub.
#include "runtime.hpp"

#include <cstdio>
#include <fstream>
#include <sstream>
#include <array>
#include <stdexcept>
#include <filesystem>

#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#endif

namespace elizaos {
namespace runtime {

namespace {

// Run a shell command, capturing stdout (mixed with stderr via 2>&1) and exit code.
std::pair<std::string, int> runShell(const std::string& cmd, const std::string& cwd) {
    std::string fullCmd;
    if (!cwd.empty()) {
        fullCmd = "cd " + cwd + " && " + cmd + " 2>&1";
    } else {
        fullCmd = cmd + " 2>&1";
    }
    std::array<char, 4096> buffer{};
    std::string output;
    FILE* pipe = ::popen(fullCmd.c_str(), "r");
    if (!pipe) {
        return {"failed to spawn shell", -1};
    }
    while (std::fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr) {
        output.append(buffer.data());
    }
    int status = ::pclose(pipe);
    int exitCode = 0;
    if (status == -1) {
        exitCode = -1;
#ifndef _WIN32
    } else if (WIFEXITED(status)) {
        exitCode = WEXITSTATUS(status);
#endif
    } else {
        // On Windows, pclose() returns the exit code directly.
        exitCode = status;
    }
    return {output, exitCode};
}

} // namespace

LocalRuntime::LocalRuntime() = default;
LocalRuntime::~LocalRuntime() = default;

std::future<void> LocalRuntime::createSession(const CreateBashSessionRequest& request) {
    return std::async(std::launch::deferred, [this, request]() {
        startupSource_ = request.startupSource;
        sessionActive_ = true;
    });
}

std::future<BashActionResult> LocalRuntime::runInSession(const BashAction& action) {
    return std::async(std::launch::deferred, [this, action]() {
        if (!sessionActive_) {
            throw std::runtime_error("LocalRuntime: no active session");
        }
        std::string composite;
        for (const auto& src : startupSource_) {
            composite += "source " + src + "; ";
        }
        composite += action.command;
        auto [out, ec] = runShell(composite, "");
        if (action.check && ec != 0) {
            throw std::runtime_error("BashAction failed (exit " + std::to_string(ec) + "): " + out);
        }
        BashActionResult r;
        r.output = out;
        r.exitCode = ec;
        return r;
    });
}

std::future<CommandResult> LocalRuntime::execute(const Command& command) {
    return std::async(std::launch::deferred, [command]() {
        std::string envPrefix;
        for (const auto& kv : command.env) {
            envPrefix += kv.first + "=\"" + kv.second + "\" ";
        }
        std::string cmd = envPrefix + command.command;
        auto [out, ec] = runShell(cmd, command.cwd);
        if (command.check && ec != 0) {
            throw std::runtime_error("Command failed (exit " + std::to_string(ec) + "): " + out);
        }
        CommandResult r;
        r.exitCode = ec;
        r.stdoutText = out;
        r.stderrText.clear(); // captured combined; left empty for compatibility
        return r;
    });
}

std::future<ReadFileResponse> LocalRuntime::readFile(const ReadFileRequest& request) {
    return std::async(std::launch::deferred, [request]() {
        std::ifstream in(request.path, std::ios::binary);
        if (!in) {
            throw std::runtime_error("readFile: cannot open '" + request.path + "'");
        }
        std::ostringstream ss;
        ss << in.rdbuf();
        ReadFileResponse r;
        r.content = ss.str();
        return r;
    });
}

std::future<void> LocalRuntime::writeFile(const WriteFileRequest& request) {
    return std::async(std::launch::deferred, [request]() {
        std::filesystem::path p(request.path);
        if (p.has_parent_path()) {
            std::error_code ec;
            std::filesystem::create_directories(p.parent_path(), ec);
        }
        std::ofstream out(request.path, std::ios::binary | std::ios::trunc);
        if (!out) {
            throw std::runtime_error("writeFile: cannot open '" + request.path + "'");
        }
        out.write(request.content.data(), static_cast<std::streamsize>(request.content.size()));
    });
}

std::future<void> LocalRuntime::upload(const UploadRequest& request) {
    return std::async(std::launch::deferred, [request]() {
        std::filesystem::path src(request.sourcePath);
        std::filesystem::path dst(request.targetPath);
        if (dst.has_parent_path()) {
            std::error_code ec;
            std::filesystem::create_directories(dst.parent_path(), ec);
        }
        std::error_code ec;
        std::filesystem::copy_file(src, dst,
            std::filesystem::copy_options::overwrite_existing, ec);
        if (ec) {
            throw std::runtime_error("upload: " + ec.message());
        }
    });
}

} // namespace runtime
} // namespace elizaos
