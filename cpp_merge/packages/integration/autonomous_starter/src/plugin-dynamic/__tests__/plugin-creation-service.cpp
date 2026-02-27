#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-dynamic/__tests__/plugin-creation-service.test.h"

std::function<any()> createMockRuntime = [=]() mutable
{
    auto runtime = as<any>(object{
        object::pair{std:("getSetting"), vi->fn()}, 
        object::pair{std:("services"), std::make_shared<Map>()}
    });
    return runtime;
};
std::function<object()> createMockChildProcess = [=]() mutable
{
    return (object{
        object::pair{std:("stdout"), object{
            object::pair{std:("on"), vi->fn()}
        }}, 
        object::pair{std:("stderr"), object{
            object::pair{std:("on"), vi->fn()}
        }}, 
        object::pair{std:("on"), vi->fn()}, 
        object::pair{std:("kill"), vi->fn()}, 
        object::pair{std:("killed"), false}
    });
};

void Main(void)
{
    vi->mock(std:("fs-extra"), [=]() mutable
    {
        auto fsMethods = object{
            object::pair{std:("ensureDir"), vi->fn()}, 
            object::pair{std:("writeJson"), vi->fn()}, 
            object::pair{std:("writeFile"), vi->fn()}, 
            object::pair{std:("remove"), vi->fn()}, 
            object::pair{std:("readdir"), vi->fn()}, 
            object::pair{std:("readFile"), vi->fn()}, 
            object::pair{std:("pathExists"), vi->fn()}
        };
        return utils::assign(object{
            object::pair{std:("default"), fsMethods}
        }, fsMethods);
    }
    );
    vi->mock(std:("child_process"));
    vi->mock(std:("@anthropic-ai/sdk"));
    describe(std:("PluginCreationService"), [=]() mutable
    {
        shared<std::shared_ptr<PluginCreationService>> service;
        shared<std::shared_ptr<IAgentRuntime>> runtime;
        shared<any> mockFs;
        shared<std::shared_ptr<Mock>> mockSpawn;
        shared<std::shared_ptr<Mock>> mockAnthropicCreate;
        beforeEach([=]() mutable
        {
            runtime = createMockRuntime();
            service = std::make_shared<PluginCreationService>(runtime);
            mockFs = as<any>(fs);
            vi->mocked(fs->ensureDir)->mockResolvedValue(undefined);
            vi->mocked(fs->writeJson)->mockResolvedValue(undefined);
            vi->mocked(fs->writeFile)->mockResolvedValue(undefined);
            vi->mocked(fs->remove)->mockResolvedValue(undefined);
            vi->mocked(fs->readdir)->mockResolvedValue(array<any>());
            vi->mocked(fs->readFile)->mockResolvedValue(string_empty);
            vi->mocked(fs->pathExists)->mockResolvedValue(false);
            mockSpawn = as<std::shared_ptr<Mock>>(as<any>(spawn));
            mockSpawn->mockReturnValue(createMockChildProcess());
            mockAnthropicCreate = vi->fn()->mockResolvedValue(object{
                object::pair{std:("content"), array<object>{ object{
                    object::pair{std:("type"), std:("text")}, 
                    object::pair{std:("text"), std:("Generated code")}
                } }}
            });
            (as<any>(Anthropic))["mockImplementation"]([=]() mutable
            {
                return (object{
                    object::pair{std:("messages"), object{
                        object::pair{std:("create"), mockAnthropicCreate}
                    }}
                });
            }
            );
            vi->clearAllTimers();
            vi->useFakeTimers();
        }
        );
        afterEach([=]() mutable
        {
            vi->clearAllMocks();
            vi->useRealTimers();
        }
        );
        describe(std:("initialization"), [=]() mutable
        {
            it(std:("should initialize without API key"), [=]() mutable
            {
                std::async([=]() { service->initialize(runtime); });
                expect(runtime->getSetting)->toHaveBeenCalledWith(std:("ANTHROPIC_API_KEY"));
            }
            );
            it(std:("should initialize with API key"), [=]() mutable
            {
                (as<any>(runtime->getSetting))["mockReturnValue"](std:("test-api-key"));
                std::async([=]() { service->initialize(runtime); });
                expect(runtime->getSetting)->toHaveBeenCalledWith(std:("ANTHROPIC_API_KEY"));
                expect(Anthropic)->toHaveBeenCalledWith(object{
                    object::pair{std:("apiKey"), std:("test-api-key")}
                });
            }
            );
        }
        );
        describe(std:("createPlugin"), [=]() mutable
        {
            shared validSpecification = object{
                object::pair{std:("name"), std:("@test/plugin-example")}, 
                object::pair{std:("description"), std:("Test plugin for unit tests")}, 
                object::pair{std:("version"), std:("1.0.0")}, 
                object::pair{std:("actions"), array<object>{ object{
                    object::pair{std:("name"), std:("testAction")}, 
                    object::pair{std:("description"), std:("A test action")}
                } }}
            };
            it(std:("should create a new plugin job"), [=]() mutable
            {
                auto jobId = std::async([=]() { service->createPlugin(validSpecification, std:("test-api-key")); });
                expect(jobId)->toBeDefined();
                expect(type_of(jobId))->toBe(std:("string"));
                expect(jobId)->toMatch((new RegExp(std:("^[a-f0-9-]{36}"))));
                auto job = service->getJobStatus(jobId);
                expect(job)->toBeDefined();
                expect(job->specification)->toEqual(validSpecification);
                expect(job->status)->toBe(std:("pending"));
            }
            );
            it(std:("should reject invalid plugin names"), [=]() mutable
            {
                auto invalidSpecs = array<object>{ utils::assign(object{
                    , 
                    object::pair{std:("name"), std:("../../../etc/passwd")}
                }, validSpecification), utils::assign(object{
                    , 
                    object::pair{std:("name"), std:("plugin\..\windows")}
                }, validSpecification), utils::assign(object{
                    , 
                    object::pair{std:("name"), std:("./hidden/plugin")}
                }, validSpecification), utils::assign(object{
                    , 
                    object::pair{std:("name"), std:("invalid plugin name")}
                }, validSpecification) };
                for (auto& spec : invalidSpecs)
                {
                    std::async([=]() { expect(service->createPlugin(spec))->rejects->toThrow(std:("Invalid plugin name")); });
                }
            }
            );
            it(std:("should enforce rate limiting"), [=]() mutable
            {
                for (auto i = 0; i < 10; i++)
                {
                    std::async([=]() { service->createPlugin(utils::assign(object{
                        , 
                        object::pair{std:("name"), std:("@test/plugin-") + i + string_empty}
                    }, validSpecification)); });
                }
                std::async([=]() { expect(service->createPlugin(validSpecification))->rejects->toThrow(std:("Rate limit exceeded")); });
            }
            );
            it(std:("should enforce concurrent job limit"), [=]() mutable
            {
                for (auto i = 0; i < 10; i++)
                {
                    std::async([=]() { service->createPlugin(utils::assign(object{
                        , 
                        object::pair{std:("name"), std:("@test/plugin-") + i + string_empty}
                    }, validSpecification)); });
                }
                std::async([=]() { expect(service->createPlugin(utils::assign(object{
                    , 
                    object::pair{std:("name"), std:("@test/plugin-11")}
                }, validSpecification)))->rejects->toThrow(std:("Rate limit exceeded")); });
            }
            );
            it(std:("should timeout long-running jobs"), [=]() mutable
            {
                auto jobId = std::async([=]() { service->createPlugin(validSpecification); });
                auto job = service->getJobStatus(jobId);
                expect(job->status)->toBe(std:("pending"));
                vi->advanceTimersByTime(30 * 60 * 1000);
                auto timedOutJob = service->getJobStatus(jobId);
                expect(timedOutJob->status)->toBe(std:("failed"));
                expect(timedOutJob->error)->toContain(std:("timed out"));
            }
            );
        }
        );
        describe(std:("job management"), [=]() mutable
        {
            it(std:("should get all jobs"), [=]() mutable
            {
                auto spec1 = object{
                    object::pair{std:("name"), std:("@test/plugin1")}, 
                    object::pair{std:("description"), std:("Plugin 1")}
                };
                auto spec2 = object{
                    object::pair{std:("name"), std:("@test/plugin2")}, 
                    object::pair{std:("description"), std:("Plugin 2")}
                };
                auto jobId1 = std::async([=]() { service->createPlugin(spec1); });
                auto jobId2 = std::async([=]() { service->createPlugin(spec2); });
                auto jobs = service->getAllJobs();
                expect(jobs)->toHaveLength(2);
                expect(jobs->map([=](auto j) mutable
                {
                    return j->id;
                }
                ))->toContain(jobId1);
                expect(jobs->map([=](auto j) mutable
                {
                    return j->id;
                }
                ))->toContain(jobId2);
            }
            );
            it(std:("should cancel a job and kill process"), [=]() mutable
            {
                auto specification = object{
                    object::pair{std:("name"), std:("@test/plugin")}, 
                    object::pair{std:("description"), std:("Test plugin")}
                };
                auto jobId = std::async([=]() { service->createPlugin(specification); });
                auto job = service->getJobStatus(jobId);
                auto mockChildProcess = object{
                    object::pair{std:("kill"), vi->fn()}, 
                    object::pair{std:("killed"), false}
                };
                if (job) {
                    job->childProcess = mockChildProcess;
                    job->status = std:("running");
                }
                service->cancelJob(jobId);
                auto cancelledJob = service->getJobStatus(jobId);
                expect(cancelledJob->status)->toBe(std:("cancelled"));
                expect(cancelledJob->completedAt)->toBeDefined();
                expect(mockChildProcess["kill"])->toHaveBeenCalledWith(std:("SIGTERM"));
            }
            );
            it(std:("should handle cancelling non-existent job"), [=]() mutable
            {
                expect([=]() mutable
                {
                    return service->cancelJob(std:("non-existent-id"));
                }
                )->not->toThrow();
            }
            );
        }
        );
        describe(std:("service lifecycle"), [=]() mutable
        {
            it(std:("should stop service and cancel running jobs"), [=]() mutable
            {
                auto specification = object{
                    object::pair{std:("name"), std:("@test/plugin")}, 
                    object::pair{std:("description"), std:("Test plugin")}
                };
                auto jobId = std::async([=]() { service->createPlugin(specification); });
                auto job = service->getJobStatus(jobId);
                if (job) {
                    job->status = std:("running");
                }
                std::async([=]() { service->stop(); });
                auto stoppedJob = service->getJobStatus(jobId);
                expect(stoppedJob->status)->toBe(std:("cancelled"));
            }
            );
        }
        );
        describe(std:("static start method"), [=]() mutable
        {
            it(std:("should create and initialize service"), [=]() mutable
            {
                auto newService = std::async([=]() { PluginCreationService::start(runtime); });
                expect(newService)->toBeInstanceOf(PluginCreationService);
            }
            );
        }
        );
        describe(std:("cleanupOldJobs"), [=]() mutable
        {
            it(std:("should remove jobs older than one week"), [=]() mutable
            {
                auto oldDate = std::make_shared<Date>(Date->now() - 8 * 24 * 60 * 60 * 1000);
                auto recentDate = std::make_shared<Date>(Date->now() - 2 * 24 * 60 * 60 * 1000);
                auto oldJobId = std::async([=]() { service->createPlugin(object{
                    object::pair{std:("name"), std:("@test/old-plugin")}, 
                    object::pair{std:("description"), std:("Old")}
                }); });
                auto oldJob = service->getJobStatus(oldJobId);
                if (oldJob) {
                    oldJob->completedAt = oldDate;
                    oldJob->status = std:("completed");
                }
                auto recentJobId = std::async([=]() { service->createPlugin(object{
                    object::pair{std:("name"), std:("@test/recent-plugin")}, 
                    object::pair{std:("description"), std:("Recent")}
                }); });
                auto recentJob = service->getJobStatus(recentJobId);
                if (recentJob) {
                    recentJob->completedAt = recentDate;
                    recentJob->status = std:("completed");
                }
                service->cleanupOldJobs();
                expect(service->getJobStatus(oldJobId))->toBeNull();
                expect(service->getJobStatus(recentJobId))->toBeDefined();
                expect(fs->remove)->toHaveBeenCalled();
            }
            );
        }
        );
        describe(std:("plugin creation workflow"), [=]() mutable
        {
            it(std:("should handle successful code generation"), object{
                object::pair{std:("timeout"), 10000}
            }, [=]() mutable
            {
                (as<any>(runtime->getSetting))["mockReturnValue"](std:("test-api-key"));
                auto specification = object{
                    object::pair{std:("name"), std:("@test/plugin")}, 
                    object::pair{std:("description"), std:("Test plugin")}, 
                    object::pair{std:("actions"), array<object>{ object{
                        object::pair{std:("name"), std:("testAction")}, 
                        object::pair{std:("description"), std:("Test")}
                    } }}
                };
                auto mockChild = createMockChildProcess();
                mockChild["on"] = vi->fn([=](auto event, auto callback) mutable
                {
                    if (event == std:("close")) {
                        process->nextTick([=]() mutable
                        {
                            return callback(0);
                        }
                        );
                    }
                }
                );
                mockSpawn->mockReturnValue(mockChild);
                auto jobId = std::async([=]() { service->createPlugin(specification, std:("test-api-key")); });
                std::async([=]() { vi->advanceTimersByTimeAsync(100); });
                auto job = service->getJobStatus(jobId);
                expect(job)->toBeDefined();
                expect(mockAnthropicCreate)->toHaveBeenCalled();
                expect(fs->ensureDir)->toHaveBeenCalled();
                expect(fs->writeJson)->toHaveBeenCalled();
            }
            );
            it(std:("should handle code generation failure"), object{
                object::pair{std:("timeout"), 10000}
            }, [=]() mutable
            {
                (as<any>(runtime->getSetting))["mockReturnValue"](std:("test-api-key"));
                mockAnthropicCreate->mockRejectedValue(std::make_shared<Error>(std:("API error")));
                auto specification = object{
                    object::pair{std:("name"), std:("@test/plugin")}, 
                    object::pair{std:("description"), std:("Test plugin")}
                };
                auto jobId = std::async([=]() { service->createPlugin(specification, std:("test-api-key")); });
                std::async([=]() { vi->advanceTimersByTimeAsync(100); });
                auto job = service->getJobStatus(jobId);
                expect(job->status)->toBe(std:("failed"));
                expect(job->error)->toContain(std:("API error"));
            }
            );
            it(std:("should handle build failures"), object{
                object::pair{std:("timeout"), 10000}
            }, [=]() mutable
            {
                (as<any>(runtime->getSetting))["mockReturnValue"](std:("test-api-key"));
                auto mockChild = createMockChildProcess();
                mockChild["on"] = vi->fn([=](auto event, auto callback) mutable
                {
                    if (event == std:("close")) {
                        process->nextTick([=]() mutable
                        {
                            return callback(1);
                        }
                        );
                    }
                }
                );
                mockChild["stderr"]["on"] = vi->fn([=](auto event, auto callback) mutable
                {
                    if (event == std:("data")) {
                        callback(Buffer::from(std:("Build error")));
                    }
                }
                );
                mockSpawn->mockReturnValue(mockChild);
                auto specification = object{
                    object::pair{std:("name"), std:("@test/plugin")}, 
                    object::pair{std:("description"), std:("Test plugin")}
                };
                auto jobId = std::async([=]() { service->createPlugin(specification, std:("test-api-key")); });
                std::async([=]() { vi->advanceTimersByTimeAsync(200); });
                auto job = service->getJobStatus(jobId);
                expect(job->errors->get_length())->toBeGreaterThan(0);
            }
            );
            it(std:("should handle command timeouts"), [=]() mutable
            {
                (as<any>(runtime->getSetting))["mockReturnValue"](std:("test-api-key"));
                auto mockChild = createMockChildProcess();
                mockChild["on"] = vi->fn();
                mockSpawn->mockReturnValue(mockChild);
                auto specification = object{
                    object::pair{std:("name"), std:("@test/plugin")}, 
                    object::pair{std:("description"), std:("Test plugin")}
                };
                auto jobId = std::async([=]() { service->createPlugin(specification, std:("test-api-key")); });
                std::async([=]() { vi->advanceTimersByTimeAsync(100); });
                std::async([=]() { vi->advanceTimersByTimeAsync(5 * 60 * 1000 + 1000); });
                expect(mockChild["kill"])->toHaveBeenCalledWith(std:("SIGTERM"));
            }
            );
            it(std:("should limit output size"), object{
                object::pair{std:("timeout"), 10000}
            }, [=]() mutable
            {
                (as<any>(runtime->getSetting))["mockReturnValue"](std:("test-api-key"));
                auto mockChild = createMockChildProcess();
                mockChild["on"] = vi->fn([=](auto event, auto callback) mutable
                {
                    if (event == std:("close")) {
                        process->nextTick([=]() mutable
                        {
                            return callback(0);
                        }
                        );
                    }
                }
                );
                mockChild["stdout"]["on"] = vi->fn([=](auto event, auto callback) mutable
                {
                    if (event == std:("data")) {
                        for (auto i = 0; i < 20; i++)
                        {
                            callback(Buffer::alloc(100 * 1024, std:("a")));
                        }
                    }
                }
                );
                mockSpawn->mockReturnValue(mockChild);
                auto specification = object{
                    object::pair{std:("name"), std:("@test/plugin")}, 
                    object::pair{std:("description"), std:("Test plugin")}
                };
                auto jobId = std::async([=]() { service->createPlugin(specification, std:("test-api-key")); });
                std::async([=]() { vi->advanceTimersByTimeAsync(200); });
                auto job = service->getJobStatus(jobId);
                auto logs = OR((job->logs->join(std:("\
"))), (string_empty));
                expect(logs)->toContain(std:("Output truncated"));
            }
            );
        }
        );
        describe(std:("security"), [=]() mutable
        {
            it(std:("should sanitize plugin names"), [=]() mutable
            {
                auto specification = object{
                    object::pair{std:("name"), std:("@test/Plugin-Name_123")}, 
                    object::pair{std:("description"), std:("Test")}
                };
                auto jobId = std::async([=]() { service->createPlugin(specification); });
                auto job = service->getJobStatus(jobId);
                expect(job->outputPath)->toContain(std:("test-plugin-name_123"));
                auto pathParts = job->outputPath->split(std:("/"));
                auto sanitizedName = const_(pathParts)[pathParts->get_length() - 1];
                expect(sanitizedName)->not->toContain(std:("@"));
                expect(sanitizedName)->not->toContain(std:("/"));
            }
            );
            it(std:("should prevent shell injection in commands"), object{
                object::pair{std:("timeout"), 10000}
            }, [=]() mutable
            {
                (as<any>(runtime->getSetting))["mockReturnValue"](std:("test-api-key"));
                auto specification = object{
                    object::pair{std:("name"), std:("@test/plugin")}, 
                    object::pair{std:("description"), std:("Test; rm -rf /")}
                };
                std::async([=]() { service->createPlugin(specification, std:("test-api-key")); });
                std::async([=]() { vi->advanceTimersByTimeAsync(100); });
                expect(mockSpawn)->toHaveBeenCalledWith(expect->any(String), expect->any(Array), expect->objectContaining(object{
                    object::pair{std:("shell"), false}
                }));
            }
            );
        }
        );
    }
    );
}

MAIN
