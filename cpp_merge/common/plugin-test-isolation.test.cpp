#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/tests/integration/plugin-test-isolation.test.h"
#include <cstdlib>

void Main(void)
{
    describe(std:("Plugin Test Isolation"), [=]() mutable
    {
        shared<string> tempDir;
        shared cliPath = join(process->cwd(), std:("dist"), std:("index.js"));
        beforeEach([=]() mutable
        {
            tempDir = mkdtempSync(join(tmpdir(), std:("cli-test-")));
        }
        );
        afterEach([=]() mutable
        {
            rmSync(tempDir, object{
                object::pair{std:("recursive"), true}, 
                object::pair{std:("force"), true}
            });
        }
        );
        it(std:("should only run tests for the specific plugin being tested"), [=]() mutable
        {
            auto pluginDir = join(tempDir, std:("test-plugin"));
            mkdirSync(pluginDir, object{
                object::pair{std:("recursive"), true}
            });
            auto packageJson = object{
                object::pair{std:("name"), std:("test-plugin")}, 
                object::pair{std:("version"), std:("1.0.0")}, 
                object::pair{std:("dependencies"), object{
                    object::pair{std:("@elizaos/core"), std:("*")}, 
                    object::pair{std:("@elizaos/plugin-sql"), std:("*")}
                }}
            };
            writeFileSync(join(pluginDir, std:("package.json")), JSON->stringify(packageJson, nullptr, 2));
            auto pluginContent = std:("\
const testPlugin = {\
  name: 'test-plugin',\
  tests: [{\
    name: 'test-plugin-suite',\
    tests: [{\
      name: 'test-plugin-test',\
      handler: async () => ({ success: true, message: 'Test passed' })\
    }]\
  }]\
};\
");
            mkdirSync(join(pluginDir, std:("src")), object{
                object::pair{std:("recursive"), true}
            });
            writeFileSync(join(pluginDir, std:("src"), std:("index.ts")), pluginContent);
            try
            {
                auto output = execSync(std:("node ") + cliPath + std:(" test --skip-build"), object{
                    object::pair{std:("cwd"), pluginDir}, 
                    object::pair{std:("encoding"), std:("utf8")}, 
                    object::pair{std:("env"), utils::assign(object{
                        , 
                        object::pair{std:("NODE_ENV"), std:("test")}
                    }, process->env)}
                });
                expect(output)->toContain(std:("plugin: test-plugin"));
                expect(output)->not->toContain(std:("Running test suite: sql_test_suite"));
                expect(output)->not->toContain(std:("@elizaos/plugin-sql"));
            }
            catch (const any& error)
            {
                auto errorOutput = (AND((is<Error>(error)), (in(std:("stderr"), error)))) ? (as<any>(error))["stderr"] : (AND((is<Error>(error)), (in(std:("stdout"), error)))) ? any((as<any>(error))["stdout"]) (string_empty);
                expect(errorOutput)->toContain(std:("plugin-test-a"));
            }
        }
        );
        it(std:("should set ELIZA_TESTING_PLUGIN environment variable for plugins"), [=]() mutable
        {
            auto pluginDir = join(tempDir, std:("env-test-plugin"));
            mkdirSync(pluginDir, object{
                object::pair{std:("recursive"), true}
            });
            auto packageJson = object{
                object::pair{std:("name"), std:("env-test-plugin")}, 
                object::pair{std:("version"), std:("1.0.0")}, 
                object::pair{std:("dependencies"), object{
                    object::pair{std:("@elizaos/core"), std:("*")}
                }}
            };
            writeFileSync(join(pluginDir, std:("package.json")), JSON->stringify(packageJson, nullptr, 2));
            auto pluginContent = std:("\
std::cout << 'ELIZA_TESTING_PLUGIN:', std::getenv("ELIZA_TESTING_PLUGIN") << std::endl;\
const envTestPlugin = {\
  name: 'env-test-plugin',\
  tests: []\
};\
");
            mkdirSync(join(pluginDir, std:("src")), object{
                object::pair{std:("recursive"), true}
            });
            writeFileSync(join(pluginDir, std:("src"), std:("index.ts")), pluginContent);
            try
            {
                auto output = execSync(std:("node ") + cliPath + std:(" test --skip-build"), object{
                    object::pair{std:("cwd"), pluginDir}, 
                    object::pair{std:("encoding"), std:("utf8")}, 
                    object::pair{std:("env"), utils::assign(object{
                        , 
                        object::pair{std:("NODE_ENV"), std:("test")}
                    }, process->env)}
                });
                expect(output)->toContain(std:("ELIZA_TESTING_PLUGIN: true"));
            }
            catch (const any& error)
            {
                auto errorOutput = (AND((is<Error>(error)), (in(std:("stderr"), error)))) ? (as<any>(error))["stderr"] : (AND((is<Error>(error)), (in(std:("stdout"), error)))) ? any((as<any>(error))["stdout"]) (string_empty);
                expect(errorOutput)->toContain(std:("plugin-test-b"));
            }
        }
        );
    }
    );
}

MAIN
