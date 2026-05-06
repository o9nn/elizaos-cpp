#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-shell/provider.h"

double MAX_INDIVIDUAL_OUTPUT_LENGTH = 8000;
double TRUNCATE_SEGMENT_LENGTH = 4000;
std::shared_ptr<Provider> shellProvider = object{
    object::pair{std::string("name"), std::string("SHELL_HISTORY")}, 
    object::pair{std::string("description"), std::string("Provides the recent shell command history and current working directory. Assumes ShellService manages overall history length and summarization.")}, 
    object::pair{std::string("position"), 99}, 
    object::pair{std::string("get"), [=](auto runtime, auto _message, auto _state) mutable
    {
        auto shellService = runtime->getService<std::shared_ptr<ShellService>>(as<any>(std::string("SHELL")));
        if (!shellService) {
            logger->warn(std::string("[shellProvider] ShellService not found."));
            return object{
                object::pair{std::string("values"), object{
                    object::pair{std::string("shellHistory"), std::string("Shell service is not available.")}, 
                    object::pair{std::string("currentWorkingDirectory"), std::string("N/A")}
                }}, 
                object::pair{std::string("text"), addHeader(std::string("# Shell Status"), std::string("Shell service is not available."))}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("history"), array<any>()}, 
                    object::pair{std::string("cwd"), std::string("N/A")}
                }}
            };
        }
        auto history = shellService->getHistory();
        shared cwd = shellService->getCurrentWorkingDirectory();
        auto historyText = std::string("No commands in history.");
        if (history->length > 0) {
            historyText = history->map([=](auto entry) mutable
            {
                auto entryStr = std::string("[") + ((std::make_shared<Date>(entry["timestamp"])))->toISOString() + std::string("] ") + entry["cwd"] + std::string("> ") + entry["command"] + string_empty;
                if (entry["output"]) {
                    if (entry["output"]["length"] > MAX_INDIVIDUAL_OUTPUT_LENGTH) {
                        entryStr += std::string("\
  Output: ") + entry["output"]["substring"](0, TRUNCATE_SEGMENT_LENGTH) + std::string("\
  ... [TRUNCATED] ...\
  ") + entry["output"]["substring"](entry["output"]["length"] - TRUNCATE_SEGMENT_LENGTH) + string_empty;
                    } else {
                        entryStr += std::string("\
  Output: ") + entry["output"] + string_empty;
                    }
                }
                if (entry["error"]) {
                    if (entry["error"]["length"] > MAX_INDIVIDUAL_OUTPUT_LENGTH) {
                        entryStr += std::string("\
  Error: ") + entry["error"]["substring"](0, TRUNCATE_SEGMENT_LENGTH) + std::string("\
  ... [TRUNCATED] ...\
  ") + entry["error"]["substring"](entry["error"]["length"] - TRUNCATE_SEGMENT_LENGTH) + string_empty;
                    } else {
                        entryStr += std::string("\
  Error: ") + entry["error"] + string_empty;
                    }
                }
                entryStr += std::string("\
  Exit Code: ") + entry["exitCode"] + string_empty;
                return entryStr;
            }
            )->join(std::string("\
\
"));
        }
        auto text = std::string("Current Directory: ") + cwd + std::string("\
\
") + addHeader(std::string("# Shell History"), historyText) + string_empty;
        return object{
            object::pair{std::string("values"), object{
                object::pair{std::string("shellHistory"), historyText}, 
                object::pair{std::string("currentWorkingDirectory"), cwd}
            }}, 
            object::pair{std::string("text"), std::string("text")}, 
            object::pair{std::string("data"), object{
                object::pair{std::string("history"), std::string("history")}, 
                object::pair{std::string("cwd"), std::string("cwd")}
            }}
        };
    }
    }
};

void Main(void)
{
}

MAIN
