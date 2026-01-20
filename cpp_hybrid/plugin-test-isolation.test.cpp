#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/tests/integration/plugin-test-isolation.test.h"

void Main(void)
{
    describe(std::string("Plugin Test Isolation"), [=]() mutable
    {
        shared<string> tempDir;
        shared cliPath = join(process->cwd(), std::string("dist"), std::string("index.js"));
        beforeEach([=]() mutable
        {
            tempDir = mkdtempSync(join(tmpdir(), std::string("cli-test-")));
        }
        );
        afterEach([=]() mutable
        {
            rmSync(tempDir, object{
                object::pair{std::string("recursive"), true}, 
                object::pair{std::string("force"), true}
            });
        }
        );
        it(std::string("should only run tests for the specific plugin being tested"), [=]() mutable
        {
            auto pluginDir = join(tempDir, std::string("test-plugin"));
            mkdirSync(pluginDir, object{
                object::pair{std::string("recursive"), true}
            });
            auto packageJson = object{
                object::pair{std::string("name"), std::string("test-plugin")}, 
                object::pair{std::string("version"), std::string("1.0.0")}, 
                object::pair{std::string("dependencies"), object{
                    object::pair{std::string("@elizaos/core"), std::string("*")}, 
                    object::pair{std::string("@elizaos/plugin-sql"), std::string("*")}
                }}
            };
            writeFileSync(join(pluginDir, std::string("package.json")), JSON->stringify(packageJson, nullptr, 2));
            auto pluginContent = std::string("\
export const testPlugin = {\
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
            mkdirSync(join(pluginDir, std::string("src")), object{
                object::pair{std::string("recursive"), true}
            });
            writeFileSync(join(pluginDir, std::string("src"), std::string("index.ts")), pluginContent);
            try
            {
                auto output = execSync(std::string("node ") + cliPath + std::string(" test --skip-build"), object{
                    object::pair{std::string("cwd"), pluginDir}, 
                    object::pair{std::string("encoding"), std::string("utf8")}, 
                    object::pair{std::string("env"), utils::assign(object{
                        , 
                        object::pair{std::string("NODE_ENV"), std::string("test")}
                    }, process->env)}
                });
                expect(output)->toContain(std::string("plugin: test-plugin"));
                expect(output)->not->toContain(std::string("Running test suite: sql_test_suite"));
                expect(output)->not->toContain(std::string("@elizaos/plugin-sql"));
            }
            catch (const any& error)
            {
                auto errorOutput = (AND((is<Error>(error)), (in(std::string("stderr"), error)))) ? (as<any>(error))["stderr"] : (AND((is<Error>(error)), (in(std::string("stdout"), error)))) ? any((as<any>(error))["stdout"]) : any(string_empty);
                expect(errorOutput)->toContain(std::string("plugin-test-a"));
            }
        }
        );
        it(std::string("should set ELIZA_TESTING_PLUGIN environment variable for plugins"), [=]() mutable
        {
            auto pluginDir = join(tempDir, std::string("env-test-plugin"));
            mkdirSync(pluginDir, object{
                object::pair{std::string("recursive"), true}
            });
            auto packageJson = object{
                object::pair{std::string("name"), std::string("env-test-plugin")}, 
                object::pair{std::string("version"), std::string("1.0.0")}, 
                object::pair{std::string("dependencies"), object{
                    object::pair{std::string("@elizaos/core"), std::string("*")}
                }}
            };
            writeFileSync(join(pluginDir, std::string("package.json")), JSON->stringify(packageJson, nullptr, 2));
            auto pluginContent = std::string("\
console.log('ELIZA_TESTING_PLUGIN:', process.env.ELIZA_TESTING_PLUGIN);\
export const envTestPlugin = {\
  name: 'env-test-plugin',\
  tests: []\
};\
");
            mkdirSync(join(pluginDir, std::string("src")), object{
                object::pair{std::string("recursive"), true}
            });
            writeFileSync(join(pluginDir, std::string("src"), std::string("index.ts")), pluginContent);
            try
            {
                auto output = execSync(std::string("node ") + cliPath + std::string(" test --skip-build"), object{
                    object::pair{std::string("cwd"), pluginDir}, 
                    object::pair{std::string("encoding"), std::string("utf8")}, 
                    object::pair{std::string("env"), utils::assign(object{
                        , 
                        object::pair{std::string("NODE_ENV"), std::string("test")}
                    }, process->env)}
                });
                expect(output)->toContain(std::string("ELIZA_TESTING_PLUGIN: true"));
            }
            catch (const any& error)
            {
                auto errorOutput = (AND((is<Error>(error)), (in(std::string("stderr"), error)))) ? (as<any>(error))["stderr"] : (AND((is<Error>(error)), (in(std::string("stdout"), error)))) ? any((as<any>(error))["stdout"]) : any(string_empty);
                expect(errorOutput)->toContain(std::string("plugin-test-b"));
            }
        }
        );
    }
    );
}

MAIN
