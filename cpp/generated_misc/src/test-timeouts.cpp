#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/tests/test-timeouts.h"

boolean isCI = OR((process->env->CI == std::string("true")), (process->env->GITHUB_ACTIONS == std::string("true")));
boolean isMacOS = process->platform == std::string("darwin");
boolean isWindows = process->platform == std::string("win32");
object TEST_TIMEOUTS = as<std::shared_ptr<const>>(object{
    object::pair{std::string("SUITE_TIMEOUT"), (isCI) ? 2 * 60 * 1000 : (isWindows) ? 8 * 60 * 1000 : (isMacOS) ? 6 * 60 * 1000 : 5 * 60 * 1000}, 
    object::pair{std::string("INDIVIDUAL_TEST"), (isCI) ? 60 * 1000 : (isWindows) ? 5 * 60 * 1000 : (isMacOS) ? 4 * 60 * 1000 : 3 * 60 * 1000}, 
    object::pair{std::string("QUICK_COMMAND"), (isCI) ? (isMacOS) ? 25 * 1000 : 15 * 1000 : (isWindows) ? 45 * 1000 : (isMacOS) ? 40 * 1000 : 30 * 1000}, 
    object::pair{std::string("STANDARD_COMMAND"), (isCI) ? (isMacOS) ? 45 * 1000 : 30 * 1000 : (isWindows) ? 90 * 1000 : (isMacOS) ? 75 * 1000 : 60 * 1000}, 
    object::pair{std::string("PLUGIN_INSTALLATION"), (isCI) ? 90 * 1000 : (process->platform == std::string("win32")) ? 3 * 60 * 1000 : 2 * 60 * 1000}, 
    object::pair{std::string("PROJECT_CREATION"), (isCI) ? 60 * 1000 : (process->platform == std::string("win32")) ? 3 * 60 * 1000 : 2 * 60 * 1000}, 
    object::pair{std::string("NETWORK_OPERATION"), (isCI) ? 45 * 1000 : (process->platform == std::string("win32")) ? 2 * 60 * 1000 : 90 * 1000}, 
    object::pair{std::string("SERVER_STARTUP"), (isCI) ? (isMacOS) ? 60 * 1000 : 15 * 1000 : (isWindows) ? 45 * 1000 : (isMacOS) ? 40 * 1000 : 30 * 1000}, 
    object::pair{std::string("PROCESS_CLEANUP"), (isCI) ? (isMacOS) ? 8 * 1000 : 5 * 1000 : (isWindows) ? 15 * 1000 : (isMacOS) ? 12 * 1000 : 10 * 1000}, 
    object::pair{std::string("SHORT_WAIT"), (isCI) ? 1 * 1000 : (isWindows) ? 3 * 1000 : (isMacOS) ? 3 * 1000 : 2 * 1000}, 
    object::pair{std::string("MEDIUM_WAIT"), (isCI) ? 2 * 1000 : (isWindows) ? 8 * 1000 : (isMacOS) ? 7 * 1000 : 5 * 1000}, 
    object::pair{std::string("LONG_WAIT"), (isCI) ? 3 * 1000 : (isWindows) ? 15 * 1000 : (isMacOS) ? 12 * 1000 : 10 * 1000}
});
object LEGACY_TIMEOUTS = as<std::shared_ptr<const>>(object{
    object::pair{std::string("DEFAULT_EXECSYNC"), TEST_TIMEOUTS["STANDARD_COMMAND"]}, 
    object::pair{std::string("PROJECT_SETUP"), TEST_TIMEOUTS["PROJECT_CREATION"]}
});

void Main(void)
{
}

MAIN
