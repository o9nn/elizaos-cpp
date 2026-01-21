#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-bootstrap/src/__tests__/services.test.h"

std::function<array<std::shared_ptr<PluginService>>()> getPluginServices = [=]() mutable
{
    return as<array<std::shared_ptr<PluginService>>>(as<any>((OR((bootstrapPlugin->services), (array<any>())))));
};

void Main(void)
{
    describe(std::string("TaskService"), [=]() mutable
    {
        shared<MockRuntime> mockRuntime;
        shared<std::shared_ptr<TaskService>> taskService;
        shared<array<object>> mockTasks;
        beforeEach([=]() mutable
        {
            auto setup = setupActionTest();
            mockRuntime = setup["mockRuntime"];
            mockTasks = array<object>{ object{
                object::pair{std::string("id"), std::string("task-1")}, 
                object::pair{std::string("name"), std::string("Complete first task")}, 
                object::pair{std::string("description"), std::string("First test task")}, 
                object::pair{std::string("status"), std::string("PENDING")}, 
                object::pair{std::string("createdAt"), ((std::make_shared<Date>(Date->now() - 10000)))->toISOString()}, 
                object::pair{std::string("updatedAt"), ((std::make_shared<Date>(Date->now() - 5000)))->toISOString()}, 
                object::pair{std::string("tags"), array<string>{ std::string("queue") }}
            }, object{
                object::pair{std::string("id"), std::string("task-2")}, 
                object::pair{std::string("name"), std::string("Make a decision")}, 
                object::pair{std::string("description"), std::string("Choose between options")}, 
                object::pair{std::string("status"), std::string("PENDING")}, 
                object::pair{std::string("createdAt"), ((std::make_shared<Date>(Date->now() - 20000)))->toISOString()}, 
                object::pair{std::string("updatedAt"), ((std::make_shared<Date>(Date->now() - 15000)))->toISOString()}, 
                object::pair{std::string("tags"), array<string>{ std::string("queue") }}, 
                object::pair{std::string("metadata"), object{
                    object::pair{std::string("options"), array<object>{ object{
                        object::pair{std::string("name"), std::string("Option A")}, 
                        object::pair{std::string("description"), std::string("First option")}
                    }, object{
                        object::pair{std::string("name"), std::string("Option B")}, 
                        object::pair{std::string("description"), std::string("Second option")}
                    } }}
                }}
            } };
            mockRuntime->getTasks = mock()->mockResolvedValue(mockTasks);
            taskService = std::make_shared<TaskService>(as<std::shared_ptr<IAgentRuntime>>(mockRuntime));
        }
        );
        afterEach([=]() mutable
        {
            mock->restore();
        }
        );
        it(std::string("should be instantiated with a runtime"), [=]() mutable
        {
            expect(taskService)->toBeDefined();
            expect(taskService)->toBeInstanceOf(TaskService);
            expect(TaskService)->toHaveProperty(std::string("serviceType"));
            expect(TaskService::serviceType)->toBe(ServiceType->TASK);
            expect(taskService)->toHaveProperty(std::string("runtime"));
            expect(taskService)->toHaveProperty(std::string("stop"));
            expect(type_of(std::bind(&TaskService::stop, taskService)))->toBe(std::string("function"));
        }
        );
        it(std::string("should start the service successfully"), [=]() mutable
        {
            auto startPromise = TaskService::start(as<std::shared_ptr<IAgentRuntime>>(mockRuntime));
            expect(startPromise)->toBeInstanceOf(Promise);
            auto service = std::async([=]() { startPromise; });
            expect(service)->toBeDefined();
            expect(service)->toBeInstanceOf(TaskService);
            expect((as<any>(service))["runtime"])->toBe(mockRuntime);
        }
        );
        it(std::string("should retrieve pending tasks correctly"), [=]() mutable
        {
            auto checkTasksMethod = (as<any>(taskService))["checkTasks"]["bind"](taskService);
            std::async([=]() { checkTasksMethod(); });
            expect(mockRuntime->getTasks)->toHaveBeenCalledWith(object{
                object::pair{std::string("tags"), array<string>{ std::string("queue") }}
            });
        }
        );
        it(std::string("should process tasks that are ready"), [=]() mutable
        {
            auto pastTask = object{
                object::pair{std::string("id"), std::string("past-task")}, 
                object::pair{std::string("name"), std::string("Past scheduled task")}, 
                object::pair{std::string("description"), std::string("This task was scheduled in the past")}, 
                object::pair{std::string("status"), std::string("PENDING")}, 
                object::pair{std::string("createdAt"), ((std::make_shared<Date>(Date->now() - 10000)))->toISOString()}, 
                object::pair{std::string("scheduledFor"), ((std::make_shared<Date>(Date->now() - 5000)))->toISOString()}, 
                object::pair{std::string("tags"), array<string>{ std::string("queue") }}
            };
            mockRuntime->getTasks = mock()->mockResolvedValue(array<object>{ pastTask });
            auto executeTaskMethod = (as<any>(taskService))["executeTask"]["bind"](taskService);
            shared mockWorkerExecute = mock()->mockResolvedValue(undefined);
            mockRuntime->getTaskWorker = as<any>(mock()->mockImplementation([=](auto taskName) mutable
            {
                if (taskName == std::string("Past scheduled task")) {
                    return object{
                        object::pair{std::string("name"), taskName}, 
                        object::pair{std::string("execute"), mockWorkerExecute}, 
                        object::pair{std::string("validate"), mock()->mockResolvedValue(true)}
                    };
                }
                return undefined;
            }
            ));
            std::async([=]() { executeTaskMethod(pastTask); });
            expect(mockRuntime->getTaskWorker)->toHaveBeenCalledWith(pastTask["name"]);
            expect(mockWorkerExecute)->toHaveBeenCalled();
            expect(mockRuntime->deleteTask)->toHaveBeenCalledWith(pastTask["id"]);
        }
        );
        it(std::string("should handle errors during task processing"), [=]() mutable
        {
            auto testTask = object{
                object::pair{std::string("id"), std::string("error-task")}, 
                object::pair{std::string("name"), std::string("Error task")}, 
                object::pair{std::string("description"), std::string("This task will cause an error")}, 
                object::pair{std::string("status"), std::string("PENDING")}, 
                object::pair{std::string("tags"), array<string>{ std::string("queue") }}
            };
            shared mockErrorExecute = mock()->mockRejectedValue(std::make_shared<Error>(std::string("Worker execution error")));
            mockRuntime->getTaskWorker = as<any>(mock()->mockImplementation([=](auto taskName) mutable
            {
                if (taskName == std::string("Error task")) {
                    return object{
                        object::pair{std::string("name"), taskName}, 
                        object::pair{std::string("execute"), mockErrorExecute}, 
                        object::pair{std::string("validate"), mock()->mockResolvedValue(true)}
                    };
                }
                return undefined;
            }
            ));
            spyOn(logger, std::string("error"))->mockImplementation([=]() mutable
            {
            }
            );
            auto executeTaskMethod = (as<any>(taskService))["executeTask"]["bind"](taskService);
            std::async([=]() { executeTaskMethod(testTask); });
            expect(mockRuntime->getTaskWorker)->toHaveBeenCalledWith(testTask["name"]);
            expect(mockErrorExecute)->toHaveBeenCalled();
            expect(logger->error)->toHaveBeenCalledWith(std::string("[Bootstrap] Error executing task ") + testTask["id"] + std::string(":"), expect->any(Error));
        }
        );
    }
    );
    describe(std::string("Service Registry"), [=]() mutable
    {
        shared<MockRuntime> mockRuntime;
        beforeEach([=]() mutable
        {
            mock->restore();
            spyOn(logger, std::string("warn"))->mockImplementation([=]() mutable
            {
            }
            );
            auto setup = setupActionTest();
            mockRuntime = setup["mockRuntime"];
        }
        );
        afterEach([=]() mutable
        {
            mock->restore();
        }
        );
        it(std::string("should register all services correctly"), [=]() mutable
        {
            auto services = getPluginServices();
            expect(services)->toBeDefined();
            expect(services->get_length())->toBeGreaterThan(0);
            services->forEach([=](auto serviceDefinitionOrClass) mutable
            {
                if (type_of(serviceDefinitionOrClass) == std::string("function")) {
                    auto serviceClass = as<any>(serviceDefinitionOrClass);
                    expect(serviceClass)->toHaveProperty(std::string("serviceType"));
                    expect(type_of(serviceClass["serviceType"]))->toBe(std::string("string"));
                    expect(serviceClass)->toHaveProperty(std::string("name"));
                    expect(type_of(serviceClass["name"]))->toBe(std::string("string"));
                    expect(serviceClass)->toHaveProperty(std::string("start"));
                    expect(type_of(serviceClass["start"]))->toBe(std::string("function"));
                } else {
                    auto serviceDesc = as<std::shared_ptr<PluginService>>(serviceDefinitionOrClass);
                    expect(serviceDesc)->toHaveProperty(std::string("type"));
                    expect(type_of(serviceDesc->type))->toBe(std::string("string"));
                    expect(serviceDesc)->toHaveProperty(std::string("name"));
                    expect(type_of(serviceDesc->name))->toBe(std::string("string"));
                    expect(serviceDesc)->toHaveProperty(std::string("init"));
                    expect(type_of(serviceDesc->init))->toBe(std::string("function"));
                }
            }
            );
        }
        );
        it(std::string("should initialize file service if available"), [=]() mutable
        {
            auto services = getPluginServices();
            auto fileServiceDefinition = services->find([=](auto s) mutable
            {
                if (type_of(s) == std::string("function")) return (as<any>(s))["serviceType"] == std::string("file");
                return (as<std::shared_ptr<PluginService>>(s))->type == std::string("file");
            }
            );
            if (fileServiceDefinition) {
                auto serviceInstance = (type_of(fileServiceDefinition) == std::string("function")) ? std::async([=]() { (as<any>(fileServiceDefinition))["start"](as<std::shared_ptr<IAgentRuntime>>(mockRuntime)); }) : std::async([=]() { (as<std::shared_ptr<PluginService>>(fileServiceDefinition))->init(as<std::shared_ptr<IAgentRuntime>>(mockRuntime)); });
                expect(serviceInstance)->toBeDefined();
                expect(serviceInstance)->toHaveProperty(std::string("uploadFile"));
                expect(serviceInstance)->toHaveProperty(std::string("getFile"));
                expect(serviceInstance)->toHaveProperty(std::string("listFiles"));
                expect(serviceInstance)->toHaveProperty(std::string("deleteFile"));
                expect(type_of(serviceInstance["uploadFile"]))->toBe(std::string("function"));
                expect(type_of(serviceInstance["getFile"]))->toBe(std::string("function"));
                expect(type_of(serviceInstance["listFiles"]))->toBe(std::string("function"));
                expect(type_of(serviceInstance["deleteFile"]))->toBe(std::string("function"));
            }
        }
        );
        it(std::string("should initialize PDF service if available"), [=]() mutable
        {
            auto services = getPluginServices();
            auto pdfServiceDefinition = services->find([=](auto s) mutable
            {
                if (type_of(s) == std::string("function")) return (as<any>(s))["serviceType"] == ServiceType->PDF;
                return (as<std::shared_ptr<PluginService>>(s))->type == ServiceType->PDF;
            }
            );
            if (pdfServiceDefinition) {
                auto serviceInstance = (type_of(pdfServiceDefinition) == std::string("function")) ? std::async([=]() { (as<any>(pdfServiceDefinition))["start"](as<std::shared_ptr<IAgentRuntime>>(mockRuntime)); }) : std::async([=]() { (as<std::shared_ptr<PluginService>>(pdfServiceDefinition))->init(as<std::shared_ptr<IAgentRuntime>>(mockRuntime)); });
                expect(serviceInstance)->toBeDefined();
                expect(serviceInstance)->toHaveProperty(std::string("extractText"));
                expect(type_of(serviceInstance["extractText"]))->toBe(std::string("function"));
            }
        }
        );
        it(std::string("should initialize image service if available"), [=]() mutable
        {
            auto services = getPluginServices();
            auto imageServiceDefinition = services->find([=](auto s) mutable
            {
                if (type_of(s) == std::string("function")) return (as<any>(s))["serviceType"] == std::string("image");
                return (as<std::shared_ptr<PluginService>>(s))->type == std::string("image");
            }
            );
            if (imageServiceDefinition) {
                auto serviceInstance = (type_of(imageServiceDefinition) == std::string("function")) ? std::async([=]() { (as<any>(imageServiceDefinition))["start"](as<std::shared_ptr<IAgentRuntime>>(mockRuntime)); }) : std::async([=]() { (as<std::shared_ptr<PluginService>>(imageServiceDefinition))->init(as<std::shared_ptr<IAgentRuntime>>(mockRuntime)); });
                expect(serviceInstance)->toBeDefined();
                expect(serviceInstance)->toHaveProperty(std::string("describeImage"));
                expect(type_of(serviceInstance["describeImage"]))->toBe(std::string("function"));
            }
        }
        );
        it(std::string("should initialize browser service if available"), [=]() mutable
        {
            auto services = getPluginServices();
            auto browserServiceDefinition = services->find([=](auto s) mutable
            {
                if (type_of(s) == std::string("function")) return (as<any>(s))["serviceType"] == ServiceType->BROWSER;
                return (as<std::shared_ptr<PluginService>>(s))->type == ServiceType->BROWSER;
            }
            );
            if (browserServiceDefinition) {
                auto serviceInstance = (type_of(browserServiceDefinition) == std::string("function")) ? std::async([=]() { (as<any>(browserServiceDefinition))["start"](as<std::shared_ptr<IAgentRuntime>>(mockRuntime)); }) : std::async([=]() { (as<std::shared_ptr<PluginService>>(browserServiceDefinition))->init(as<std::shared_ptr<IAgentRuntime>>(mockRuntime)); });
                expect(serviceInstance)->toBeDefined();
                expect(serviceInstance)->toHaveProperty(std::string("browse"));
                expect(type_of(serviceInstance["browse"]))->toBe(std::string("function"));
            }
        }
        );
        it(std::string("should handle service initialization errors gracefully"), [=]() mutable
        {
            auto services = getPluginServices();
            auto fileServiceDefinition = services->find([=](auto s) mutable
            {
                if (type_of(s) == std::string("function")) return (as<any>(s))["serviceType"] == std::string("file");
                return (as<std::shared_ptr<PluginService>>(s))->type == std::string("file");
            }
            );
            if (fileServiceDefinition) {
                mockRuntime->getService = mock()->mockImplementation([=]() mutable
                {
                    throw any(std::make_shared<Error>(std::string("Service initialization failed")));
                }
                );
                auto serviceInstance = (type_of(fileServiceDefinition) == std::string("function")) ? std::async([=]() { (as<any>(fileServiceDefinition))["start"](as<std::shared_ptr<IAgentRuntime>>(mockRuntime)); }) : std::async([=]() { (as<std::shared_ptr<PluginService>>(fileServiceDefinition))->init(as<std::shared_ptr<IAgentRuntime>>(mockRuntime)); });
                expect(serviceInstance)->toBeDefined();
                expect(logger->warn)->toHaveBeenCalled();
                expect(serviceInstance)->toHaveProperty(std::string("uploadFile"));
                expect(serviceInstance)->toHaveProperty(std::string("getFile"));
                expect(serviceInstance)->toHaveProperty(std::string("listFiles"));
                expect(serviceInstance)->toHaveProperty(std::string("deleteFile"));
            }
        }
        );
    }
    );
}

MAIN
