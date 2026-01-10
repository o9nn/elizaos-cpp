#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/tests/test-quick-stats.test.h"

void Main(void)
{
    describe(std::string("Quick Stats"), [=]() mutable
    {
        shared<string> tmpDir;
        beforeEach([=]() mutable
        {
            tmpDir = fs::mkdtempSync(path->join(os::tmpdir(), std::string("quick-stats-test-")));
        }
        );
        afterEach([=]() mutable
        {
            if (fs::existsSync(tmpDir)) {
                fs::rmSync(tmpDir, object{
                    object::pair{std::string("recursive"), true}
                });
            }
        }
        );
        describe(std::string("quickStats function"), [=]() mutable
        {
            it(std::string("should handle empty directories"), [=]() mutable
            {
                auto result = std::async([=]() { quickStats(tmpDir); });
                expect(result)->toBe(std::string("No .traj files found."));
            }
            );
            it(std::string("should process trajectory files correctly"), [=]() mutable
            {
                auto trajFile = path->join(tmpDir, std::string("test.traj"));
                auto trajData = object{
                    object::pair{std::string("info"), object{
                        object::pair{std::string("model_stats"), object{
                            object::pair{std::string("api_calls"), 42}, 
                            object::pair{std::string("tokens_sent"), 1000}, 
                            object::pair{std::string("tokens_received"), 500}, 
                            object::pair{std::string("instance_cost"), 0.05}
                        }}, 
                        object::pair{std::string("exit_status"), std::string("success")}
                    }}
                };
                fs::writeFileSync(trajFile, JSON->stringify(trajData));
                auto result = std::async([=]() { quickStats(tmpDir); });
                expect(result)->toContain(std::string("## `success`"));
                expect(result)->not->toBe(std::string("No .traj files found."));
            }
            );
            it(std::string("should handle multiple trajectory files"), [=]() mutable
            {
                auto trajData1 = object{
                    object::pair{std::string("info"), object{
                        object::pair{std::string("model_stats"), object{
                            object::pair{std::string("api_calls"), 10}, 
                            object::pair{std::string("tokens_sent"), 500}, 
                            object::pair{std::string("tokens_received"), 200}, 
                            object::pair{std::string("instance_cost"), 0.02}
                        }}, 
                        object::pair{std::string("exit_status"), std::string("success")}
                    }}
                };
                auto trajData2 = object{
                    object::pair{std::string("info"), object{
                        object::pair{std::string("model_stats"), object{
                            object::pair{std::string("api_calls"), 5}, 
                            object::pair{std::string("tokens_sent"), 300}, 
                            object::pair{std::string("tokens_received"), 150}, 
                            object::pair{std::string("instance_cost"), 0.01}
                        }}, 
                        object::pair{std::string("exit_status"), std::string("exit_cost")}
                    }}
                };
                auto trajData3 = object{
                    object::pair{std::string("info"), object{
                        object::pair{std::string("model_stats"), object{
                            object::pair{std::string("api_calls"), 20}, 
                            object::pair{std::string("tokens_sent"), 1500}, 
                            object::pair{std::string("tokens_received"), 800}, 
                            object::pair{std::string("instance_cost"), 0.08}
                        }}, 
                        object::pair{std::string("exit_status"), std::string("success")}
                    }}
                };
                fs::writeFileSync(path->join(tmpDir, std::string("test1.traj")), JSON->stringify(trajData1));
                fs::writeFileSync(path->join(tmpDir, std::string("test2.traj")), JSON->stringify(trajData2));
                fs::writeFileSync(path->join(tmpDir, std::string("test3.traj")), JSON->stringify(trajData3));
                auto result = std::async([=]() { quickStats(tmpDir); });
                expect(result)->toContain(std::string("## `success`"));
                expect(result)->toContain(std::string("## `exit_cost`"));
                expect(result)->toMatch((new RegExp(std::string("`success`.*2 trajectorie"))));
                expect(result)->toMatch((new RegExp(std::string("`exit_cost`.*1 trajectorie"))));
            }
            );
            it(std::string("should handle malformed trajectory files gracefully"), [=]() mutable
            {
                auto malformedFile = path->join(tmpDir, std::string("malformed.traj"));
                fs::writeFileSync(malformedFile, std::string("not valid json"));
                auto validFile = path->join(tmpDir, std::string("valid.traj"));
                auto validData = object{
                    object::pair{std::string("info"), object{
                        object::pair{std::string("model_stats"), object{
                            object::pair{std::string("api_calls"), 5}
                        }}, 
                        object::pair{std::string("exit_status"), std::string("success")}
                    }}
                };
                fs::writeFileSync(validFile, JSON->stringify(validData));
                auto result = std::async([=]() { quickStats(tmpDir); });
                expect(result)->toContain(std::string("## `success`"));
            }
            );
            it(std::string("should handle trajectory files without model_stats"), [=]() mutable
            {
                auto trajData = object{
                    object::pair{std::string("info"), object{
                        object::pair{std::string("exit_status"), std::string("submitted")}
                    }}
                };
                fs::writeFileSync(path->join(tmpDir, std::string("test.traj")), JSON->stringify(trajData));
                auto result = std::async([=]() { quickStats(tmpDir); });
                expect(result)->toContain(std::string("## `submitted`"));
            }
            );
            it(std::string("should handle trajectory files without exit_status"), [=]() mutable
            {
                auto trajData = object{
                    object::pair{std::string("info"), object{
                        object::pair{std::string("model_stats"), object{
                            object::pair{std::string("api_calls"), 10}
                        }}
                    }}
                };
                fs::writeFileSync(path->join(tmpDir, std::string("test.traj")), JSON->stringify(trajData));
                auto result = std::async([=]() { quickStats(tmpDir); });
                expect(result)->not->toBe(std::string("No .traj files found."));
            }
            );
            it(std::string("should calculate statistics correctly"), [=]() mutable
            {
                auto trajData1 = object{
                    object::pair{std::string("info"), object{
                        object::pair{std::string("model_stats"), object{
                            object::pair{std::string("api_calls"), 10}, 
                            object::pair{std::string("tokens_sent"), 1000}, 
                            object::pair{std::string("tokens_received"), 500}, 
                            object::pair{std::string("instance_cost"), 0.05}
                        }}, 
                        object::pair{std::string("exit_status"), std::string("success")}
                    }}
                };
                auto trajData2 = object{
                    object::pair{std::string("info"), object{
                        object::pair{std::string("model_stats"), object{
                            object::pair{std::string("api_calls"), 20}, 
                            object::pair{std::string("tokens_sent"), 2000}, 
                            object::pair{std::string("tokens_received"), 1000}, 
                            object::pair{std::string("instance_cost"), 0.1}
                        }}, 
                        object::pair{std::string("exit_status"), std::string("success")}
                    }}
                };
                fs::writeFileSync(path->join(tmpDir, std::string("test1.traj")), JSON->stringify(trajData1));
                fs::writeFileSync(path->join(tmpDir, std::string("test2.traj")), JSON->stringify(trajData2));
                auto result = std::async([=]() { quickStats(tmpDir); });
                expect(result)->toContain(std::string("success"));
            }
            );
            it(std::string("should handle nested directories"), [=]() mutable
            {
                auto nestedDir = path->join(tmpDir, std::string("nested"), std::string("deep"));
                fs::mkdirSync(nestedDir, object{
                    object::pair{std::string("recursive"), true}
                });
                auto trajData = object{
                    object::pair{std::string("info"), object{
                        object::pair{std::string("model_stats"), object{
                            object::pair{std::string("api_calls"), 5}
                        }}, 
                        object::pair{std::string("exit_status"), std::string("success")}
                    }}
                };
                fs::writeFileSync(path->join(tmpDir, std::string("root.traj")), JSON->stringify(trajData));
                fs::writeFileSync(path->join(nestedDir, std::string("nested.traj")), JSON->stringify(trajData));
                auto result = std::async([=]() { quickStats(tmpDir); });
                expect(result)->toContain(std::string("## `success`"));
                expect(result)->toMatch((new RegExp(std::string("2 trajectorie"))));
            }
            );
            it(std::string("should sort exit statuses consistently"), [=]() mutable
            {
                auto statuses = array<string>{ std::string("exit_cost"), std::string("success"), std::string("submitted"), std::string("exit_format"), std::string("exit_context") };
                statuses->forEach([=](auto status, auto index) mutable
                {
                    auto trajData = object{
                        object::pair{std::string("info"), object{
                            object::pair{std::string("model_stats"), object{
                                object::pair{std::string("api_calls"), index + 1}
                            }}, 
                            object::pair{std::string("exit_status"), status}
                        }}
                    };
                    fs::writeFileSync(path->join(tmpDir, std::string("test") + index + std::string(".traj")), JSON->stringify(trajData));
                }
                );
                shared result = std::async([=]() { quickStats(tmpDir); });
                statuses->forEach([=](auto status) mutable
                {
                    expect(result)->toContain(std::string("## `") + status + std::string("`"));
                }
                );
                auto statusOrder = result->match((new RegExp(std::string("## `([^`]+)`"))));
                expect(statusOrder)->toBeDefined();
                expect(statusOrder->length)->toBe(statuses->get_length());
            }
            );
            it(std::string("should handle special characters in exit status"), [=]() mutable
            {
                auto trajData = object{
                    object::pair{std::string("info"), object{
                        object::pair{std::string("model_stats"), object{
                            object::pair{std::string("api_calls"), 5}
                        }}, 
                        object::pair{std::string("exit_status"), std::string("submitted (exit_cost)")}
                    }}
                };
                fs::writeFileSync(path->join(tmpDir, std::string("test.traj")), JSON->stringify(trajData));
                auto result = std::async([=]() { quickStats(tmpDir); });
                expect(result)->toContain(std::string("submitted (exit_cost)"));
            }
            );
            it(std::string("should provide meaningful output format"), [=]() mutable
            {
                auto trajData = object{
                    object::pair{std::string("info"), object{
                        object::pair{std::string("model_stats"), object{
                            object::pair{std::string("api_calls"), 10}, 
                            object::pair{std::string("tokens_sent"), 1000}, 
                            object::pair{std::string("tokens_received"), 500}, 
                            object::pair{std::string("instance_cost"), 0.05}
                        }}, 
                        object::pair{std::string("exit_status"), std::string("success")}
                    }}
                };
                fs::writeFileSync(path->join(tmpDir, std::string("test1.traj")), JSON->stringify(trajData));
                fs::writeFileSync(path->join(tmpDir, std::string("test2.traj")), JSON->stringify(trajData));
                auto result = std::async([=]() { quickStats(tmpDir); });
                expect(result)->toMatch((new RegExp(std::string("#"))));
                expect(result)->toContain(std::string("trajectories"));
            }
            );
        }
        );
    }
    );
}

MAIN
