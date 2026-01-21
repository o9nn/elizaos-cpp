#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-dynamic/__tests__/plugin-creation-service.test.h"

std::function<any()> createMockRuntime = [=]() mutable
{
    auto runtime = as<any>(object{
        object::pair{std::string("getSetting"), vi->fn()}, 
        object::pair{std::string("services"), std::make_shared<Map>()}
    });
    return runtime;
};
std::function<object()> createMockChildProcess = [=]() mutable
{
    return (object{
        object::pair{std::string("stdout"), object{
            object::pair{std::string("on"), vi->fn()}
        }}, 
        object::pair{std::string("stderr"), object{
            object::pair{std::string("on"), vi->fn()}
        }}, 
        object::pair{std::string("on"), vi->fn()}, 
        object::pair{std::string("kill"), vi->fn()}, 
        object::pair{std::string("killed"), false}
    });
};

void Main(void)
{
    vi->mock(std::string("fs-extra"), [=]() mutable
    {
        auto fsMethods = object{
            object::pair{std::string("ensureDir"), vi->fn()}, 
            object::pair{std::string("writeJson"), vi->fn()}, 
            object::pair{std::string("writeFile"), vi->fn()}, 
            object::pair{std::string("remove"), vi->fn()}, 
            object::pair{std::string("readdir"), vi->fn()}, 
            object::pair{std::string("readFile"), vi->fn()}, 
            object::pair{std::string("pathExists"), vi->fn()}
        };
        return utils::assign(object{
            object::pair{std::string("default"), fsMethods}
        }, fsMethods);
    }
    );
    vi->mock(std::string("child_process"));
    vi->mock(std::string("@anthropic-ai/sdk"));
    describe(std::string("PluginCreationService"), [=]() mutable
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
                object::pair{std::string("content"), array<object>{ object{
                    object::pair{std::string("type"), std::string("text")}, 
                    object::pair{std::string("text"), std::string("Generated code")}
                } }}
            });
            (as<any>(Anthropic))["mockImplementation"]([=]() mutable
            {
                return (object{
                    object::pair{std::string("messages"), object{
                        object::pair{std::string("create"), mockAnthropicCreate}
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
        describe(std::string("initialization"), [=]() mutable
        {
            it(std::string("should initialize without API key"), [=]() mutable
            {
                std::async([=]() { service->initialize(runtime); });
                expect(runtime->getSetting)->toHaveBeenCalledWith(std::string("ANTHROPIC_API_KEY"));
            }
            );
            it(std::string("should initialize with API key"), [=]() mutable
            {
                (as<any>(runtime->getSetting))["mockReturnValue"](std::string("test-api-key"));
                std::async([=]() { service->initialize(runtime); });
                expect(runtime->getSetting)->toHaveBeenCalledWith(std::string("ANTHROPIC_API_KEY"));
                expect(Anthropic)->toHaveBeenCalledWith(object{
                    object::pair{std::string("apiKey"), std::string("test-api-key")}
                });
            }
            );
        }
        );
        describe(std::string("createPlugin"), [=]() mutable
        {
            shared validSpecification = object{
                object::pair{std::string("name"), std::string("@test/plugin-example")}, 
                object::pair{std::string("description"), std::string("Test plugin for unit tests")}, 
                object::pair{std::string("version"), std::string("1.0.0")}, 
                object::pair{std::string("actions"), array<object>{ object{
                    object::pair{std::string("name"), std::string("testAction")}, 
                    object::pair{std::string("description"), std::string("A test action")}
                } }}
            };
            it(std::string("should create a new plugin job"), [=]() mutable
            {
                auto jobId = std::async([=]() { service->createPlugin(validSpecification, std::string("test-api-key")); });
                expect(jobId)->toBeDefined();
                expect(type_of(jobId))->toBe(std::string("string"));
                expect(jobId)->toMatch((new RegExp(std::string("^[a-f0-9-]{36}"))));
                auto job = service->getJobStatus(jobId);
                expect(job)->toBeDefined();
                expect(job->specification)->toEqual(validSpecification);
                expect(job->status)->toBe(std::string("pending"));
            }
            );
            it(std::string("should reject invalid plugin names"), [=]() mutable
            {
                auto invalidSpecs = array<object>{ utils::assign(object{
                    , 
                    object::pair{std::string("name"), std::string("../../../etc/passwd")}
                }, validSpecification), utils::assign(object{
                    , 
                    object::pair{std::string("name"), std::string("plugin\..\windows")}
                }, validSpecification), utils::assign(object{
                    , 
                    object::pair{std::string("name"), std::string("./hidden/plugin")}
                }, validSpecification), utils::assign(object{
                    , 
                    object::pair{std::string("name"), std::string("invalid plugin name")}
                }, validSpecification) };
                for (auto& spec : invalidSpecs)
                {
                    std::async([=]() { expect(service->createPlugin(spec))->rejects->toThrow(std::string("Invalid plugin name")); });
                }
            }
            );
            it(std::string("should enforce rate limiting"), [=]() mutable
            {
                for (auto i = 0; i < 10; i++)
                {
                    std::async([=]() { service->createPlugin(utils::assign(object{
                        , 
                        object::pair{std::string("name"), std::string("@test/plugin-") + i + string_empty}
                    }, validSpecification)); });
                }
                std::async([=]() { expect(service->createPlugin(validSpecification))->rejects->toThrow(std::string("Rate limit exceeded")); });
            }
            );
            it(std::string("should enforce concurrent job limit"), [=]() mutable
            {
                for (auto i = 0; i < 10; i++)
                {
                    std::async([=]() { service->createPlugin(utils::assign(object{
                        , 
                        object::pair{std::string("name"), std::string("@test/plugin-") + i + string_empty}
                    }, validSpecification)); });
                }
                std::async([=]() { expect(service->createPlugin(utils::assign(object{
                    , 
                    object::pair{std::string("name"), std::string("@test/plugin-11")}
                }, validSpecification)))->rejects->toThrow(std::string("Rate limit exceeded")); });
            }
            );
            it(std::string("should timeout long-running jobs"), [=]() mutable
            {
                auto jobId = std::async([=]() { service->createPlugin(validSpecification); });
                auto job = service->getJobStatus(jobId);
                expect(job->status)->toBe(std::string("pending"));
                vi->advanceTimersByTime(30 * 60 * 1000);
                auto timedOutJob = service->getJobStatus(jobId);
                expect(timedOutJob->status)->toBe(std::string("failed"));
                expect(timedOutJob->error)->toContain(std::string("timed out"));
            }
            );
        }
        );
        describe(std::string("job management"), [=]() mutable
        {
            it(std::string("should get all jobs"), [=]() mutable
            {
                auto spec1 = object{
                    object::pair{std::string("name"), std::string("@test/plugin1")}, 
                    object::pair{std::string("description"), std::string("Plugin 1")}
                };
                auto spec2 = object{
                    object::pair{std::string("name"), std::string("@test/plugin2")}, 
                    object::pair{std::string("description"), std::string("Plugin 2")}
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
            it(std::string("should cancel a job and kill process"), [=]() mutable
            {
                auto specification = object{
                    object::pair{std::string("name"), std::string("@test/plugin")}, 
                    object::pair{std::string("description"), std::string("Test plugin")}
                };
                auto jobId = std::async([=]() { service->createPlugin(specification); });
                auto job = service->getJobStatus(jobId);
                auto mockChildProcess = object{
                    object::pair{std::string("kill"), vi->fn()}, 
                    object::pair{std::string("killed"), false}
                };
                if (job) {
                    job->childProcess = mockChildProcess;
                    job->status = std::string("running");
                }
                service->cancelJob(jobId);
                auto cancelledJob = service->getJobStatus(jobId);
                expect(cancelledJob->status)->toBe(std::string("cancelled"));
                expect(cancelledJob->completedAt)->toBeDefined();
                expect(mockChildProcess["kill"])->toHaveBeenCalledWith(std::string("SIGTERM"));
            }
            );
            it(std::string("should handle cancelling non-existent job"), [=]() mutable
            {
                expect([=]() mutable
                {
                    return service->cancelJob(std::string("non-existent-id"));
                }
                )->not->toThrow();
            }
            );
        }
        );
        describe(std::string("service lifecycle"), [=]() mutable
        {
            it(std::string("should stop service and cancel running jobs"), [=]() mutable
            {
                auto specification = object{
                    object::pair{std::string("name"), std::string("@test/plugin")}, 
                    object::pair{std::string("description"), std::string("Test plugin")}
                };
                auto jobId = std::async([=]() { service->createPlugin(specification); });
                auto job = service->getJobStatus(jobId);
                if (job) {
                    job->status = std::string("running");
                }
                std::async([=]() { service->stop(); });
                auto stoppedJob = service->getJobStatus(jobId);
                expect(stoppedJob->status)->toBe(std::string("cancelled"));
            }
            );
        }
        );
        describe(std::string("static start method"), [=]() mutable
        {
            it(std::string("should create and initialize service"), [=]() mutable
            {
                auto newService = std::async([=]() { PluginCreationService::start(runtime); });
                expect(newService)->toBeInstanceOf(PluginCreationService);
            }
            );
        }
        );
        describe(std::string("cleanupOldJobs"), [=]() mutable
        {
            it(std::string("should remove jobs older than one week"), [=]() mutable
            {
                auto oldDate = std::make_shared<Date>(Date->now() - 8 * 24 * 60 * 60 * 1000);
                auto recentDate = std::make_shared<Date>(Date->now() - 2 * 24 * 60 * 60 * 1000);
                auto oldJobId = std::async([=]() { service->createPlugin(object{
                    object::pair{std::string("name"), std::string("@test/old-plugin")}, 
                    object::pair{std::string("description"), std::string("Old")}
                }); });
                auto oldJob = service->getJobStatus(oldJobId);
                if (oldJob) {
                    oldJob->completedAt = oldDate;
                    oldJob->status = std::string("completed");
                }
                auto recentJobId = std::async([=]() { service->createPlugin(object{
                    object::pair{std::string("name"), std::string("@test/recent-plugin")}, 
                    object::pair{std::string("description"), std::string("Recent")}
                }); });
                auto recentJob = service->getJobStatus(recentJobId);
                if (recentJob) {
                    recentJob->completedAt = recentDate;
                    recentJob->status = std::string("completed");
                }
                service->cleanupOldJobs();
                expect(service->getJobStatus(oldJobId))->toBeNull();
                expect(service->getJobStatus(recentJobId))->toBeDefined();
                expect(fs->remove)->toHaveBeenCalled();
            }
            );
        }
        );
        describe(std::string("plugin creation workflow"), [=]() mutable
        {
            it(std::string("should handle successful code generation"), object{
                object::pair{std::string("timeout"), 10000}
            }, [=]() mutable
            {
                (as<any>(runtime->getSetting))["mockReturnValue"](std::string("test-api-key"));
                auto specification = object{
                    object::pair{std::string("name"), std::string("@test/plugin")}, 
                    object::pair{std::string("description"), std::string("Test plugin")}, 
                    object::pair{std::string("actions"), array<object>{ object{
                        object::pair{std::string("name"), std::string("testAction")}, 
                        object::pair{std::string("description"), std::string("Test")}
                    } }}
                };
                auto mockChild = createMockChildProcess();
                mockChild["on"] = vi->fn([=](auto event, auto callback) mutable
                {
                    if (event == std::string("close")) {
                        process->nextTick([=]() mutable
                        {
                            return callback(0);
                        }
                        );
                    }
                }
                );
                mockSpawn->mockReturnValue(mockChild);
                auto jobId = std::async([=]() { service->createPlugin(specification, std::string("test-api-key")); });
                std::async([=]() { vi->advanceTimersByTimeAsync(100); });
                auto job = service->getJobStatus(jobId);
                expect(job)->toBeDefined();
                expect(mockAnthropicCreate)->toHaveBeenCalled();
                expect(fs->ensureDir)->toHaveBeenCalled();
                expect(fs->writeJson)->toHaveBeenCalled();
            }
            );
            it(std::string("should handle code generation failure"), object{
                object::pair{std::string("timeout"), 10000}
            }, [=]() mutable
            {
                (as<any>(runtime->getSetting))["mockReturnValue"](std::string("test-api-key"));
                mockAnthropicCreate->mockRejectedValue(std::make_shared<Error>(std::string("API error")));
                auto specification = object{
                    object::pair{std::string("name"), std::string("@test/plugin")}, 
                    object::pair{std::string("description"), std::string("Test plugin")}
                };
                auto jobId = std::async([=]() { service->createPlugin(specification, std::string("test-api-key")); });
                std::async([=]() { vi->advanceTimersByTimeAsync(100); });
                auto job = service->getJobStatus(jobId);
                expect(job->status)->toBe(std::string("failed"));
                expect(job->error)->toContain(std::string("API error"));
            }
            );
            it(std::string("should handle build failures"), object{
                object::pair{std::string("timeout"), 10000}
            }, [=]() mutable
            {
                (as<any>(runtime->getSetting))["mockReturnValue"](std::string("test-api-key"));
                auto mockChild = createMockChildProcess();
                mockChild["on"] = vi->fn([=](auto event, auto callback) mutable
                {
                    if (event == std::string("close")) {
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
                    if (event == std::string("data")) {
                        callback(Buffer::from(std::string("Build error")));
                    }
                }
                );
                mockSpawn->mockReturnValue(mockChild);
                auto specification = object{
                    object::pair{std::string("name"), std::string("@test/plugin")}, 
                    object::pair{std::string("description"), std::string("Test plugin")}
                };
                auto jobId = std::async([=]() { service->createPlugin(specification, std::string("test-api-key")); });
                std::async([=]() { vi->advanceTimersByTimeAsync(200); });
                auto job = service->getJobStatus(jobId);
                expect(job->errors->get_length())->toBeGreaterThan(0);
            }
            );
            it(std::string("should handle command timeouts"), [=]() mutable
            {
                (as<any>(runtime->getSetting))["mockReturnValue"](std::string("test-api-key"));
                auto mockChild = createMockChildProcess();
                mockChild["on"] = vi->fn();
                mockSpawn->mockReturnValue(mockChild);
                auto specification = object{
                    object::pair{std::string("name"), std::string("@test/plugin")}, 
                    object::pair{std::string("description"), std::string("Test plugin")}
                };
                auto jobId = std::async([=]() { service->createPlugin(specification, std::string("test-api-key")); });
                std::async([=]() { vi->advanceTimersByTimeAsync(100); });
                std::async([=]() { vi->advanceTimersByTimeAsync(5 * 60 * 1000 + 1000); });
                expect(mockChild["kill"])->toHaveBeenCalledWith(std::string("SIGTERM"));
            }
            );
            it(std::string("should limit output size"), object{
                object::pair{std::string("timeout"), 10000}
            }, [=]() mutable
            {
                (as<any>(runtime->getSetting))["mockReturnValue"](std::string("test-api-key"));
                auto mockChild = createMockChildProcess();
                mockChild["on"] = vi->fn([=](auto event, auto callback) mutable
                {
                    if (event == std::string("close")) {
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
                    if (event == std::string("data")) {
                        for (auto i = 0; i < 20; i++)
                        {
                            callback(Buffer::alloc(100 * 1024, std::string("a")));
                        }
                    }
                }
                );
                mockSpawn->mockReturnValue(mockChild);
                auto specification = object{
                    object::pair{std::string("name"), std::string("@test/plugin")}, 
                    object::pair{std::string("description"), std::string("Test plugin")}
                };
                auto jobId = std::async([=]() { service->createPlugin(specification, std::string("test-api-key")); });
                std::async([=]() { vi->advanceTimersByTimeAsync(200); });
                auto job = service->getJobStatus(jobId);
                auto logs = OR((job->logs->join(std::string("\
"))), (string_empty));
                expect(logs)->toContain(std::string("Output truncated"));
            }
            );
        }
        );
        describe(std::string("security"), [=]() mutable
        {
            it(std::string("should sanitize plugin names"), [=]() mutable
            {
                auto specification = object{
                    object::pair{std::string("name"), std::string("@test/Plugin-Name_123")}, 
                    object::pair{std::string("description"), std::string("Test")}
                };
                auto jobId = std::async([=]() { service->createPlugin(specification); });
                auto job = service->getJobStatus(jobId);
                expect(job->outputPath)->toContain(std::string("test-plugin-name_123"));
                auto pathParts = job->outputPath->split(std::string("/"));
                auto sanitizedName = const_(pathParts)[pathParts->get_length() - 1];
                expect(sanitizedName)->not->toContain(std::string("@"));
                expect(sanitizedName)->not->toContain(std::string("/"));
            }
            );
            it(std::string("should prevent shell injection in commands"), object{
                object::pair{std::string("timeout"), 10000}
            }, [=]() mutable
            {
                (as<any>(runtime->getSetting))["mockReturnValue"](std::string("test-api-key"));
                auto specification = object{
                    object::pair{std::string("name"), std::string("@test/plugin")}, 
                    object::pair{std::string("description"), std::string("Test; rm -rf /")}
                };
                std::async([=]() { service->createPlugin(specification, std::string("test-api-key")); });
                std::async([=]() { vi->advanceTimersByTimeAsync(100); });
                expect(mockSpawn)->toHaveBeenCalledWith(expect->any(String), expect->any(Array), expect->objectContaining(object{
                    object::pair{std::string("shell"), false}
                }));
            }
            );
        }
        );
    }
    );
}

MAIN
