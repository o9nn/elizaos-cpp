#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-stagehand/src/__tests__/capsolver.test.h"

std::function<object(any)> createMockResponse = [=](auto data) mutable
{
    return (object{
        object::pair{std::string("data"), std::string("data")}, 
        object::pair{std::string("status"), 200}, 
        object::pair{std::string("statusText"), std::string("OK")}, 
        object::pair{std::string("headers"), object{}}, 
        object::pair{std::string("config"), as<any>(object{
            object::pair{std::string("url"), string_empty}, 
            object::pair{std::string("method"), std::string("post")}, 
            object::pair{std::string("headers"), object{}}
        })}, 
        object::pair{std::string("request"), object{}}
    });
};

void Main(void)
{
    mock->module(std::string("@elizaos/core"), [=]() mutable
    {
        return (object{
            object::pair{std::string("logger"), object{
                object::pair{std::string("info"), mock()}, 
                object::pair{std::string("error"), mock()}, 
                object::pair{std::string("warn"), mock()}, 
                object::pair{std::string("debug"), mock()}
            }}
        });
    }
    );
    describe(std::string("CapSolverService"), [=]() mutable
    {
        shared<std::shared_ptr<CapSolverService>> capSolver;
        shared mockApiKey = std::string("test-api-key");
        beforeEach([=]() mutable
        {
            capSolver = std::make_shared<CapSolverService>(object{
                object::pair{std::string("apiKey"), mockApiKey}
            });
        }
        );
        afterEach([=]() mutable
        {
            mock->restore();
        }
        );
        describe(std::string("createTask"), [=]() mutable
        {
            it(std::string("should create a task successfully"), [=]() mutable
            {
                auto mockTaskId = std::string("task-123");
                auto postSpy = spyOn(axios, std::string("post"))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std::string("errorId"), 0}, 
                    object::pair{std::string("taskId"), mockTaskId}
                }));
                auto task = object{
                    object::pair{std::string("type"), std::string("AntiTurnstileTaskProxyLess")}, 
                    object::pair{std::string("websiteURL"), std::string("https://example.com")}, 
                    object::pair{std::string("websiteKey"), std::string("test-key")}
                };
                auto taskId = std::async([=]() { capSolver->createTask(task); });
                expect(taskId)->toBe(mockTaskId);
                expect(postSpy)->toHaveBeenCalledWith(std::string("https://api.capsolver.com/createTask"), object{
                    object::pair{std::string("clientKey"), mockApiKey}, 
                    object::pair{std::string("task"), std::string("task")}
                }, expect->any(Object));
            }
            );
            it(std::string("should throw error when API returns error"), [=]() mutable
            {
                spyOn(axios, std::string("post"))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std::string("errorId"), 1}, 
                    object::pair{std::string("errorDescription"), std::string("Invalid API key")}
                }));
                auto task = object{
                    object::pair{std::string("type"), std::string("AntiTurnstileTaskProxyLess")}, 
                    object::pair{std::string("websiteURL"), std::string("https://example.com")}, 
                    object::pair{std::string("websiteKey"), std::string("test-key")}
                };
                std::async([=]() { expect(capSolver->createTask(task))->rejects->toThrow(std::string("CapSolver error: Invalid API key")); });
            }
            );
        }
        );
        describe(std::string("getTaskResult"), [=]() mutable
        {
            it(std::string("should return solution when task is ready"), [=]() mutable
            {
                auto mockSolution = object{
                    object::pair{std::string("token"), std::string("solved-token")}
                };
                spyOn(axios, std::string("post"))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std::string("errorId"), 0}, 
                    object::pair{std::string("status"), std::string("ready")}, 
                    object::pair{std::string("solution"), mockSolution}
                }));
                auto result = std::async([=]() { capSolver->getTaskResult(std::string("task-123")); });
                expect(result)->toEqual(mockSolution);
                expect(axios->post)->toHaveBeenCalledWith(std::string("https://api.capsolver.com/getTaskResult"), object{
                    object::pair{std::string("clientKey"), mockApiKey}, 
                    object::pair{std::string("taskId"), std::string("task-123")}
                }, expect->any(Object));
            }
            );
            it(std::string("should poll until task is ready"), [=]() mutable
            {
                spyOn(axios, std::string("post"))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std::string("errorId"), 0}, 
                    object::pair{std::string("status"), std::string("processing")}
                }))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std::string("errorId"), 0}, 
                    object::pair{std::string("status"), std::string("ready")}, 
                    object::pair{std::string("solution"), object{
                        object::pair{std::string("token"), std::string("solved-token")}
                    }}
                }));
                auto fastCapSolver = std::make_shared<CapSolverService>(object{
                    object::pair{std::string("apiKey"), mockApiKey}, 
                    object::pair{std::string("pollingInterval"), 10}
                });
                auto result = std::async([=]() { fastCapSolver->getTaskResult(std::string("task-123")); });
                expect(result)->toEqual(object{
                    object::pair{std::string("token"), std::string("solved-token")}
                });
                expect(axios->post)->toHaveBeenCalledTimes(2);
            }
            );
            it(std::string("should throw error on timeout"), [=]() mutable
            {
                spyOn(axios, std::string("post"))->mockResolvedValue(createMockResponse(object{
                    object::pair{std::string("errorId"), 0}, 
                    object::pair{std::string("status"), std::string("processing")}
                }));
                auto fastCapSolver = std::make_shared<CapSolverService>(object{
                    object::pair{std::string("apiKey"), mockApiKey}, 
                    object::pair{std::string("pollingInterval"), 10}, 
                    object::pair{std::string("retryAttempts"), 2}
                });
                std::async([=]() { expect(fastCapSolver->getTaskResult(std::string("task-123")))->rejects->toThrow(std::string("CapSolver task timeout")); });
            }
            );
        }
        );
        describe(std::string("solveTurnstile"), [=]() mutable
        {
            it(std::string("should solve Turnstile captcha"), [=]() mutable
            {
                auto mockTaskId = std::string("task-123");
                auto mockToken = std::string("turnstile-token");
                spyOn(axios, std::string("post"))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std::string("errorId"), 0}, 
                    object::pair{std::string("taskId"), mockTaskId}
                }))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std::string("errorId"), 0}, 
                    object::pair{std::string("status"), std::string("ready")}, 
                    object::pair{std::string("solution"), object{
                        object::pair{std::string("token"), mockToken}
                    }}
                }));
                auto token = std::async([=]() { capSolver->solveTurnstile(std::string("https://example.com"), std::string("site-key")); });
                expect(token)->toBe(mockToken);
                expect(logger->info)->toHaveBeenCalledWith(std::string("Solving Cloudflare Turnstile captcha"));
            }
            );
            it(std::string("should use proxy when provided"), [=]() mutable
            {
                spyOn(axios, std::string("post"))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std::string("errorId"), 0}, 
                    object::pair{std::string("taskId"), std::string("task-123")}
                }))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std::string("errorId"), 0}, 
                    object::pair{std::string("status"), std::string("ready")}, 
                    object::pair{std::string("solution"), object{
                        object::pair{std::string("token"), std::string("proxy-token")}
                    }}
                }));
                std::async([=]() { capSolver->solveTurnstile(std::string("https://example.com"), std::string("site-key"), std::string("proxy-host:8080:username:password")); });
                expect(axios->post)->toHaveBeenCalledWith(std::string("https://api.capsolver.com/createTask"), expect->objectContaining(object{
                    object::pair{std::string("task"), expect->objectContaining(object{
                        object::pair{std::string("type"), std::string("AntiTurnstileTask")}, 
                        object::pair{std::string("proxy"), std::string("proxy-host:8080")}, 
                        object::pair{std::string("proxyLogin"), std::string("username")}, 
                        object::pair{std::string("proxyPassword"), std::string("password")}
                    })}
                }), expect->any(Object));
            }
            );
        }
        );
        describe(std::string("solveRecaptchaV2"), [=]() mutable
        {
            it(std::string("should solve reCAPTCHA v2"), [=]() mutable
            {
                spyOn(axios, std::string("post"))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std::string("errorId"), 0}, 
                    object::pair{std::string("taskId"), std::string("task-456")}
                }))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std::string("errorId"), 0}, 
                    object::pair{std::string("status"), std::string("ready")}, 
                    object::pair{std::string("solution"), object{
                        object::pair{std::string("gRecaptchaResponse"), std::string("recaptcha-v2-token")}
                    }}
                }));
                auto result = std::async([=]() { capSolver->solveRecaptchaV2(std::string("https://example.com"), std::string("v2-site-key")); });
                expect(result)->toBe(std::string("recaptcha-v2-token"));
                expect(logger->info)->toHaveBeenCalledWith(std::string("Solving reCAPTCHA v2"));
            }
            );
            it(std::string("should handle invisible reCAPTCHA v2"), [=]() mutable
            {
                spyOn(axios, std::string("post"))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std::string("errorId"), 0}, 
                    object::pair{std::string("taskId"), std::string("task-789")}
                }))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std::string("errorId"), 0}, 
                    object::pair{std::string("status"), std::string("ready")}, 
                    object::pair{std::string("solution"), object{
                        object::pair{std::string("gRecaptchaResponse"), std::string("invisible-token")}
                    }}
                }));
                auto result = std::async([=]() { capSolver->solveRecaptchaV2(std::string("https://example.com"), std::string("invisible-key"), true); });
                expect(result)->toBe(std::string("invisible-token"));
                expect(axios->post)->toHaveBeenCalledWith(std::string("https://api.capsolver.com/createTask"), expect->objectContaining(object{
                    object::pair{std::string("task"), expect->objectContaining(object{
                        object::pair{std::string("isInvisible"), true}
                    })}
                }), expect->any(Object));
            }
            );
        }
        );
        describe(std::string("solveRecaptchaV3"), [=]() mutable
        {
            it(std::string("should solve reCAPTCHA v3"), [=]() mutable
            {
                spyOn(axios, std::string("post"))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std::string("errorId"), 0}, 
                    object::pair{std::string("taskId"), std::string("task-v3")}
                }))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std::string("errorId"), 0}, 
                    object::pair{std::string("status"), std::string("ready")}, 
                    object::pair{std::string("solution"), object{
                        object::pair{std::string("gRecaptchaResponse"), std::string("v3-token")}
                    }}
                }));
                auto result = std::async([=]() { capSolver->solveRecaptchaV3(std::string("https://example.com"), std::string("v3-key"), std::string("verify")); });
                expect(result)->toBe(std::string("v3-token"));
                expect(logger->info)->toHaveBeenCalledWith(std::string("Solving reCAPTCHA v3"));
            }
            );
            it(std::string("should use custom action and score"), [=]() mutable
            {
                spyOn(axios, std::string("post"))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std::string("errorId"), 0}, 
                    object::pair{std::string("taskId"), std::string("task-v3-custom")}
                }))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std::string("errorId"), 0}, 
                    object::pair{std::string("status"), std::string("ready")}, 
                    object::pair{std::string("solution"), object{
                        object::pair{std::string("gRecaptchaResponse"), std::string("v3-custom-token")}
                    }}
                }));
                auto result = std::async([=]() { capSolver->solveRecaptchaV3(std::string("https://example.com"), std::string("v3-key"), std::string("login"), 0.7); });
                expect(result)->toBe(std::string("v3-custom-token"));
                expect(axios->post)->toHaveBeenCalledWith(std::string("https://api.capsolver.com/createTask"), expect->objectContaining(object{
                    object::pair{std::string("task"), expect->objectContaining(object{
                        object::pair{std::string("pageAction"), std::string("login")}, 
                        object::pair{std::string("minScore"), 0.7}
                    })}
                }), expect->any(Object));
            }
            );
        }
        );
        describe(std::string("solveHCaptcha"), [=]() mutable
        {
            it(std::string("should solve hCaptcha"), [=]() mutable
            {
                spyOn(axios, std::string("post"))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std::string("errorId"), 0}, 
                    object::pair{std::string("taskId"), std::string("task-hcaptcha")}
                }))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std::string("errorId"), 0}, 
                    object::pair{std::string("status"), std::string("ready")}, 
                    object::pair{std::string("solution"), object{
                        object::pair{std::string("token"), std::string("hcaptcha-token")}
                    }}
                }));
                auto result = std::async([=]() { capSolver->solveHCaptcha(std::string("https://example.com"), std::string("hcaptcha-key")); });
                expect(result)->toBe(std::string("hcaptcha-token"));
                expect(logger->info)->toHaveBeenCalledWith(std::string("Solving hCaptcha"));
            }
            );
            it(std::string("should use proxy when provided"), [=]() mutable
            {
                spyOn(axios, std::string("post"))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std::string("errorId"), 0}, 
                    object::pair{std::string("taskId"), std::string("task-hcaptcha-proxy")}
                }))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std::string("errorId"), 0}, 
                    object::pair{std::string("status"), std::string("ready")}, 
                    object::pair{std::string("solution"), object{
                        object::pair{std::string("token"), std::string("hcaptcha-proxy-token")}
                    }}
                }));
                std::async([=]() { capSolver->solveHCaptcha(std::string("https://example.com"), std::string("hcaptcha-key"), std::string("proxy-host:8080:username:password")); });
                expect(axios->post)->toHaveBeenCalledWith(std::string("https://api.capsolver.com/createTask"), expect->objectContaining(object{
                    object::pair{std::string("task"), expect->objectContaining(object{
                        object::pair{std::string("type"), std::string("HCaptchaTask")}, 
                        object::pair{std::string("proxy"), std::string("proxy-host:8080")}, 
                        object::pair{std::string("proxyLogin"), std::string("username")}, 
                        object::pair{std::string("proxyPassword"), std::string("password")}
                    })}
                }), expect->any(Object));
            }
            );
        }
        );
        describe(std::string("error handling"), [=]() mutable
        {
            it(std::string("should handle network errors"), [=]() mutable
            {
                spyOn(axios, std::string("post"))->mockRejectedValueOnce(std::make_shared<Error>(std::string("Network error")));
                std::async([=]() { expect(capSolver->createTask(object{
                    object::pair{std::string("type"), std::string("AntiTurnstileTaskProxyLess")}, 
                    object::pair{std::string("websiteURL"), std::string("https://example.com")}, 
                    object::pair{std::string("websiteKey"), std::string("test-key")}
                }))->rejects->toThrow(std::string("Network error")); });
                expect(logger->error)->toHaveBeenCalledWith(std::string("Error creating CapSolver task:"), expect->any(Error));
            }
            );
            it(std::string("should handle invalid proxy format"), [=]() mutable
            {
                spyOn(axios, std::string("post"))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std::string("errorId"), 0}, 
                    object::pair{std::string("taskId"), std::string("task-proxy-error")}
                }))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std::string("errorId"), 0}, 
                    object::pair{std::string("status"), std::string("ready")}, 
                    object::pair{std::string("solution"), object{
                        object::pair{std::string("token"), std::string("proxy-error-token")}
                    }}
                }));
                std::async([=]() { expect(capSolver->solveTurnstile(std::string("https://example.com"), std::string("site-key"), std::string("invalid-proxy")))->resolves->toBe(std::string("proxy-error-token")); });
            }
            );
            it(std::string("should retry on task polling errors"), [=]() mutable
            {
                spyOn(axios, std::string("post"))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std::string("errorId"), 0}, 
                    object::pair{std::string("taskId"), std::string("task-retry")}
                }))->mockRejectedValueOnce(std::make_shared<Error>(std::string("Temporary error")))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std::string("errorId"), 0}, 
                    object::pair{std::string("status"), std::string("ready")}, 
                    object::pair{std::string("solution"), object{
                        object::pair{std::string("token"), std::string("retry-token")}
                    }}
                }));
                std::async([=]() { expect(capSolver->solveTurnstile(std::string("https://example.com"), std::string("site-key")))->rejects->toThrow(std::string("Temporary error")); });
            }
            );
        }
        );
    }
    );
    describe(std::string("detectCaptchaType"), [=]() mutable
    {
        shared<any> mockPage;
        beforeEach([=]() mutable
        {
            mockPage = object{
                object::pair{std::string("$"), mock()}, 
                object::pair{std::string("evaluate"), mock()}
            };
        }
        );
        it(std::string("should detect Cloudflare Turnstile"), [=]() mutable
        {
            shared mockElement = object{};
            mockPage["$"]["mockImplementation"]([=](auto selector) mutable
            {
                if (selector == std::string("[data-sitekey]")) {
                    return Promise->resolve(mockElement);
                }
                if (selector == std::string(".cf-turnstile")) {
                    return Promise->resolve(mockElement);
                }
                return Promise->resolve(nullptr);
            }
            );
            mockPage["evaluate"]["mockResolvedValue"](std::string("test-sitekey"));
            auto result = std::async([=]() { detectCaptchaType(mockPage); });
            expect(result)->toEqual(object{
                object::pair{std::string("type"), std::string("turnstile")}, 
                object::pair{std::string("siteKey"), std::string("test-sitekey")}
            });
        }
        );
        it(std::string("should detect reCAPTCHA v2"), [=]() mutable
        {
            shared mockElement = object{};
            mockPage["$"]["mockImplementation"]([=](auto selector) mutable
            {
                if (selector == std::string("[data-sitekey], .g-recaptcha")) {
                    return Promise->resolve(mockElement);
                }
                return Promise->resolve(nullptr);
            }
            );
            mockPage["evaluate"]["mockResolvedValueOnce"](std::string("recaptcha-sitekey"))["mockResolvedValueOnce"](false);
            auto result = std::async([=]() { detectCaptchaType(mockPage); });
            expect(result)->toEqual(object{
                object::pair{std::string("type"), std::string("recaptcha-v2")}, 
                object::pair{std::string("siteKey"), std::string("recaptcha-sitekey")}
            });
        }
        );
        it(std::string("should detect reCAPTCHA v3"), [=]() mutable
        {
            shared mockElement = object{};
            mockPage["$"]["mockImplementation"]([=](auto selector) mutable
            {
                if (selector == std::string("[data-sitekey], .g-recaptcha")) {
                    return Promise->resolve(mockElement);
                }
                return Promise->resolve(nullptr);
            }
            );
            mockPage["evaluate"]["mockResolvedValueOnce"](std::string("recaptcha-sitekey"))["mockResolvedValueOnce"](true);
            auto result = std::async([=]() { detectCaptchaType(mockPage); });
            expect(result)->toEqual(object{
                object::pair{std::string("type"), std::string("recaptcha-v3")}, 
                object::pair{std::string("siteKey"), std::string("recaptcha-sitekey")}
            });
        }
        );
        it(std::string("should detect hCaptcha"), [=]() mutable
        {
            shared mockElement = object{};
            mockPage["$"]["mockImplementation"]([=](auto selector) mutable
            {
                if (selector == std::string("[data-sitekey].h-captcha, [data-hcaptcha-sitekey]")) {
                    return Promise->resolve(mockElement);
                }
                return Promise->resolve(nullptr);
            }
            );
            mockPage["evaluate"]["mockResolvedValue"](std::string("hcaptcha-sitekey"));
            auto result = std::async([=]() { detectCaptchaType(mockPage); });
            expect(result)->toEqual(object{
                object::pair{std::string("type"), std::string("hcaptcha")}, 
                object::pair{std::string("siteKey"), std::string("hcaptcha-sitekey")}
            });
        }
        );
        it(std::string("should return null when no captcha found"), [=]() mutable
        {
            mockPage["$"]["mockResolvedValue"](nullptr);
            auto result = std::async([=]() { detectCaptchaType(mockPage); });
            expect(result)->toEqual(object{
                object::pair{std::string("type"), nullptr}
            });
        }
        );
    }
    );
    describe(std::string("injectCaptchaSolution"), [=]() mutable
    {
        shared<any> mockPage;
        beforeEach([=]() mutable
        {
            mockPage = object{
                object::pair{std::string("evaluate"), mock()}
            };
        }
        );
        it(std::string("should inject Turnstile solution"), [=]() mutable
        {
            std::async([=]() { injectCaptchaSolution(mockPage, std::string("turnstile"), std::string("test-token")); });
            expect(mockPage["evaluate"])->toHaveBeenCalledWith(expect->any(Function), std::string("test-token"));
        }
        );
        it(std::string("should inject reCAPTCHA solution"), [=]() mutable
        {
            std::async([=]() { injectCaptchaSolution(mockPage, std::string("recaptcha-v2"), std::string("test-token")); });
            expect(mockPage["evaluate"])->toHaveBeenCalledWith(expect->any(Function), std::string("test-token"));
        }
        );
        it(std::string("should inject hCaptcha solution"), [=]() mutable
        {
            std::async([=]() { injectCaptchaSolution(mockPage, std::string("hcaptcha"), std::string("test-token")); });
            expect(mockPage["evaluate"])->toHaveBeenCalledWith(expect->any(Function), std::string("test-token"));
        }
        );
    }
    );
}

MAIN
