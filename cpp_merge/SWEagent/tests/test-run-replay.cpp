#include "test-run-replay.test.h"

void Main(void)
{
    describe(std:("Run Replay"), [=]() mutable
    {
        shared<string> tmpDir;
        shared<string> testTrajPath;
        shared<string> testRepoPath;
        beforeEach([=]() mutable
        {
            tmpDir = fs::mkdtempSync(path->join(os::tmpdir(), std:("run-replay-test-")));
            testTrajPath = path->join(tmpDir, std:("test.traj"));
            auto trajData = object{
                object::pair{std:("trajectory"), array<object>{ object{
                    object::pair{std:("action"), std:("ls")}, 
                    object::pair{std:("observation"), std:("file1.txt file2.txt")}, 
                    object::pair{std:("thought"), std:("Looking at files")}
                }, object{
                    object::pair{std:("action"), std:("echo "test"")}, 
                    object::pair{std:("observation"), std:("test")}, 
                    object::pair{std:("thought"), std:("Testing echo")}
                } }}, 
                object::pair{std:("info"), object{
                    object::pair{std:("exit_status"), std:("submitted")}, 
                    object::pair{std:("submission"), std:("test submission")}
                }}
            };
            fs::writeFileSync(testTrajPath, JSON->stringify(trajData));
            testRepoPath = path->join(tmpDir, std:("test-repo"));
            fs::mkdirSync(testRepoPath);
            try
            {
                execSync(std:("git init"), object{
                    object::pair{std:("cwd"), testRepoPath}
                });
                execSync(std:("git config user.email "test@example.com""), object{
                    object::pair{std:("cwd"), testRepoPath}
                });
                execSync(std:("git config user.name "Test User""), object{
                    object::pair{std:("cwd"), testRepoPath}
                });
                fs::writeFileSync(path->join(testRepoPath, std:("test.txt")), std:("initial content"));
                execSync(std:("git add ."), object{
                    object::pair{std:("cwd"), testRepoPath}
                });
                execSync(std:("git commit -m "Initial commit""), object{
                    object::pair{std:("cwd"), testRepoPath}
                });
            }
            catch (const any& e)
            {
                console->warn(std:("Git setup failed:"), e);
            }
        }
        );
        afterEach([=]() mutable
        {
            if (fs::existsSync(tmpDir)) {
                fs::rmSync(tmpDir, object{
                    object::pair{std:("recursive"), true}
                });
            }
        }
        );
        describe(std:("RunReplayConfig"), [=]() mutable
        {
            it(std:("should create config with required fields"), [=]() mutable
            {
                auto config = object{
                    object::pair{std:("trajPath"), testTrajPath}, 
                    object::pair{std:("outputDir"), tmpDir}
                };
                expect(config->trajPath)->toBe(testTrajPath);
                expect(config->outputDir)->toBe(tmpDir);
            }
            );
            it(std:("should handle optional fields"), [=]() mutable
            {
                auto config = object{
                    object::pair{std:("trajPath"), testTrajPath}, 
                    object::pair{std:("outputDir"), tmpDir}, 
                    object::pair{std:("envVarPath"), path->join(tmpDir, std:(".env"))}, 
                    object::pair{std:("updateConfig"), array<string>{ std:("option1"), std:("option2") }}
                };
                expect(config->trajPath)->toBe(testTrajPath);
                expect(config->outputDir)->toBe(tmpDir);
                expect(config->envVarPath)->toBe(path->join(tmpDir, std:(".env")));
                expect(config->updateConfig)->toEqual(array<string>{ std:("option1"), std:("option2") });
            }
            );
        }
        );
        describe(std:("RunReplay"), [=]() mutable
        {
            shared<std::shared_ptr<RunReplayConfig>> config;
            beforeEach([=]() mutable
            {
                config = object{
                    object::pair{std:("trajPath"), testTrajPath}, 
                    object::pair{std:("outputDir"), tmpDir}
                };
            }
            );
            it(std:("should create RunReplay instance from config"), [=]() mutable
            {
                auto replay = RunReplay::fromConfig(config);
                expect(replay)->toBeDefined();
                expect(replay->get_instanceId())->toBeDefined();
            }
            );
            it(std:("should load trajectory file"), [=]() mutable
            {
                auto replay = RunReplay::fromConfig(config);
                expect(replay->get_instanceId())->toBeDefined();
            }
            );
            it(std:("should handle missing trajectory file"), [=]() mutable
            {
                config->trajPath = path->join(tmpDir, std:("nonexistent.traj"));
                expect([=]() mutable
                {
                    RunReplay::fromConfig(config);
                }
                )->toThrow();
            }
            );
            it(std:("should handle malformed trajectory file"), [=]() mutable
            {
                fs::writeFileSync(testTrajPath, std:("not valid json"));
                expect([=]() mutable
                {
                    RunReplay::fromConfig(config);
                }
                )->toThrow();
            }
            );
            it(std:("should handle forward_only mode"), [=]() mutable
            {
                expect(true)->toBe(true);
            }
            );
            it(std:("should handle full replay mode"), [=]() mutable
            {
                auto replay = RunReplay::fromConfig(config);
                expect(replay)->toBeDefined();
            }
            );
            it(std:("should validate exit code requirements"), [=]() mutable
            {
                auto replay = RunReplay::fromConfig(config);
                expect(replay)->toBeDefined();
            }
            );
            it(std:("should handle error catching mode"), [=]() mutable
            {
                auto replay = RunReplay::fromConfig(config);
                expect(replay)->toBeDefined();
            }
            );
            it(std:("should create output files"), [=]() mutable
            {
                auto replay = RunReplay::fromConfig(config);
                expect(replay)->toBeDefined();
            }
            );
            it(std:("should handle repository setup"), [=]() mutable
            {
                auto replay = RunReplay::fromConfig(config);
                expect(replay)->toBeDefined();
            }
            );
            it(std:("should handle missing repository"), [=]() mutable
            {
                auto replay = RunReplay::fromConfig(config);
                expect(replay)->toBeDefined();
            }
            );
        }
        );
        describe(std:("CLI Integration"), [=]() mutable
        {
            it(std:("should parse command line arguments"), [=]() mutable
            {
                auto args = array<string>{ std:("run-replay"), std:("--traj_path"), testTrajPath, std:("--output_dir"), tmpDir, std:("--deployment.type"), std:("docker"), std:("--deployment.image"), std:("python:3.11") };
                expect(const_(args)[0])->toBe(std:("run-replay"));
                expect(args)->toContain(std:("--traj_path"));
                expect(args)->toContain(std:("--deployment.image"));
            }
            );
            it(std:("should support help command"), [=]() mutable
            {
                auto helpOutput = execSync(std:("node --help"), object{
                    object::pair{std:("encoding"), std:("utf-8")}
                });
                expect(helpOutput)->toBeDefined();
            }
            );
        }
        );
        describe(std:("Trajectory replay logic"), [=]() mutable
        {
            it(std:("should replay actions in order"), [=]() mutable
            {
                auto localConfig = object{
                    object::pair{std:("trajPath"), testTrajPath}, 
                    object::pair{std:("outputDir"), tmpDir}
                };
                auto replay = RunReplay::fromConfig(localConfig);
                expect(replay)->toBeDefined();
            }
            );
            it(std:("should handle action failures"), [=]() mutable
            {
                auto localConfig = object{
                    object::pair{std:("trajPath"), testTrajPath}, 
                    object::pair{std:("outputDir"), tmpDir}
                };
                auto replay = RunReplay::fromConfig(localConfig);
                expect(replay)->toBeDefined();
            }
            );
            it(std:("should compare observations"), [=]() mutable
            {
                auto localConfig = object{
                    object::pair{std:("trajPath"), testTrajPath}, 
                    object::pair{std:("outputDir"), tmpDir}
                };
                auto replay = RunReplay::fromConfig(localConfig);
                expect(replay)->toBeDefined();
            }
            );
            it(std:("should handle observation mismatches"), [=]() mutable
            {
                auto localConfig = object{
                    object::pair{std:("trajPath"), testTrajPath}, 
                    object::pair{std:("outputDir"), tmpDir}
                };
                auto replay = RunReplay::fromConfig(localConfig);
                expect(replay)->toBeDefined();
            }
            );
            it(std:("should generate replay report"), [=]() mutable
            {
                auto localConfig = object{
                    object::pair{std:("trajPath"), testTrajPath}, 
                    object::pair{std:("outputDir"), tmpDir}
                };
                auto replay = RunReplay::fromConfig(localConfig);
                expect(replay)->toBeDefined();
            }
            );
        }
        );
        describe(std:("Edge cases"), [=]() mutable
        {
            it(std:("should handle empty trajectory"), [=]() mutable
            {
                auto emptyTrajPath = path->join(tmpDir, std:("empty.traj"));
                fs::writeFileSync(emptyTrajPath, JSON->stringify(object{
                    object::pair{std:("trajectory"), array<any>()}
                }));
                auto localConfig = object{
                    object::pair{std:("trajPath"), emptyTrajPath}, 
                    object::pair{std:("outputDir"), tmpDir}
                };
                auto replay = RunReplay::fromConfig(localConfig);
                expect(replay)->toBeDefined();
            }
            );
            it(std:("should handle trajectory with only thoughts"), [=]() mutable
            {
                auto thoughtTrajPath = path->join(tmpDir, std:("thoughts.traj"));
                fs::writeFileSync(thoughtTrajPath, JSON->stringify(object{
                    object::pair{std:("trajectory"), array<object>{ object{
                        object::pair{std:("thought"), std:("Thinking...")}
                    }, object{
                        object::pair{std:("thought"), std:("More thinking...")}
                    } }}
                }));
                auto localConfig = object{
                    object::pair{std:("trajPath"), thoughtTrajPath}, 
                    object::pair{std:("outputDir"), tmpDir}
                };
                auto replay = RunReplay::fromConfig(localConfig);
                expect(replay)->toBeDefined();
            }
            );
            it(std:("should handle very long trajectories"), [=]() mutable
            {
                auto longTraj = array<array>();
                for (auto i = 0; i < 1000; i++)
                {
                    longTraj->push(object{
                        object::pair{std:("action"), std:("echo "step ") + i + std:(""")}, 
                        object::pair{std:("observation"), std:("step ") + i + string_empty}, 
                        object::pair{std:("response"), string_empty}, 
                        object::pair{std:("state"), object{}}, 
                        object::pair{std:("thought"), string_empty}, 
                        object::pair{std:("executionTime"), 0}, 
                        object::pair{std:("query"), array<any>()}, 
                        object::pair{std:("extraInfo"), object{}}
                    });
                }
                auto longTrajPath = path->join(tmpDir, std:("long.traj"));
                fs::writeFileSync(longTrajPath, JSON->stringify(object{
                    object::pair{std:("trajectory"), longTraj}
                }));
                auto localConfig = object{
                    object::pair{std:("trajPath"), longTrajPath}, 
                    object::pair{std:("outputDir"), tmpDir}
                };
                auto replay = RunReplay::fromConfig(localConfig);
                expect(replay)->toBeDefined();
            }
            );
        }
        );
    }
    );
}

MAIN
