#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/tests/test-run-hooks.test.h"

any mockedGithub = jest->mocked(github);

void Main(void)
{
    jest->mock(std::string("@octokit/rest"), [=]() mutable
    {
        return (object{
            object::pair{std::string("Octokit"), jest->fn()->mockImplementation([=]() mutable
            {
                return (object{
                    object::pair{std::string("rest"), object{
                        object::pair{std::string("pulls"), object{
                            object::pair{std::string("create"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                                object::pair{std::string("data"), object{
                                    object::pair{std::string("html_url"), std::string("https://github.com/test/repo/pull/1")}
                                }}
                            })}
                        }}, 
                        object::pair{std::string("repos"), object{
                            object::pair{std::string("get"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                                object::pair{std::string("data"), object{
                                    object::pair{std::string("default_branch"), std::string("main")}
                                }}
                            })}, 
                            object::pair{std::string("getCommit"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                                object::pair{std::string("data"), object{
                                    object::pair{std::string("commit"), object{
                                        object::pair{std::string("message"), std::string("test commit")}
                                    }}
                                }}
                            })}
                        }}
                    }}
                });
            }
            )}
        });
    }
    );
    jest->mock(std::string("../src/utils/github"));
    mockedGithub->parseGhIssueUrl->mockImplementation([=](auto url) mutable
    {
        auto match = url->match((new RegExp(std::string("github\.com\/([^/]+)\/([^/]+)\/issues\/(\d+"))));
        if (!match) return as<any>(undefined);
        return object{
            object::pair{std::string("owner"), (*const_(match))[1]}, 
            object::pair{std::string("repo"), (*const_(match))[2]}, 
            object::pair{std::string("issueNumber"), (*const_(match))[3]}
        };
    }
    );
    mockedGithub->getAssociatedCommitUrls->mockResolvedValue(array<any>());
    mockedGithub->getGhIssueData->mockResolvedValue(as<any>(object{
        object::pair{std::string("state"), std::string("open")}, 
        object::pair{std::string("locked"), false}, 
        object::pair{std::string("assignee"), nullptr}, 
        object::pair{std::string("assignees"), array<any>()}, 
        object::pair{std::string("pull_request"), nullptr}
    }));
    describe(std::string("Run Hooks"), [=]() mutable
    {
        describe(std::string("OpenPRHook"), [=]() mutable
        {
            shared<std::shared_ptr<OpenPRHook>> hook;
            shared<std::shared_ptr<AgentRunResult>> agentRunResult;
            beforeEach([=]() mutable
            {
                jest->clearAllMocks();
                process->env->GITHUB_TOKEN = std::string("test-token");
                hook = std::make_shared<OpenPRHook>(object{
                    object::pair{std::string("skipIfCommitsReferenceIssue"), true}
                });
                auto mockEnv = as<std::shared_ptr<SWEEnv>>(as<any>(object{
                    object::pair{std::string("communicate"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                        object::pair{std::string("stdout"), string_empty}, 
                        object::pair{std::string("stderr"), string_empty}, 
                        object::pair{std::string("exitCode"), 0}
                    })}, 
                    object::pair{std::string("executeCommand"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                        object::pair{std::string("stdout"), string_empty}, 
                        object::pair{std::string("stderr"), string_empty}, 
                        object::pair{std::string("exitCode"), 0}
                    })}
                }));
                auto problemStatement = object{
                    object::pair{std::string("githubUrl"), std::string("https://github.com/swe-agent/test-repo/issues/1")}
                };
                hook->onInit(object{
                    object::pair{std::string("env"), mockEnv}, 
                    object::pair{std::string("problemStatement"), std::string("problemStatement")}
                });
                agentRunResult = object{
                    object::pair{std::string("info"), object{
                        object::pair{std::string("submission"), std::string("asdf")}, 
                        object::pair{std::string("exitStatus"), std::string("submitted")}
                    }}, 
                    object::pair{std::string("trajectory"), array<any>()}
                };
            }
            );
            describe(std::string("should_open_pr checks"), [=]() mutable
            {
                it(std::string("should fail when submission is missing"), [=]() mutable
                {
                    agentRunResult->info->submission = nullptr;
                    std::async([=]() { hook->onInstanceCompleted(object{
                        object::pair{std::string("result"), agentRunResult}
                    }); });
                }
                );
                it(std::string("should fail when submission is empty"), [=]() mutable
                {
                    agentRunResult->info->submission = string_empty;
                    std::async([=]() { hook->onInstanceCompleted(object{
                        object::pair{std::string("result"), agentRunResult}
                    }); });
                }
                );
                it(std::string("should fail when exit status is not submitted"), [=]() mutable
                {
                    agentRunResult->info->exitStatus = std::string("fail");
                    std::async([=]() { hook->onInstanceCompleted(object{
                        object::pair{std::string("result"), agentRunResult}
                    }); });
                }
                );
                it(std::string("should fail when exit status indicates error"), [=]() mutable
                {
                    agentRunResult->info->exitStatus = std::string("exit_cost");
                    std::async([=]() { hook->onInstanceCompleted(object{
                        object::pair{std::string("result"), agentRunResult}
                    }); });
                }
                );
                it(std::string("should fail when invalid URL is provided"), [=]() mutable
                {
                    auto mockEnv = as<std::shared_ptr<SWEEnv>>(as<any>(object{
                        object::pair{std::string("communicate"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                            object::pair{std::string("stdout"), string_empty}, 
                            object::pair{std::string("stderr"), string_empty}, 
                            object::pair{std::string("exitCode"), 0}
                        })}, 
                        object::pair{std::string("executeCommand"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                            object::pair{std::string("stdout"), string_empty}, 
                            object::pair{std::string("stderr"), string_empty}, 
                            object::pair{std::string("exitCode"), 0}
                        })}
                    }));
                    auto problemStatement = object{
                        object::pair{std::string("githubUrl"), std::string("https://github.com/swe-agent/test-repo/issues/invalid")}
                    };
                    hook->onInit(object{
                        object::pair{std::string("env"), mockEnv}, 
                        object::pair{std::string("problemStatement"), std::string("problemStatement")}
                    });
                    mockedGithub->getGhIssueData->mockRejectedValueOnce(std::make_shared<github->InvalidGithubURL>(std::string("Invalid URL")));
                    std::async([=]() { hook->onInstanceCompleted(object{
                        object::pair{std::string("result"), agentRunResult}
                    }); });
                }
                );
                it(std::string("should fail when issue is closed"), [=]() mutable
                {
                    auto mockEnv = as<std::shared_ptr<SWEEnv>>(as<any>(object{
                        object::pair{std::string("communicate"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                            object::pair{std::string("stdout"), string_empty}, 
                            object::pair{std::string("stderr"), string_empty}, 
                            object::pair{std::string("exitCode"), 0}
                        })}, 
                        object::pair{std::string("executeCommand"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                            object::pair{std::string("stdout"), string_empty}, 
                            object::pair{std::string("stderr"), string_empty}, 
                            object::pair{std::string("exitCode"), 0}
                        })}
                    }));
                    auto problemStatement = object{
                        object::pair{std::string("githubUrl"), std::string("https://github.com/swe-agent/test-repo/issues/16")}
                    };
                    hook->onInit(object{
                        object::pair{std::string("env"), mockEnv}, 
                        object::pair{std::string("problemStatement"), std::string("problemStatement")}
                    });
                    mockedGithub->getGhIssueData->mockResolvedValueOnce(object{
                        object::pair{std::string("state"), std::string("closed")}, 
                        object::pair{std::string("locked"), false}, 
                        object::pair{std::string("assignee"), nullptr}, 
                        object::pair{std::string("assignees"), array<any>()}, 
                        object::pair{std::string("pull_request"), nullptr}
                    });
                    std::async([=]() { hook->onInstanceCompleted(object{
                        object::pair{std::string("result"), agentRunResult}
                    }); });
                    expect(mockedGithub->getGhIssueData)->toHaveBeenCalled();
                }
                );
                it(std::string("should fail when issue is assigned"), [=]() mutable
                {
                    auto mockEnv = as<std::shared_ptr<SWEEnv>>(as<any>(object{
                        object::pair{std::string("communicate"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                            object::pair{std::string("stdout"), string_empty}, 
                            object::pair{std::string("stderr"), string_empty}, 
                            object::pair{std::string("exitCode"), 0}
                        })}, 
                        object::pair{std::string("executeCommand"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                            object::pair{std::string("stdout"), string_empty}, 
                            object::pair{std::string("stderr"), string_empty}, 
                            object::pair{std::string("exitCode"), 0}
                        })}
                    }));
                    auto problemStatement = object{
                        object::pair{std::string("githubUrl"), std::string("https://github.com/swe-agent/test-repo/issues/17")}
                    };
                    hook->onInit(object{
                        object::pair{std::string("env"), mockEnv}, 
                        object::pair{std::string("problemStatement"), std::string("problemStatement")}
                    });
                    mockedGithub->getGhIssueData->mockResolvedValueOnce(object{
                        object::pair{std::string("state"), std::string("open")}, 
                        object::pair{std::string("locked"), false}, 
                        object::pair{std::string("assignee"), object{
                            object::pair{std::string("login"), std::string("someone")}
                        }}, 
                        object::pair{std::string("assignees"), array<object>{ object{
                            object::pair{std::string("login"), std::string("someone")}
                        } }}, 
                        object::pair{std::string("pull_request"), nullptr}
                    });
                    std::async([=]() { hook->onInstanceCompleted(object{
                        object::pair{std::string("result"), agentRunResult}
                    }); });
                    expect(mockedGithub->getGhIssueData)->toHaveBeenCalled();
                }
                );
                it(std::string("should fail when issue is locked"), [=]() mutable
                {
                    auto mockEnv = as<std::shared_ptr<SWEEnv>>(as<any>(object{
                        object::pair{std::string("communicate"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                            object::pair{std::string("stdout"), string_empty}, 
                            object::pair{std::string("stderr"), string_empty}, 
                            object::pair{std::string("exitCode"), 0}
                        })}, 
                        object::pair{std::string("executeCommand"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                            object::pair{std::string("stdout"), string_empty}, 
                            object::pair{std::string("stderr"), string_empty}, 
                            object::pair{std::string("exitCode"), 0}
                        })}
                    }));
                    auto problemStatement = object{
                        object::pair{std::string("githubUrl"), std::string("https://github.com/swe-agent/test-repo/issues/18")}
                    };
                    hook->onInit(object{
                        object::pair{std::string("env"), mockEnv}, 
                        object::pair{std::string("problemStatement"), std::string("problemStatement")}
                    });
                    mockedGithub->getGhIssueData->mockResolvedValueOnce(object{
                        object::pair{std::string("state"), std::string("open")}, 
                        object::pair{std::string("locked"), true}, 
                        object::pair{std::string("assignee"), nullptr}, 
                        object::pair{std::string("assignees"), array<any>()}, 
                        object::pair{std::string("pull_request"), nullptr}
                    });
                    std::async([=]() { hook->onInstanceCompleted(object{
                        object::pair{std::string("result"), agentRunResult}
                    }); });
                    expect(mockedGithub->getGhIssueData)->toHaveBeenCalled();
                }
                );
                it(std::string("should fail when issue already has PR"), [=]() mutable
                {
                    auto mockEnv = as<std::shared_ptr<SWEEnv>>(as<any>(object{
                        object::pair{std::string("communicate"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                            object::pair{std::string("stdout"), string_empty}, 
                            object::pair{std::string("stderr"), string_empty}, 
                            object::pair{std::string("exitCode"), 0}
                        })}, 
                        object::pair{std::string("executeCommand"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                            object::pair{std::string("stdout"), string_empty}, 
                            object::pair{std::string("stderr"), string_empty}, 
                            object::pair{std::string("exitCode"), 0}
                        })}
                    }));
                    auto problemStatement = object{
                        object::pair{std::string("githubUrl"), std::string("https://github.com/swe-agent/test-repo/issues/19")}
                    };
                    hook->onInit(object{
                        object::pair{std::string("env"), mockEnv}, 
                        object::pair{std::string("problemStatement"), std::string("problemStatement")}
                    });
                    mockedGithub->getGhIssueData->mockResolvedValueOnce(object{
                        object::pair{std::string("state"), std::string("open")}, 
                        object::pair{std::string("locked"), false}, 
                        object::pair{std::string("assignee"), nullptr}, 
                        object::pair{std::string("assignees"), array<any>()}, 
                        object::pair{std::string("pull_request"), object{
                            object::pair{std::string("url"), std::string("https://api.github.com/repos/swe-agent/test-repo/pulls/20")}
                        }}
                    });
                    mockedGithub->getAssociatedCommitUrls->mockResolvedValueOnce(array<string>{ std::string("https://github.com/swe-agent/test-repo/commit/abc123") });
                    std::async([=]() { hook->onInstanceCompleted(object{
                        object::pair{std::string("result"), agentRunResult}
                    }); });
                    expect(mockedGithub->getGhIssueData)->toHaveBeenCalled();
                }
                );
                it(std::string("should succeed when issue has commits but override is set"), [=]() mutable
                {
                    auto overrideHook = std::make_shared<OpenPRHook>(object{
                        object::pair{std::string("skipIfCommitsReferenceIssue"), false}
                    });
                    auto mockEnv = as<std::shared_ptr<SWEEnv>>(as<any>(object{
                        object::pair{std::string("communicate"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                            object::pair{std::string("stdout"), string_empty}, 
                            object::pair{std::string("stderr"), string_empty}, 
                            object::pair{std::string("exitCode"), 0}
                        })}, 
                        object::pair{std::string("executeCommand"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                            object::pair{std::string("stdout"), string_empty}, 
                            object::pair{std::string("stderr"), string_empty}, 
                            object::pair{std::string("exitCode"), 0}
                        })}
                    }));
                    auto problemStatement = object{
                        object::pair{std::string("githubUrl"), std::string("https://github.com/swe-agent/test-repo/issues/19")}
                    };
                    overrideHook->onInit(object{
                        object::pair{std::string("env"), mockEnv}, 
                        object::pair{std::string("problemStatement"), std::string("problemStatement")}
                    });
                    mockedGithub->getGhIssueData->mockResolvedValueOnce(object{
                        object::pair{std::string("state"), std::string("open")}, 
                        object::pair{std::string("locked"), false}, 
                        object::pair{std::string("assignee"), nullptr}, 
                        object::pair{std::string("assignees"), array<any>()}, 
                        object::pair{std::string("pull_request"), nullptr}
                    });
                    mockedGithub->getAssociatedCommitUrls->mockResolvedValueOnce(array<string>{ std::string("https://github.com/swe-agent/test-repo/commit/abc123") });
                    std::async([=]() { overrideHook->onInstanceCompleted(object{
                        object::pair{std::string("result"), agentRunResult}
                    }); });
                    expect(mockedGithub->getGhIssueData)->toHaveBeenCalled();
                }
                );
                it(std::string("should succeed for valid open issue"), [=]() mutable
                {
                    mockedGithub->getGhIssueData->mockResolvedValueOnce(object{
                        object::pair{std::string("state"), std::string("open")}, 
                        object::pair{std::string("locked"), false}, 
                        object::pair{std::string("assignee"), nullptr}, 
                        object::pair{std::string("assignees"), array<any>()}, 
                        object::pair{std::string("pull_request"), nullptr}
                    });
                    mockedGithub->getAssociatedCommitUrls->mockResolvedValueOnce(array<any>());
                    std::async([=]() { hook->onInstanceCompleted(object{
                        object::pair{std::string("result"), agentRunResult}
                    }); });
                    expect(mockedGithub->getGhIssueData)->toHaveBeenCalled();
                }
                );
            }
            );
            describe(std::string("Configuration options"), [=]() mutable
            {
                it(std::string("should respect skipIfCommitsReferenceIssue config"), [=]() mutable
                {
                    auto hookWithSkip = std::make_shared<OpenPRHook>(object{
                        object::pair{std::string("skipIfCommitsReferenceIssue"), true}
                    });
                    expect(hookWithSkip)->toBeDefined();
                    auto hookWithoutSkip = std::make_shared<OpenPRHook>(object{
                        object::pair{std::string("skipIfCommitsReferenceIssue"), false}
                    });
                    expect(hookWithoutSkip)->toBeDefined();
                }
                );
            }
            );
            describe(std::string("PR creation helpers"), [=]() mutable
            {
                it(std::string("should handle trajectory with response and observation fields"), [=]() mutable
                {
                    auto trajectoryWithResponse = array<array>{ object{
                        object::pair{std::string("action"), std::string("ls -la")}, 
                        object::pair{std::string("response"), std::string("ls -la")}, 
                        object::pair{std::string("observation"), std::string("file1.txt file2.txt")}, 
                        object::pair{std::string("thought"), std::string("Looking at files")}, 
                        object::pair{std::string("state"), object{}}, 
                        object::pair{std::string("executionTime"), 0}, 
                        object::pair{std::string("query"), array<any>()}, 
                        object::pair{std::string("extraInfo"), object{}}
                    }, object{
                        object::pair{std::string("action"), std::string("cat file1.txt")}, 
                        object::pair{std::string("response"), std::string("cat file1.txt")}, 
                        object::pair{std::string("observation"), std::string("Content of file1")}, 
                        object::pair{std::string("thought"), std::string("Reading file content")}, 
                        object::pair{std::string("state"), object{}}, 
                        object::pair{std::string("executionTime"), 0}, 
                        object::pair{std::string("query"), array<any>()}, 
                        object::pair{std::string("extraInfo"), object{}}
                    } };
                    agentRunResult->trajectory = trajectoryWithResponse;
                    mockedGithub->getGhIssueData->mockResolvedValueOnce(object{
                        object::pair{std::string("state"), std::string("open")}, 
                        object::pair{std::string("locked"), false}, 
                        object::pair{std::string("assignee"), nullptr}, 
                        object::pair{std::string("assignees"), array<any>()}, 
                        object::pair{std::string("pull_request"), nullptr}
                    });
                    mockedGithub->getAssociatedCommitUrls->mockResolvedValueOnce(array<any>());
                    std::async([=]() { hook->onInstanceCompleted(object{
                        object::pair{std::string("result"), agentRunResult}
                    }); });
                    expect(mockedGithub->getGhIssueData)->toHaveBeenCalled();
                }
                );
                it(std::string("should handle associated commits correctly"), [=]() mutable
                {
                    auto mockEnv = as<std::shared_ptr<SWEEnv>>(as<any>(object{
                        object::pair{std::string("communicate"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                            object::pair{std::string("stdout"), string_empty}, 
                            object::pair{std::string("stderr"), string_empty}, 
                            object::pair{std::string("exitCode"), 0}
                        })}, 
                        object::pair{std::string("executeCommand"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                            object::pair{std::string("stdout"), string_empty}, 
                            object::pair{std::string("stderr"), string_empty}, 
                            object::pair{std::string("exitCode"), 0}
                        })}
                    }));
                    auto problemStatement = object{
                        object::pair{std::string("githubUrl"), std::string("https://github.com/owner/repo/issues/41")}
                    };
                    hook->onInit(object{
                        object::pair{std::string("env"), mockEnv}, 
                        object::pair{std::string("problemStatement"), std::string("problemStatement")}
                    });
                    mockedGithub->getAssociatedCommitUrls->mockResolvedValueOnce(array<string>{ std::string("https://github.com/owner/repo/commit/abc123"), std::string("https://github.com/owner/repo/commit/def456") });
                    mockedGithub->getGhIssueData->mockResolvedValueOnce(object{
                        object::pair{std::string("state"), std::string("open")}, 
                        object::pair{std::string("locked"), false}, 
                        object::pair{std::string("assignee"), nullptr}, 
                        object::pair{std::string("assignees"), array<any>()}, 
                        object::pair{std::string("pull_request"), nullptr}
                    });
                    std::async([=]() { hook->onInstanceCompleted(object{
                        object::pair{std::string("result"), agentRunResult}
                    }); });
                    expect(mockedGithub->getAssociatedCommitUrls)->toHaveBeenCalled();
                }
                );
            }
            );
            describe(std::string("Hook lifecycle"), [=]() mutable
            {
                it(std::string("should handle onInstanceStart"), [=]() mutable
                {
                    shared mockEnv = as<std::shared_ptr<SWEEnv>>(as<any>(object{
                        object::pair{std::string("communicate"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                            object::pair{std::string("stdout"), string_empty}, 
                            object::pair{std::string("stderr"), string_empty}, 
                            object::pair{std::string("exitCode"), 0}
                        })}, 
                        object::pair{std::string("executeCommand"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                            object::pair{std::string("stdout"), string_empty}, 
                            object::pair{std::string("stderr"), string_empty}, 
                            object::pair{std::string("exitCode"), 0}
                        })}
                    }));
                    shared problemStatement = object{
                        object::pair{std::string("githubUrl"), std::string("https://github.com/owner/repo/issues/1")}
                    };
                    expect([=]() mutable
                    {
                        hook->onInstanceStart(object{
                            object::pair{std::string("index"), 0}, 
                            object::pair{std::string("env"), mockEnv}, 
                            object::pair{std::string("problemStatement"), as<any>(problemStatement)}
                        });
                    }
                    )->not->toThrow();
                }
                );
                it(std::string("should handle onInstanceSkipped"), [=]() mutable
                {
                    expect([=]() mutable
                    {
                        hook->onInstanceSkipped();
                    }
                    )->not->toThrow();
                }
                );
                it(std::string("should handle onInstanceCompleted"), [=]() mutable
                {
                    mockedGithub->getGhIssueData->mockResolvedValueOnce(object{
                        object::pair{std::string("state"), std::string("open")}, 
                        object::pair{std::string("locked"), false}, 
                        object::pair{std::string("assignee"), nullptr}, 
                        object::pair{std::string("assignees"), array<any>()}, 
                        object::pair{std::string("pull_request"), nullptr}
                    });
                    mockedGithub->getAssociatedCommitUrls->mockResolvedValueOnce(array<any>());
                    std::async([=]() { hook->onInstanceCompleted(object{
                        object::pair{std::string("result"), agentRunResult}
                    }); });
                    expect(mockedGithub->getGhIssueData)->toHaveBeenCalled();
                }
                );
                it(std::string("should handle onStart"), [=]() mutable
                {
                    expect([=]() mutable
                    {
                        hook->onStart();
                    }
                    )->not->toThrow();
                }
                );
                it(std::string("should handle onEnd"), [=]() mutable
                {
                    expect([=]() mutable
                    {
                        hook->onEnd();
                    }
                    )->not->toThrow();
                }
                );
            }
            );
            describe(std::string("Error handling"), [=]() mutable
            {
                it(std::string("should handle GitHub API errors gracefully"), [=]() mutable
                {
                    mockedGithub->getGhIssueData->mockRejectedValueOnce(std::make_shared<Error>(std::string("API Error")));
                    std::async([=]() { expect(hook->onInstanceCompleted(object{
                        object::pair{std::string("result"), agentRunResult}
                    }))->resolves->not->toThrow(); });
                }
                );
                it(std::string("should handle missing token"), [=]() mutable
                {
                    process->env.Delete("GITHUB_TOKEN");
                    auto hookWithoutToken = std::make_shared<OpenPRHook>(object{});
                    auto mockEnv = as<std::shared_ptr<SWEEnv>>(as<any>(object{
                        object::pair{std::string("communicate"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                            object::pair{std::string("stdout"), string_empty}, 
                            object::pair{std::string("stderr"), string_empty}, 
                            object::pair{std::string("exitCode"), 0}
                        })}, 
                        object::pair{std::string("executeCommand"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                            object::pair{std::string("stdout"), string_empty}, 
                            object::pair{std::string("stderr"), string_empty}, 
                            object::pair{std::string("exitCode"), 0}
                        })}
                    }));
                    auto problemStatement = object{
                        object::pair{std::string("githubUrl"), std::string("https://github.com/owner/repo/issues/1")}
                    };
                    hookWithoutToken->onInit(object{
                        object::pair{std::string("env"), mockEnv}, 
                        object::pair{std::string("problemStatement"), std::string("problemStatement")}
                    });
                    mockedGithub->getGhIssueData->mockResolvedValueOnce(object{
                        object::pair{std::string("state"), std::string("open")}, 
                        object::pair{std::string("locked"), false}, 
                        object::pair{std::string("assignee"), nullptr}, 
                        object::pair{std::string("assignees"), array<any>()}, 
                        object::pair{std::string("pull_request"), nullptr}
                    });
                    mockedGithub->getAssociatedCommitUrls->mockResolvedValueOnce(array<any>());
                    std::async([=]() { expect(hookWithoutToken->onInstanceCompleted(object{
                        object::pair{std::string("result"), agentRunResult}
                    }))->resolves->not->toThrow(); });
                }
                );
                it(std::string("should handle missing problem statement"), [=]() mutable
                {
                    auto hookNoProblem = std::make_shared<OpenPRHook>(object{});
                    auto mockEnv = as<std::shared_ptr<SWEEnv>>(as<any>(object{
                        object::pair{std::string("communicate"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                            object::pair{std::string("stdout"), string_empty}, 
                            object::pair{std::string("stderr"), string_empty}, 
                            object::pair{std::string("exitCode"), 0}
                        })}, 
                        object::pair{std::string("executeCommand"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                            object::pair{std::string("stdout"), string_empty}, 
                            object::pair{std::string("stderr"), string_empty}, 
                            object::pair{std::string("exitCode"), 0}
                        })}
                    }));
                    hookNoProblem->onInit(object{
                        object::pair{std::string("env"), mockEnv}, 
                        object::pair{std::string("problemStatement"), object{
                            object::pair{std::string("githubUrl"), string_empty}
                        }}
                    });
                    std::async([=]() { expect(hookNoProblem->onInstanceCompleted(object{
                        object::pair{std::string("result"), agentRunResult}
                    }))->resolves->not->toThrow(); });
                }
                );
            }
            );
        }
        );
    }
    );
}

MAIN
