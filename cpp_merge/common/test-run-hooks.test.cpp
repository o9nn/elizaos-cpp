#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/tests/test-run-hooks.test.h"

any mockedGithub = jest->mocked(github);

void Main(void)
{
    jest->mock(std:("@octokit/rest"), [=]() mutable
    {
        return (object{
            object::pair{std:("Octokit"), jest->fn()->mockImplementation([=]() mutable
            {
                return (object{
                    object::pair{std:("rest"), object{
                        object::pair{std:("pulls"), object{
                            object::pair{std:("create"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                                object::pair{std:("data"), object{
                                    object::pair{std:("html_url"), std:("https://github.com/test/repo/pull/1")}
                                }}
                            })}
                        }}, 
                        object::pair{std:("repos"), object{
                            object::pair{std:("get"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                                object::pair{std:("data"), object{
                                    object::pair{std:("default_branch"), std:("main")}
                                }}
                            })}, 
                            object::pair{std:("getCommit"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                                object::pair{std:("data"), object{
                                    object::pair{std:("commit"), object{
                                        object::pair{std:("message"), std:("test commit")}
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
    jest->mock(std:("../src/utils/github"));
    mockedGithub->parseGhIssueUrl->mockImplementation([=](auto url) mutable
    {
        auto match = url->match((new RegExp(std:("github\.com\/([^/]+)\/([^/]+)\/issues\/(\d+"))));
        if (!match) return as<any>(undefined);
        return object{
            object::pair{std:("owner"), (*const_(match))[1]}, 
            object::pair{std:("repo"), (*const_(match))[2]}, 
            object::pair{std:("issueNumber"), (*const_(match))[3]}
        };
    }
    );
    mockedGithub->getAssociatedCommitUrls->mockResolvedValue(array<any>());
    mockedGithub->getGhIssueData->mockResolvedValue(as<any>(object{
        object::pair{std:("state"), std:("open")}, 
        object::pair{std:("locked"), false}, 
        object::pair{std:("assignee"), nullptr}, 
        object::pair{std:("assignees"), array<any>()}, 
        object::pair{std:("pull_request"), nullptr}
    }));
    describe(std:("Run Hooks"), [=]() mutable
    {
        describe(std:("OpenPRHook"), [=]() mutable
        {
            shared<std::shared_ptr<OpenPRHook>> hook;
            shared<std::shared_ptr<AgentRunResult>> agentRunResult;
            beforeEach([=]() mutable
            {
                jest->clearAllMocks();
                process->env->GITHUB_TOKEN = std:("test-token");
                hook = std::make_shared<OpenPRHook>(object{
                    object::pair{std:("skipIfCommitsReferenceIssue"), true}
                });
                auto mockEnv = as<std::shared_ptr<SWEEnv>>(as<any>(object{
                    object::pair{std:("communicate"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                        object::pair{std:("stdout"), string_empty}, 
                        object::pair{std:("stderr"), string_empty}, 
                        object::pair{std:("exitCode"), 0}
                    })}, 
                    object::pair{std:("executeCommand"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                        object::pair{std:("stdout"), string_empty}, 
                        object::pair{std:("stderr"), string_empty}, 
                        object::pair{std:("exitCode"), 0}
                    })}
                }));
                auto problemStatement = object{
                    object::pair{std:("githubUrl"), std:("https://github.com/swe-agent/test-repo/issues/1")}
                };
                hook->onInit(object{
                    object::pair{std:("env"), mockEnv}, 
                    object::pair{std:("problemStatement"), std:("problemStatement")}
                });
                agentRunResult = object{
                    object::pair{std:("info"), object{
                        object::pair{std:("submission"), std:("asdf")}, 
                        object::pair{std:("exitStatus"), std:("submitted")}
                    }}, 
                    object::pair{std:("trajectory"), array<any>()}
                };
            }
            );
            describe(std:("should_open_pr checks"), [=]() mutable
            {
                it(std:("should fail when submission is missing"), [=]() mutable
                {
                    agentRunResult->info->submission = nullptr;
                    std::async([=]() { hook->onInstanceCompleted(object{
                        object::pair{std:("result"), agentRunResult}
                    }); });
                }
                );
                it(std:("should fail when submission is empty"), [=]() mutable
                {
                    agentRunResult->info->submission = string_empty;
                    std::async([=]() { hook->onInstanceCompleted(object{
                        object::pair{std:("result"), agentRunResult}
                    }); });
                }
                );
                it(std:("should fail when exit status is not submitted"), [=]() mutable
                {
                    agentRunResult->info->exitStatus = std:("fail");
                    std::async([=]() { hook->onInstanceCompleted(object{
                        object::pair{std:("result"), agentRunResult}
                    }); });
                }
                );
                it(std:("should fail when exit status indicates error"), [=]() mutable
                {
                    agentRunResult->info->exitStatus = std:("exit_cost");
                    std::async([=]() { hook->onInstanceCompleted(object{
                        object::pair{std:("result"), agentRunResult}
                    }); });
                }
                );
                it(std:("should fail when invalid URL is provided"), [=]() mutable
                {
                    auto mockEnv = as<std::shared_ptr<SWEEnv>>(as<any>(object{
                        object::pair{std:("communicate"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                            object::pair{std:("stdout"), string_empty}, 
                            object::pair{std:("stderr"), string_empty}, 
                            object::pair{std:("exitCode"), 0}
                        })}, 
                        object::pair{std:("executeCommand"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                            object::pair{std:("stdout"), string_empty}, 
                            object::pair{std:("stderr"), string_empty}, 
                            object::pair{std:("exitCode"), 0}
                        })}
                    }));
                    auto problemStatement = object{
                        object::pair{std:("githubUrl"), std:("https://github.com/swe-agent/test-repo/issues/invalid")}
                    };
                    hook->onInit(object{
                        object::pair{std:("env"), mockEnv}, 
                        object::pair{std:("problemStatement"), std:("problemStatement")}
                    });
                    mockedGithub->getGhIssueData->mockRejectedValueOnce(std::make_shared<github->InvalidGithubURL>(std:("Invalid URL")));
                    std::async([=]() { hook->onInstanceCompleted(object{
                        object::pair{std:("result"), agentRunResult}
                    }); });
                }
                );
                it(std:("should fail when issue is closed"), [=]() mutable
                {
                    auto mockEnv = as<std::shared_ptr<SWEEnv>>(as<any>(object{
                        object::pair{std:("communicate"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                            object::pair{std:("stdout"), string_empty}, 
                            object::pair{std:("stderr"), string_empty}, 
                            object::pair{std:("exitCode"), 0}
                        })}, 
                        object::pair{std:("executeCommand"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                            object::pair{std:("stdout"), string_empty}, 
                            object::pair{std:("stderr"), string_empty}, 
                            object::pair{std:("exitCode"), 0}
                        })}
                    }));
                    auto problemStatement = object{
                        object::pair{std:("githubUrl"), std:("https://github.com/swe-agent/test-repo/issues/16")}
                    };
                    hook->onInit(object{
                        object::pair{std:("env"), mockEnv}, 
                        object::pair{std:("problemStatement"), std:("problemStatement")}
                    });
                    mockedGithub->getGhIssueData->mockResolvedValueOnce(object{
                        object::pair{std:("state"), std:("closed")}, 
                        object::pair{std:("locked"), false}, 
                        object::pair{std:("assignee"), nullptr}, 
                        object::pair{std:("assignees"), array<any>()}, 
                        object::pair{std:("pull_request"), nullptr}
                    });
                    std::async([=]() { hook->onInstanceCompleted(object{
                        object::pair{std:("result"), agentRunResult}
                    }); });
                    expect(mockedGithub->getGhIssueData)->toHaveBeenCalled();
                }
                );
                it(std:("should fail when issue is assigned"), [=]() mutable
                {
                    auto mockEnv = as<std::shared_ptr<SWEEnv>>(as<any>(object{
                        object::pair{std:("communicate"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                            object::pair{std:("stdout"), string_empty}, 
                            object::pair{std:("stderr"), string_empty}, 
                            object::pair{std:("exitCode"), 0}
                        })}, 
                        object::pair{std:("executeCommand"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                            object::pair{std:("stdout"), string_empty}, 
                            object::pair{std:("stderr"), string_empty}, 
                            object::pair{std:("exitCode"), 0}
                        })}
                    }));
                    auto problemStatement = object{
                        object::pair{std:("githubUrl"), std:("https://github.com/swe-agent/test-repo/issues/17")}
                    };
                    hook->onInit(object{
                        object::pair{std:("env"), mockEnv}, 
                        object::pair{std:("problemStatement"), std:("problemStatement")}
                    });
                    mockedGithub->getGhIssueData->mockResolvedValueOnce(object{
                        object::pair{std:("state"), std:("open")}, 
                        object::pair{std:("locked"), false}, 
                        object::pair{std:("assignee"), object{
                            object::pair{std:("login"), std:("someone")}
                        }}, 
                        object::pair{std:("assignees"), array<object>{ object{
                            object::pair{std:("login"), std:("someone")}
                        } }}, 
                        object::pair{std:("pull_request"), nullptr}
                    });
                    std::async([=]() { hook->onInstanceCompleted(object{
                        object::pair{std:("result"), agentRunResult}
                    }); });
                    expect(mockedGithub->getGhIssueData)->toHaveBeenCalled();
                }
                );
                it(std:("should fail when issue is locked"), [=]() mutable
                {
                    auto mockEnv = as<std::shared_ptr<SWEEnv>>(as<any>(object{
                        object::pair{std:("communicate"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                            object::pair{std:("stdout"), string_empty}, 
                            object::pair{std:("stderr"), string_empty}, 
                            object::pair{std:("exitCode"), 0}
                        })}, 
                        object::pair{std:("executeCommand"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                            object::pair{std:("stdout"), string_empty}, 
                            object::pair{std:("stderr"), string_empty}, 
                            object::pair{std:("exitCode"), 0}
                        })}
                    }));
                    auto problemStatement = object{
                        object::pair{std:("githubUrl"), std:("https://github.com/swe-agent/test-repo/issues/18")}
                    };
                    hook->onInit(object{
                        object::pair{std:("env"), mockEnv}, 
                        object::pair{std:("problemStatement"), std:("problemStatement")}
                    });
                    mockedGithub->getGhIssueData->mockResolvedValueOnce(object{
                        object::pair{std:("state"), std:("open")}, 
                        object::pair{std:("locked"), true}, 
                        object::pair{std:("assignee"), nullptr}, 
                        object::pair{std:("assignees"), array<any>()}, 
                        object::pair{std:("pull_request"), nullptr}
                    });
                    std::async([=]() { hook->onInstanceCompleted(object{
                        object::pair{std:("result"), agentRunResult}
                    }); });
                    expect(mockedGithub->getGhIssueData)->toHaveBeenCalled();
                }
                );
                it(std:("should fail when issue already has PR"), [=]() mutable
                {
                    auto mockEnv = as<std::shared_ptr<SWEEnv>>(as<any>(object{
                        object::pair{std:("communicate"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                            object::pair{std:("stdout"), string_empty}, 
                            object::pair{std:("stderr"), string_empty}, 
                            object::pair{std:("exitCode"), 0}
                        })}, 
                        object::pair{std:("executeCommand"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                            object::pair{std:("stdout"), string_empty}, 
                            object::pair{std:("stderr"), string_empty}, 
                            object::pair{std:("exitCode"), 0}
                        })}
                    }));
                    auto problemStatement = object{
                        object::pair{std:("githubUrl"), std:("https://github.com/swe-agent/test-repo/issues/19")}
                    };
                    hook->onInit(object{
                        object::pair{std:("env"), mockEnv}, 
                        object::pair{std:("problemStatement"), std:("problemStatement")}
                    });
                    mockedGithub->getGhIssueData->mockResolvedValueOnce(object{
                        object::pair{std:("state"), std:("open")}, 
                        object::pair{std:("locked"), false}, 
                        object::pair{std:("assignee"), nullptr}, 
                        object::pair{std:("assignees"), array<any>()}, 
                        object::pair{std:("pull_request"), object{
                            object::pair{std:("url"), std:("https://api.github.com/repos/swe-agent/test-repo/pulls/20")}
                        }}
                    });
                    mockedGithub->getAssociatedCommitUrls->mockResolvedValueOnce(array<string>{ std:("https://github.com/swe-agent/test-repo/commit/abc123") });
                    std::async([=]() { hook->onInstanceCompleted(object{
                        object::pair{std:("result"), agentRunResult}
                    }); });
                    expect(mockedGithub->getGhIssueData)->toHaveBeenCalled();
                }
                );
                it(std:("should succeed when issue has commits but override is set"), [=]() mutable
                {
                    auto overrideHook = std::make_shared<OpenPRHook>(object{
                        object::pair{std:("skipIfCommitsReferenceIssue"), false}
                    });
                    auto mockEnv = as<std::shared_ptr<SWEEnv>>(as<any>(object{
                        object::pair{std:("communicate"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                            object::pair{std:("stdout"), string_empty}, 
                            object::pair{std:("stderr"), string_empty}, 
                            object::pair{std:("exitCode"), 0}
                        })}, 
                        object::pair{std:("executeCommand"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                            object::pair{std:("stdout"), string_empty}, 
                            object::pair{std:("stderr"), string_empty}, 
                            object::pair{std:("exitCode"), 0}
                        })}
                    }));
                    auto problemStatement = object{
                        object::pair{std:("githubUrl"), std:("https://github.com/swe-agent/test-repo/issues/19")}
                    };
                    overrideHook->onInit(object{
                        object::pair{std:("env"), mockEnv}, 
                        object::pair{std:("problemStatement"), std:("problemStatement")}
                    });
                    mockedGithub->getGhIssueData->mockResolvedValueOnce(object{
                        object::pair{std:("state"), std:("open")}, 
                        object::pair{std:("locked"), false}, 
                        object::pair{std:("assignee"), nullptr}, 
                        object::pair{std:("assignees"), array<any>()}, 
                        object::pair{std:("pull_request"), nullptr}
                    });
                    mockedGithub->getAssociatedCommitUrls->mockResolvedValueOnce(array<string>{ std:("https://github.com/swe-agent/test-repo/commit/abc123") });
                    std::async([=]() { overrideHook->onInstanceCompleted(object{
                        object::pair{std:("result"), agentRunResult}
                    }); });
                    expect(mockedGithub->getGhIssueData)->toHaveBeenCalled();
                }
                );
                it(std:("should succeed for valid open issue"), [=]() mutable
                {
                    mockedGithub->getGhIssueData->mockResolvedValueOnce(object{
                        object::pair{std:("state"), std:("open")}, 
                        object::pair{std:("locked"), false}, 
                        object::pair{std:("assignee"), nullptr}, 
                        object::pair{std:("assignees"), array<any>()}, 
                        object::pair{std:("pull_request"), nullptr}
                    });
                    mockedGithub->getAssociatedCommitUrls->mockResolvedValueOnce(array<any>());
                    std::async([=]() { hook->onInstanceCompleted(object{
                        object::pair{std:("result"), agentRunResult}
                    }); });
                    expect(mockedGithub->getGhIssueData)->toHaveBeenCalled();
                }
                );
            }
            );
            describe(std:("Configuration options"), [=]() mutable
            {
                it(std:("should respect skipIfCommitsReferenceIssue config"), [=]() mutable
                {
                    auto hookWithSkip = std::make_shared<OpenPRHook>(object{
                        object::pair{std:("skipIfCommitsReferenceIssue"), true}
                    });
                    expect(hookWithSkip)->toBeDefined();
                    auto hookWithoutSkip = std::make_shared<OpenPRHook>(object{
                        object::pair{std:("skipIfCommitsReferenceIssue"), false}
                    });
                    expect(hookWithoutSkip)->toBeDefined();
                }
                );
            }
            );
            describe(std:("PR creation helpers"), [=]() mutable
            {
                it(std:("should handle trajectory with response and observation fields"), [=]() mutable
                {
                    auto trajectoryWithResponse = array<array>{ object{
                        object::pair{std:("action"), std:("ls -la")}, 
                        object::pair{std:("response"), std:("ls -la")}, 
                        object::pair{std:("observation"), std:("file1.txt file2.txt")}, 
                        object::pair{std:("thought"), std:("Looking at files")}, 
                        object::pair{std:("state"), object{}}, 
                        object::pair{std:("executionTime"), 0}, 
                        object::pair{std:("query"), array<any>()}, 
                        object::pair{std:("extraInfo"), object{}}
                    }, object{
                        object::pair{std:("action"), std:("cat file1.txt")}, 
                        object::pair{std:("response"), std:("cat file1.txt")}, 
                        object::pair{std:("observation"), std:("Content of file1")}, 
                        object::pair{std:("thought"), std:("Reading file content")}, 
                        object::pair{std:("state"), object{}}, 
                        object::pair{std:("executionTime"), 0}, 
                        object::pair{std:("query"), array<any>()}, 
                        object::pair{std:("extraInfo"), object{}}
                    } };
                    agentRunResult->trajectory = trajectoryWithResponse;
                    mockedGithub->getGhIssueData->mockResolvedValueOnce(object{
                        object::pair{std:("state"), std:("open")}, 
                        object::pair{std:("locked"), false}, 
                        object::pair{std:("assignee"), nullptr}, 
                        object::pair{std:("assignees"), array<any>()}, 
                        object::pair{std:("pull_request"), nullptr}
                    });
                    mockedGithub->getAssociatedCommitUrls->mockResolvedValueOnce(array<any>());
                    std::async([=]() { hook->onInstanceCompleted(object{
                        object::pair{std:("result"), agentRunResult}
                    }); });
                    expect(mockedGithub->getGhIssueData)->toHaveBeenCalled();
                }
                );
                it(std:("should handle associated commits correctly"), [=]() mutable
                {
                    auto mockEnv = as<std::shared_ptr<SWEEnv>>(as<any>(object{
                        object::pair{std:("communicate"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                            object::pair{std:("stdout"), string_empty}, 
                            object::pair{std:("stderr"), string_empty}, 
                            object::pair{std:("exitCode"), 0}
                        })}, 
                        object::pair{std:("executeCommand"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                            object::pair{std:("stdout"), string_empty}, 
                            object::pair{std:("stderr"), string_empty}, 
                            object::pair{std:("exitCode"), 0}
                        })}
                    }));
                    auto problemStatement = object{
                        object::pair{std:("githubUrl"), std:("https://github.com/owner/repo/issues/41")}
                    };
                    hook->onInit(object{
                        object::pair{std:("env"), mockEnv}, 
                        object::pair{std:("problemStatement"), std:("problemStatement")}
                    });
                    mockedGithub->getAssociatedCommitUrls->mockResolvedValueOnce(array<string>{ std:("https://github.com/owner/repo/commit/abc123"), std:("https://github.com/owner/repo/commit/def456") });
                    mockedGithub->getGhIssueData->mockResolvedValueOnce(object{
                        object::pair{std:("state"), std:("open")}, 
                        object::pair{std:("locked"), false}, 
                        object::pair{std:("assignee"), nullptr}, 
                        object::pair{std:("assignees"), array<any>()}, 
                        object::pair{std:("pull_request"), nullptr}
                    });
                    std::async([=]() { hook->onInstanceCompleted(object{
                        object::pair{std:("result"), agentRunResult}
                    }); });
                    expect(mockedGithub->getAssociatedCommitUrls)->toHaveBeenCalled();
                }
                );
            }
            );
            describe(std:("Hook lifecycle"), [=]() mutable
            {
                it(std:("should handle onInstanceStart"), [=]() mutable
                {
                    shared mockEnv = as<std::shared_ptr<SWEEnv>>(as<any>(object{
                        object::pair{std:("communicate"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                            object::pair{std:("stdout"), string_empty}, 
                            object::pair{std:("stderr"), string_empty}, 
                            object::pair{std:("exitCode"), 0}
                        })}, 
                        object::pair{std:("executeCommand"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                            object::pair{std:("stdout"), string_empty}, 
                            object::pair{std:("stderr"), string_empty}, 
                            object::pair{std:("exitCode"), 0}
                        })}
                    }));
                    shared problemStatement = object{
                        object::pair{std:("githubUrl"), std:("https://github.com/owner/repo/issues/1")}
                    };
                    expect([=]() mutable
                    {
                        hook->onInstanceStart(object{
                            object::pair{std:("index"), 0}, 
                            object::pair{std:("env"), mockEnv}, 
                            object::pair{std:("problemStatement"), as<any>(problemStatement)}
                        });
                    }
                    )->not->toThrow();
                }
                );
                it(std:("should handle onInstanceSkipped"), [=]() mutable
                {
                    expect([=]() mutable
                    {
                        hook->onInstanceSkipped();
                    }
                    )->not->toThrow();
                }
                );
                it(std:("should handle onInstanceCompleted"), [=]() mutable
                {
                    mockedGithub->getGhIssueData->mockResolvedValueOnce(object{
                        object::pair{std:("state"), std:("open")}, 
                        object::pair{std:("locked"), false}, 
                        object::pair{std:("assignee"), nullptr}, 
                        object::pair{std:("assignees"), array<any>()}, 
                        object::pair{std:("pull_request"), nullptr}
                    });
                    mockedGithub->getAssociatedCommitUrls->mockResolvedValueOnce(array<any>());
                    std::async([=]() { hook->onInstanceCompleted(object{
                        object::pair{std:("result"), agentRunResult}
                    }); });
                    expect(mockedGithub->getGhIssueData)->toHaveBeenCalled();
                }
                );
                it(std:("should handle onStart"), [=]() mutable
                {
                    expect([=]() mutable
                    {
                        hook->onStart();
                    }
                    )->not->toThrow();
                }
                );
                it(std:("should handle onEnd"), [=]() mutable
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
            describe(std:("Error handling"), [=]() mutable
            {
                it(std:("should handle GitHub API errors gracefully"), [=]() mutable
                {
                    mockedGithub->getGhIssueData->mockRejectedValueOnce(std::make_shared<Error>(std:("API Error")));
                    std::async([=]() { expect(hook->onInstanceCompleted(object{
                        object::pair{std:("result"), agentRunResult}
                    }))->resolves->not->toThrow(); });
                }
                );
                it(std:("should handle missing token"), [=]() mutable
                {
                    process->env.Delete("GITHUB_TOKEN");
                    auto hookWithoutToken = std::make_shared<OpenPRHook>(object{});
                    auto mockEnv = as<std::shared_ptr<SWEEnv>>(as<any>(object{
                        object::pair{std:("communicate"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                            object::pair{std:("stdout"), string_empty}, 
                            object::pair{std:("stderr"), string_empty}, 
                            object::pair{std:("exitCode"), 0}
                        })}, 
                        object::pair{std:("executeCommand"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                            object::pair{std:("stdout"), string_empty}, 
                            object::pair{std:("stderr"), string_empty}, 
                            object::pair{std:("exitCode"), 0}
                        })}
                    }));
                    auto problemStatement = object{
                        object::pair{std:("githubUrl"), std:("https://github.com/owner/repo/issues/1")}
                    };
                    hookWithoutToken->onInit(object{
                        object::pair{std:("env"), mockEnv}, 
                        object::pair{std:("problemStatement"), std:("problemStatement")}
                    });
                    mockedGithub->getGhIssueData->mockResolvedValueOnce(object{
                        object::pair{std:("state"), std:("open")}, 
                        object::pair{std:("locked"), false}, 
                        object::pair{std:("assignee"), nullptr}, 
                        object::pair{std:("assignees"), array<any>()}, 
                        object::pair{std:("pull_request"), nullptr}
                    });
                    mockedGithub->getAssociatedCommitUrls->mockResolvedValueOnce(array<any>());
                    std::async([=]() { expect(hookWithoutToken->onInstanceCompleted(object{
                        object::pair{std:("result"), agentRunResult}
                    }))->resolves->not->toThrow(); });
                }
                );
                it(std:("should handle missing problem statement"), [=]() mutable
                {
                    auto hookNoProblem = std::make_shared<OpenPRHook>(object{});
                    auto mockEnv = as<std::shared_ptr<SWEEnv>>(as<any>(object{
                        object::pair{std:("communicate"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                            object::pair{std:("stdout"), string_empty}, 
                            object::pair{std:("stderr"), string_empty}, 
                            object::pair{std:("exitCode"), 0}
                        })}, 
                        object::pair{std:("executeCommand"), (as<any>(jest->fn()))["mockResolvedValue"](object{
                            object::pair{std:("stdout"), string_empty}, 
                            object::pair{std:("stderr"), string_empty}, 
                            object::pair{std:("exitCode"), 0}
                        })}
                    }));
                    hookNoProblem->onInit(object{
                        object::pair{std:("env"), mockEnv}, 
                        object::pair{std:("problemStatement"), object{
                            object::pair{std:("githubUrl"), string_empty}
                        }}
                    });
                    std::async([=]() { expect(hookNoProblem->onInstanceCompleted(object{
                        object::pair{std:("result"), agentRunResult}
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
