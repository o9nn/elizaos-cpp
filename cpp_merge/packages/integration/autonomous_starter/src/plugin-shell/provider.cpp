#include "provider.hpp"

double MAX_INDIVIDUAL_OUTPUT_LENGTH = 8000;
double TRUNCATE_SEGMENT_LENGTH = 4000;
std::shared_ptr<Provider> shellProvider = object{
    object::pair{std:("name"), std:("SHELL_HISTORY")}, 
    object::pair{std:("description"), std:("Provides the recent shell command history and current working directory. Assumes ShellService manages overall history length and summarization.")}, 
    object::pair{std:("position"), 99}, 
    object::pair{std:("get"), [=](auto runtime, auto _message, auto _state) mutable
    {
        auto shellService = runtime->getService<std::shared_ptr<ShellService>>(as<any>(std:("SHELL")));
        if (!shellService) {
            logger->warn(std:("[shellProvider] ShellService not found."));
            return object{
                object::pair{std:("values"), object{
                    object::pair{std:("shellHistory"), std:("Shell service is not available.")}, 
                    object::pair{std:("currentWorkingDirectory"), std:("N/A")}
                }}, 
                object::pair{std:("text"), addHeader(std:("# Shell Status"), std:("Shell service is not available."))}, 
                object::pair{std:("data"), object{
                    object::pair{std:("history"), array<any>()}, 
                    object::pair{std:("cwd"), std:("N/A")}
                }}
            };
        }
        auto history = shellService->getHistory();
        shared cwd = shellService->getCurrentWorkingDirectory();
        auto historyText = std:("No commands in history.");
        if (history->length > 0) {
            historyText = history->map([=](auto entry) mutable
            {
                auto entryStr = std:("[") + ((std::make_shared<Date>(entry["timestamp"])))->toISOString() + std:("] ") + entry["cwd"] + std:("> ") + entry["command"] + string_empty;
                if (entry["output"]) {
                    if (entry["output"]["length"] > MAX_INDIVIDUAL_OUTPUT_LENGTH) {
                        entryStr += std:("\
  Output: ") + entry["output"]["substring"](0, TRUNCATE_SEGMENT_LENGTH) + std:("\
  ... [TRUNCATED] ...\
  ") + entry["output"]["substring"](entry["output"]["length"] - TRUNCATE_SEGMENT_LENGTH) + string_empty;
                    } else {
                        entryStr += std:("\
  Output: ") + entry["output"] + string_empty;
                    }
                }
                if (entry["error"]) {
                    if (entry["error"]["length"] > MAX_INDIVIDUAL_OUTPUT_LENGTH) {
                        entryStr += std:("\
  Error: ") + entry["error"]["substring"](0, TRUNCATE_SEGMENT_LENGTH) + std:("\
  ... [TRUNCATED] ...\
  ") + entry["error"]["substring"](entry["error"]["length"] - TRUNCATE_SEGMENT_LENGTH) + string_empty;
                    } else {
                        entryStr += std:("\
  Error: ") + entry["error"] + string_empty;
                    }
                }
                entryStr += std:("\
  Exit Code: ") + entry["exitCode"] + string_empty;
                return entryStr;
            }
            )->join(std:("\
\
"));
        }
        auto text = std:("Current Directory: ") + cwd + std:("\
\
") + addHeader(std:("# Shell History"), historyText) + string_empty;
        return object{
            object::pair{std:("values"), object{
                object::pair{std:("shellHistory"), historyText}, 
                object::pair{std:("currentWorkingDirectory"), cwd}
            }}, 
            object::pair{std:("text"), std:("text")}, 
            object::pair{std:("data"), object{
                object::pair{std:("history"), std:("history")}, 
                object::pair{std:("cwd"), std:("cwd")}
            }}
        };
    }
    }
};

void Main(void)
{
}

MAIN
