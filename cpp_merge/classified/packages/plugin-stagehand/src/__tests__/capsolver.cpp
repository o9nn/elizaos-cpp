#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-stagehand/src/__tests__/capsolver.test.h"

std::function<object(any)> createMockResponse = [=](auto data) mutable
{
    return (object{
        object::pair{std:("data"), std:("data")}, 
        object::pair{std:("status"), 200}, 
        object::pair{std:("statusText"), std:("OK")}, 
        object::pair{std:("headers"), object{}}, 
        object::pair{std:("config"), as<any>(object{
            object::pair{std:("url"), string_empty}, 
            object::pair{std:("method"), std:("post")}, 
            object::pair{std:("headers"), object{}}
        })}, 
        object::pair{std:("request"), object{}}
    });
};

void Main(void)
{
    mock->module(std:("@elizaos/core"), [=]() mutable
    {
        return (object{
            object::pair{std:("logger"), object{
                object::pair{std:("info"), mock()}, 
                object::pair{std:("error"), mock()}, 
                object::pair{std:("warn"), mock()}, 
                object::pair{std:("debug"), mock()}
            }}
        });
    }
    );
    describe(std:("CapSolverService"), [=]() mutable
    {
        shared<std::shared_ptr<CapSolverService>> capSolver;
        shared mockApiKey = std:("test-api-key");
        beforeEach([=]() mutable
        {
            capSolver = std::make_shared<CapSolverService>(object{
                object::pair{std:("apiKey"), mockApiKey}
            });
        }
        );
        afterEach([=]() mutable
        {
            mock->restore();
        }
        );
        describe(std:("createTask"), [=]() mutable
        {
            it(std:("should create a task successfully"), [=]() mutable
            {
                auto mockTaskId = std:("task-123");
                auto postSpy = spyOn(axios, std:("post"))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std:("errorId"), 0}, 
                    object::pair{std:("taskId"), mockTaskId}
                }));
                auto task = object{
                    object::pair{std:("type"), std:("AntiTurnstileTaskProxyLess")}, 
                    object::pair{std:("websiteURL"), std:("https://example.com")}, 
                    object::pair{std:("websiteKey"), std:("test-key")}
                };
                auto taskId = std::async([=]() { capSolver->createTask(task); });
                expect(taskId)->toBe(mockTaskId);
                expect(postSpy)->toHaveBeenCalledWith(std:("https://api.capsolver.com/createTask"), object{
                    object::pair{std:("clientKey"), mockApiKey}, 
                    object::pair{std:("task"), std:("task")}
                }, expect->any(Object));
            }
            );
            it(std:("should throw error when API returns error"), [=]() mutable
            {
                spyOn(axios, std:("post"))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std:("errorId"), 1}, 
                    object::pair{std:("errorDescription"), std:("Invalid API key")}
                }));
                auto task = object{
                    object::pair{std:("type"), std:("AntiTurnstileTaskProxyLess")}, 
                    object::pair{std:("websiteURL"), std:("https://example.com")}, 
                    object::pair{std:("websiteKey"), std:("test-key")}
                };
                std::async([=]() { expect(capSolver->createTask(task))->rejects->toThrow(std:("CapSolver error: Invalid API key")); });
            }
            );
        }
        );
        describe(std:("getTaskResult"), [=]() mutable
        {
            it(std:("should return solution when task is ready"), [=]() mutable
            {
                auto mockSolution = object{
                    object::pair{std:("token"), std:("solved-token")}
                };
                spyOn(axios, std:("post"))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std:("errorId"), 0}, 
                    object::pair{std:("status"), std:("ready")}, 
                    object::pair{std:("solution"), mockSolution}
                }));
                auto result = std::async([=]() { capSolver->getTaskResult(std:("task-123")); });
                expect(result)->toEqual(mockSolution);
                expect(axios->post)->toHaveBeenCalledWith(std:("https://api.capsolver.com/getTaskResult"), object{
                    object::pair{std:("clientKey"), mockApiKey}, 
                    object::pair{std:("taskId"), std:("task-123")}
                }, expect->any(Object));
            }
            );
            it(std:("should poll until task is ready"), [=]() mutable
            {
                spyOn(axios, std:("post"))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std:("errorId"), 0}, 
                    object::pair{std:("status"), std:("processing")}
                }))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std:("errorId"), 0}, 
                    object::pair{std:("status"), std:("ready")}, 
                    object::pair{std:("solution"), object{
                        object::pair{std:("token"), std:("solved-token")}
                    }}
                }));
                auto fastCapSolver = std::make_shared<CapSolverService>(object{
                    object::pair{std:("apiKey"), mockApiKey}, 
                    object::pair{std:("pollingInterval"), 10}
                });
                auto result = std::async([=]() { fastCapSolver->getTaskResult(std:("task-123")); });
                expect(result)->toEqual(object{
                    object::pair{std:("token"), std:("solved-token")}
                });
                expect(axios->post)->toHaveBeenCalledTimes(2);
            }
            );
            it(std:("should throw error on timeout"), [=]() mutable
            {
                spyOn(axios, std:("post"))->mockResolvedValue(createMockResponse(object{
                    object::pair{std:("errorId"), 0}, 
                    object::pair{std:("status"), std:("processing")}
                }));
                auto fastCapSolver = std::make_shared<CapSolverService>(object{
                    object::pair{std:("apiKey"), mockApiKey}, 
                    object::pair{std:("pollingInterval"), 10}, 
                    object::pair{std:("retryAttempts"), 2}
                });
                std::async([=]() { expect(fastCapSolver->getTaskResult(std:("task-123")))->rejects->toThrow(std:("CapSolver task timeout")); });
            }
            );
        }
        );
        describe(std:("solveTurnstile"), [=]() mutable
        {
            it(std:("should solve Turnstile captcha"), [=]() mutable
            {
                auto mockTaskId = std:("task-123");
                auto mockToken = std:("turnstile-token");
                spyOn(axios, std:("post"))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std:("errorId"), 0}, 
                    object::pair{std:("taskId"), mockTaskId}
                }))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std:("errorId"), 0}, 
                    object::pair{std:("status"), std:("ready")}, 
                    object::pair{std:("solution"), object{
                        object::pair{std:("token"), mockToken}
                    }}
                }));
                auto token = std::async([=]() { capSolver->solveTurnstile(std:("https://example.com"), std:("site-key")); });
                expect(token)->toBe(mockToken);
                expect(logger->info)->toHaveBeenCalledWith(std:("Solving Cloudflare Turnstile captcha"));
            }
            );
            it(std:("should use proxy when provided"), [=]() mutable
            {
                spyOn(axios, std:("post"))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std:("errorId"), 0}, 
                    object::pair{std:("taskId"), std:("task-123")}
                }))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std:("errorId"), 0}, 
                    object::pair{std:("status"), std:("ready")}, 
                    object::pair{std:("solution"), object{
                        object::pair{std:("token"), std:("proxy-token")}
                    }}
                }));
                std::async([=]() { capSolver->solveTurnstile(std:("https://example.com"), std:("site-key"), std:("proxy-host:8080:username:password")); });
                expect(axios->post)->toHaveBeenCalledWith(std:("https://api.capsolver.com/createTask"), expect->objectContaining(object{
                    object::pair{std:("task"), expect->objectContaining(object{
                        object::pair{std:("type"), std:("AntiTurnstileTask")}, 
                        object::pair{std:("proxy"), std:("proxy-host:8080")}, 
                        object::pair{std:("proxyLogin"), std:("username")}, 
                        object::pair{std:("proxyPassword"), std:("password")}
                    })}
                }), expect->any(Object));
            }
            );
        }
        );
        describe(std:("solveRecaptchaV2"), [=]() mutable
        {
            it(std:("should solve reCAPTCHA v2"), [=]() mutable
            {
                spyOn(axios, std:("post"))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std:("errorId"), 0}, 
                    object::pair{std:("taskId"), std:("task-456")}
                }))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std:("errorId"), 0}, 
                    object::pair{std:("status"), std:("ready")}, 
                    object::pair{std:("solution"), object{
                        object::pair{std:("gRecaptchaResponse"), std:("recaptcha-v2-token")}
                    }}
                }));
                auto result = std::async([=]() { capSolver->solveRecaptchaV2(std:("https://example.com"), std:("v2-site-key")); });
                expect(result)->toBe(std:("recaptcha-v2-token"));
                expect(logger->info)->toHaveBeenCalledWith(std:("Solving reCAPTCHA v2"));
            }
            );
            it(std:("should handle invisible reCAPTCHA v2"), [=]() mutable
            {
                spyOn(axios, std:("post"))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std:("errorId"), 0}, 
                    object::pair{std:("taskId"), std:("task-789")}
                }))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std:("errorId"), 0}, 
                    object::pair{std:("status"), std:("ready")}, 
                    object::pair{std:("solution"), object{
                        object::pair{std:("gRecaptchaResponse"), std:("invisible-token")}
                    }}
                }));
                auto result = std::async([=]() { capSolver->solveRecaptchaV2(std:("https://example.com"), std:("invisible-key"), true); });
                expect(result)->toBe(std:("invisible-token"));
                expect(axios->post)->toHaveBeenCalledWith(std:("https://api.capsolver.com/createTask"), expect->objectContaining(object{
                    object::pair{std:("task"), expect->objectContaining(object{
                        object::pair{std:("isInvisible"), true}
                    })}
                }), expect->any(Object));
            }
            );
        }
        );
        describe(std:("solveRecaptchaV3"), [=]() mutable
        {
            it(std:("should solve reCAPTCHA v3"), [=]() mutable
            {
                spyOn(axios, std:("post"))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std:("errorId"), 0}, 
                    object::pair{std:("taskId"), std:("task-v3")}
                }))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std:("errorId"), 0}, 
                    object::pair{std:("status"), std:("ready")}, 
                    object::pair{std:("solution"), object{
                        object::pair{std:("gRecaptchaResponse"), std:("v3-token")}
                    }}
                }));
                auto result = std::async([=]() { capSolver->solveRecaptchaV3(std:("https://example.com"), std:("v3-key"), std:("verify")); });
                expect(result)->toBe(std:("v3-token"));
                expect(logger->info)->toHaveBeenCalledWith(std:("Solving reCAPTCHA v3"));
            }
            );
            it(std:("should use custom action and score"), [=]() mutable
            {
                spyOn(axios, std:("post"))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std:("errorId"), 0}, 
                    object::pair{std:("taskId"), std:("task-v3-custom")}
                }))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std:("errorId"), 0}, 
                    object::pair{std:("status"), std:("ready")}, 
                    object::pair{std:("solution"), object{
                        object::pair{std:("gRecaptchaResponse"), std:("v3-custom-token")}
                    }}
                }));
                auto result = std::async([=]() { capSolver->solveRecaptchaV3(std:("https://example.com"), std:("v3-key"), std:("login"), 0.7); });
                expect(result)->toBe(std:("v3-custom-token"));
                expect(axios->post)->toHaveBeenCalledWith(std:("https://api.capsolver.com/createTask"), expect->objectContaining(object{
                    object::pair{std:("task"), expect->objectContaining(object{
                        object::pair{std:("pageAction"), std:("login")}, 
                        object::pair{std:("minScore"), 0.7}
                    })}
                }), expect->any(Object));
            }
            );
        }
        );
        describe(std:("solveHCaptcha"), [=]() mutable
        {
            it(std:("should solve hCaptcha"), [=]() mutable
            {
                spyOn(axios, std:("post"))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std:("errorId"), 0}, 
                    object::pair{std:("taskId"), std:("task-hcaptcha")}
                }))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std:("errorId"), 0}, 
                    object::pair{std:("status"), std:("ready")}, 
                    object::pair{std:("solution"), object{
                        object::pair{std:("token"), std:("hcaptcha-token")}
                    }}
                }));
                auto result = std::async([=]() { capSolver->solveHCaptcha(std:("https://example.com"), std:("hcaptcha-key")); });
                expect(result)->toBe(std:("hcaptcha-token"));
                expect(logger->info)->toHaveBeenCalledWith(std:("Solving hCaptcha"));
            }
            );
            it(std:("should use proxy when provided"), [=]() mutable
            {
                spyOn(axios, std:("post"))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std:("errorId"), 0}, 
                    object::pair{std:("taskId"), std:("task-hcaptcha-proxy")}
                }))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std:("errorId"), 0}, 
                    object::pair{std:("status"), std:("ready")}, 
                    object::pair{std:("solution"), object{
                        object::pair{std:("token"), std:("hcaptcha-proxy-token")}
                    }}
                }));
                std::async([=]() { capSolver->solveHCaptcha(std:("https://example.com"), std:("hcaptcha-key"), std:("proxy-host:8080:username:password")); });
                expect(axios->post)->toHaveBeenCalledWith(std:("https://api.capsolver.com/createTask"), expect->objectContaining(object{
                    object::pair{std:("task"), expect->objectContaining(object{
                        object::pair{std:("type"), std:("HCaptchaTask")}, 
                        object::pair{std:("proxy"), std:("proxy-host:8080")}, 
                        object::pair{std:("proxyLogin"), std:("username")}, 
                        object::pair{std:("proxyPassword"), std:("password")}
                    })}
                }), expect->any(Object));
            }
            );
        }
        );
        describe(std:("error handling"), [=]() mutable
        {
            it(std:("should handle network errors"), [=]() mutable
            {
                spyOn(axios, std:("post"))->mockRejectedValueOnce(std::make_shared<Error>(std:("Network error")));
                std::async([=]() { expect(capSolver->createTask(object{
                    object::pair{std:("type"), std:("AntiTurnstileTaskProxyLess")}, 
                    object::pair{std:("websiteURL"), std:("https://example.com")}, 
                    object::pair{std:("websiteKey"), std:("test-key")}
                }))->rejects->toThrow(std:("Network error")); });
                expect(logger->error)->toHaveBeenCalledWith(std:("Error creating CapSolver task:"), expect->any(Error));
            }
            );
            it(std:("should handle invalid proxy format"), [=]() mutable
            {
                spyOn(axios, std:("post"))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std:("errorId"), 0}, 
                    object::pair{std:("taskId"), std:("task-proxy-error")}
                }))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std:("errorId"), 0}, 
                    object::pair{std:("status"), std:("ready")}, 
                    object::pair{std:("solution"), object{
                        object::pair{std:("token"), std:("proxy-error-token")}
                    }}
                }));
                std::async([=]() { expect(capSolver->solveTurnstile(std:("https://example.com"), std:("site-key"), std:("invalid-proxy")))->resolves->toBe(std:("proxy-error-token")); });
            }
            );
            it(std:("should retry on task polling errors"), [=]() mutable
            {
                spyOn(axios, std:("post"))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std:("errorId"), 0}, 
                    object::pair{std:("taskId"), std:("task-retry")}
                }))->mockRejectedValueOnce(std::make_shared<Error>(std:("Temporary error")))->mockResolvedValueOnce(createMockResponse(object{
                    object::pair{std:("errorId"), 0}, 
                    object::pair{std:("status"), std:("ready")}, 
                    object::pair{std:("solution"), object{
                        object::pair{std:("token"), std:("retry-token")}
                    }}
                }));
                std::async([=]() { expect(capSolver->solveTurnstile(std:("https://example.com"), std:("site-key")))->rejects->toThrow(std:("Temporary error")); });
            }
            );
        }
        );
    }
    );
    describe(std:("detectCaptchaType"), [=]() mutable
    {
        shared<any> mockPage;
        beforeEach([=]() mutable
        {
            mockPage = object{
                object::pair{std:("$"), mock()}, 
                object::pair{std:("evaluate"), mock()}
            };
        }
        );
        it(std:("should detect Cloudflare Turnstile"), [=]() mutable
        {
            shared mockElement = object{};
            mockPage["$"]["mockImplementation"]([=](auto selector) mutable
            {
                if (selector == std:("[data-sitekey]")) {
                    return Promise->resolve(mockElement);
                }
                if (selector == std:(".cf-turnstile")) {
                    return Promise->resolve(mockElement);
                }
                return Promise->resolve(nullptr);
            }
            );
            mockPage["evaluate"]["mockResolvedValue"](std:("test-sitekey"));
            auto result = std::async([=]() { detectCaptchaType(mockPage); });
            expect(result)->toEqual(object{
                object::pair{std:("type"), std:("turnstile")}, 
                object::pair{std:("siteKey"), std:("test-sitekey")}
            });
        }
        );
        it(std:("should detect reCAPTCHA v2"), [=]() mutable
        {
            shared mockElement = object{};
            mockPage["$"]["mockImplementation"]([=](auto selector) mutable
            {
                if (selector == std:("[data-sitekey], .g-recaptcha")) {
                    return Promise->resolve(mockElement);
                }
                return Promise->resolve(nullptr);
            }
            );
            mockPage["evaluate"]["mockResolvedValueOnce"](std:("recaptcha-sitekey"))["mockResolvedValueOnce"](false);
            auto result = std::async([=]() { detectCaptchaType(mockPage); });
            expect(result)->toEqual(object{
                object::pair{std:("type"), std:("recaptcha-v2")}, 
                object::pair{std:("siteKey"), std:("recaptcha-sitekey")}
            });
        }
        );
        it(std:("should detect reCAPTCHA v3"), [=]() mutable
        {
            shared mockElement = object{};
            mockPage["$"]["mockImplementation"]([=](auto selector) mutable
            {
                if (selector == std:("[data-sitekey], .g-recaptcha")) {
                    return Promise->resolve(mockElement);
                }
                return Promise->resolve(nullptr);
            }
            );
            mockPage["evaluate"]["mockResolvedValueOnce"](std:("recaptcha-sitekey"))["mockResolvedValueOnce"](true);
            auto result = std::async([=]() { detectCaptchaType(mockPage); });
            expect(result)->toEqual(object{
                object::pair{std:("type"), std:("recaptcha-v3")}, 
                object::pair{std:("siteKey"), std:("recaptcha-sitekey")}
            });
        }
        );
        it(std:("should detect hCaptcha"), [=]() mutable
        {
            shared mockElement = object{};
            mockPage["$"]["mockImplementation"]([=](auto selector) mutable
            {
                if (selector == std:("[data-sitekey].h-captcha, [data-hcaptcha-sitekey]")) {
                    return Promise->resolve(mockElement);
                }
                return Promise->resolve(nullptr);
            }
            );
            mockPage["evaluate"]["mockResolvedValue"](std:("hcaptcha-sitekey"));
            auto result = std::async([=]() { detectCaptchaType(mockPage); });
            expect(result)->toEqual(object{
                object::pair{std:("type"), std:("hcaptcha")}, 
                object::pair{std:("siteKey"), std:("hcaptcha-sitekey")}
            });
        }
        );
        it(std:("should return null when no captcha found"), [=]() mutable
        {
            mockPage["$"]["mockResolvedValue"](nullptr);
            auto result = std::async([=]() { detectCaptchaType(mockPage); });
            expect(result)->toEqual(object{
                object::pair{std:("type"), nullptr}
            });
        }
        );
    }
    );
    describe(std:("injectCaptchaSolution"), [=]() mutable
    {
        shared<any> mockPage;
        beforeEach([=]() mutable
        {
            mockPage = object{
                object::pair{std:("evaluate"), mock()}
            };
        }
        );
        it(std:("should inject Turnstile solution"), [=]() mutable
        {
            std::async([=]() { injectCaptchaSolution(mockPage, std:("turnstile"), std:("test-token")); });
            expect(mockPage["evaluate"])->toHaveBeenCalledWith(expect->any(Function), std:("test-token"));
        }
        );
        it(std:("should inject reCAPTCHA solution"), [=]() mutable
        {
            std::async([=]() { injectCaptchaSolution(mockPage, std:("recaptcha-v2"), std:("test-token")); });
            expect(mockPage["evaluate"])->toHaveBeenCalledWith(expect->any(Function), std:("test-token"));
        }
        );
        it(std:("should inject hCaptcha solution"), [=]() mutable
        {
            std::async([=]() { injectCaptchaSolution(mockPage, std:("hcaptcha"), std:("test-token")); });
            expect(mockPage["evaluate"])->toHaveBeenCalledWith(expect->any(Function), std:("test-token"));
        }
        );
    }
    );
}

MAIN
