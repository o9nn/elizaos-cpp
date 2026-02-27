#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/tests/test-quick-stats.test.h"

void Main(void)
{
    describe(std:("Quick Stats"), [=]() mutable
    {
        shared<string> tmpDir;
        beforeEach([=]() mutable
        {
            tmpDir = fs::mkdtempSync(path->join(os::tmpdir(), std:("quick-stats-test-")));
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
        describe(std:("quickStats function"), [=]() mutable
        {
            it(std:("should handle empty directories"), [=]() mutable
            {
                auto result = std::async([=]() { quickStats(tmpDir); });
                expect(result)->toBe(std:("No .traj files found."));
            }
            );
            it(std:("should process trajectory files correctly"), [=]() mutable
            {
                auto trajFile = path->join(tmpDir, std:("test.traj"));
                auto trajData = object{
                    object::pair{std:("info"), object{
                        object::pair{std:("model_stats"), object{
                            object::pair{std:("api_calls"), 42}, 
                            object::pair{std:("tokens_sent"), 1000}, 
                            object::pair{std:("tokens_received"), 500}, 
                            object::pair{std:("instance_cost"), 0.05}
                        }}, 
                        object::pair{std:("exit_status"), std:("success")}
                    }}
                };
                fs::writeFileSync(trajFile, JSON->stringify(trajData));
                auto result = std::async([=]() { quickStats(tmpDir); });
                expect(result)->toContain(std:("## "success""));
                expect(result)->not->toBe(std:("No .traj files found."));
            }
            );
            it(std:("should handle multiple trajectory files"), [=]() mutable
            {
                auto trajData1 = object{
                    object::pair{std:("info"), object{
                        object::pair{std:("model_stats"), object{
                            object::pair{std:("api_calls"), 10}, 
                            object::pair{std:("tokens_sent"), 500}, 
                            object::pair{std:("tokens_received"), 200}, 
                            object::pair{std:("instance_cost"), 0.02}
                        }}, 
                        object::pair{std:("exit_status"), std:("success")}
                    }}
                };
                auto trajData2 = object{
                    object::pair{std:("info"), object{
                        object::pair{std:("model_stats"), object{
                            object::pair{std:("api_calls"), 5}, 
                            object::pair{std:("tokens_sent"), 300}, 
                            object::pair{std:("tokens_received"), 150}, 
                            object::pair{std:("instance_cost"), 0.01}
                        }}, 
                        object::pair{std:("exit_status"), std:("exit_cost")}
                    }}
                };
                auto trajData3 = object{
                    object::pair{std:("info"), object{
                        object::pair{std:("model_stats"), object{
                            object::pair{std:("api_calls"), 20}, 
                            object::pair{std:("tokens_sent"), 1500}, 
                            object::pair{std:("tokens_received"), 800}, 
                            object::pair{std:("instance_cost"), 0.08}
                        }}, 
                        object::pair{std:("exit_status"), std:("success")}
                    }}
                };
                fs::writeFileSync(path->join(tmpDir, std:("test1.traj")), JSON->stringify(trajData1));
                fs::writeFileSync(path->join(tmpDir, std:("test2.traj")), JSON->stringify(trajData2));
                fs::writeFileSync(path->join(tmpDir, std:("test3.traj")), JSON->stringify(trajData3));
                auto result = std::async([=]() { quickStats(tmpDir); });
                expect(result)->toContain(std:("## "success""));
                expect(result)->toContain(std:("## "exit_cost""));
                expect(result)->toMatch((new RegExp(std:(""success".*2 trajectorie"))));
                expect(result)->toMatch((new RegExp(std:(""exit_cost".*1 trajectorie"))));
            }
            );
            it(std:("should handle malformed trajectory files gracefully"), [=]() mutable
            {
                auto malformedFile = path->join(tmpDir, std:("malformed.traj"));
                fs::writeFileSync(malformedFile, std:("not valid json"));
                auto validFile = path->join(tmpDir, std:("valid.traj"));
                auto validData = object{
                    object::pair{std:("info"), object{
                        object::pair{std:("model_stats"), object{
                            object::pair{std:("api_calls"), 5}
                        }}, 
                        object::pair{std:("exit_status"), std:("success")}
                    }}
                };
                fs::writeFileSync(validFile, JSON->stringify(validData));
                auto result = std::async([=]() { quickStats(tmpDir); });
                expect(result)->toContain(std:("## "success""));
            }
            );
            it(std:("should handle trajectory files without model_stats"), [=]() mutable
            {
                auto trajData = object{
                    object::pair{std:("info"), object{
                        object::pair{std:("exit_status"), std:("submitted")}
                    }}
                };
                fs::writeFileSync(path->join(tmpDir, std:("test.traj")), JSON->stringify(trajData));
                auto result = std::async([=]() { quickStats(tmpDir); });
                expect(result)->toContain(std:("## "submitted""));
            }
            );
            it(std:("should handle trajectory files without exit_status"), [=]() mutable
            {
                auto trajData = object{
                    object::pair{std:("info"), object{
                        object::pair{std:("model_stats"), object{
                            object::pair{std:("api_calls"), 10}
                        }}
                    }}
                };
                fs::writeFileSync(path->join(tmpDir, std:("test.traj")), JSON->stringify(trajData));
                auto result = std::async([=]() { quickStats(tmpDir); });
                expect(result)->not->toBe(std:("No .traj files found."));
            }
            );
            it(std:("should calculate statistics correctly"), [=]() mutable
            {
                auto trajData1 = object{
                    object::pair{std:("info"), object{
                        object::pair{std:("model_stats"), object{
                            object::pair{std:("api_calls"), 10}, 
                            object::pair{std:("tokens_sent"), 1000}, 
                            object::pair{std:("tokens_received"), 500}, 
                            object::pair{std:("instance_cost"), 0.05}
                        }}, 
                        object::pair{std:("exit_status"), std:("success")}
                    }}
                };
                auto trajData2 = object{
                    object::pair{std:("info"), object{
                        object::pair{std:("model_stats"), object{
                            object::pair{std:("api_calls"), 20}, 
                            object::pair{std:("tokens_sent"), 2000}, 
                            object::pair{std:("tokens_received"), 1000}, 
                            object::pair{std:("instance_cost"), 0.1}
                        }}, 
                        object::pair{std:("exit_status"), std:("success")}
                    }}
                };
                fs::writeFileSync(path->join(tmpDir, std:("test1.traj")), JSON->stringify(trajData1));
                fs::writeFileSync(path->join(tmpDir, std:("test2.traj")), JSON->stringify(trajData2));
                auto result = std::async([=]() { quickStats(tmpDir); });
                expect(result)->toContain(std:("success"));
            }
            );
            it(std:("should handle nested directories"), [=]() mutable
            {
                auto nestedDir = path->join(tmpDir, std:("nested"), std:("deep"));
                fs::mkdirSync(nestedDir, object{
                    object::pair{std:("recursive"), true}
                });
                auto trajData = object{
                    object::pair{std:("info"), object{
                        object::pair{std:("model_stats"), object{
                            object::pair{std:("api_calls"), 5}
                        }}, 
                        object::pair{std:("exit_status"), std:("success")}
                    }}
                };
                fs::writeFileSync(path->join(tmpDir, std:("root.traj")), JSON->stringify(trajData));
                fs::writeFileSync(path->join(nestedDir, std:("nested.traj")), JSON->stringify(trajData));
                auto result = std::async([=]() { quickStats(tmpDir); });
                expect(result)->toContain(std:("## "success""));
                expect(result)->toMatch((new RegExp(std:("2 trajectorie"))));
            }
            );
            it(std:("should sort exit statuses consistently"), [=]() mutable
            {
                auto statuses = array<string>{ std:("exit_cost"), std:("success"), std:("submitted"), std:("exit_format"), std:("exit_context") };
                statuses->forEach([=](auto status, auto index) mutable
                {
                    auto trajData = object{
                        object::pair{std:("info"), object{
                            object::pair{std:("model_stats"), object{
                                object::pair{std:("api_calls"), index + 1}
                            }}, 
                            object::pair{std:("exit_status"), status}
                        }}
                    };
                    fs::writeFileSync(path->join(tmpDir, std:("test") + index + std:(".traj")), JSON->stringify(trajData));
                }
                );
                shared result = std::async([=]() { quickStats(tmpDir); });
                statuses->forEach([=](auto status) mutable
                {
                    expect(result)->toContain(std:("## "") + status + std:("""));
                }
                );
                auto statusOrder = result->match((new RegExp(std:("## "([^"]+)`"))));
                expect(statusOrder)->toBeDefined();
                expect(statusOrder->length)->toBe(statuses->get_length());
            }
            );
            it(std:("should handle special characters in exit status"), [=]() mutable
            {
                auto trajData = object{
                    object::pair{std:("info"), object{
                        object::pair{std:("model_stats"), object{
                            object::pair{std:("api_calls"), 5}
                        }}, 
                        object::pair{std:("exit_status"), std:("submitted (exit_cost)")}
                    }}
                };
                fs::writeFileSync(path->join(tmpDir, std:("test.traj")), JSON->stringify(trajData));
                auto result = std::async([=]() { quickStats(tmpDir); });
                expect(result)->toContain(std:("submitted (exit_cost)"));
            }
            );
            it(std:("should provide meaningful output format"), [=]() mutable
            {
                auto trajData = object{
                    object::pair{std:("info"), object{
                        object::pair{std:("model_stats"), object{
                            object::pair{std:("api_calls"), 10}, 
                            object::pair{std:("tokens_sent"), 1000}, 
                            object::pair{std:("tokens_received"), 500}, 
                            object::pair{std:("instance_cost"), 0.05}
                        }}, 
                        object::pair{std:("exit_status"), std:("success")}
                    }}
                };
                fs::writeFileSync(path->join(tmpDir, std:("test1.traj")), JSON->stringify(trajData));
                fs::writeFileSync(path->join(tmpDir, std:("test2.traj")), JSON->stringify(trajData));
                auto result = std::async([=]() { quickStats(tmpDir); });
                expect(result)->toMatch((new RegExp(std:("#"))));
                expect(result)->toContain(std:("trajectories"));
            }
            );
        }
        );
    }
    );
}

MAIN
