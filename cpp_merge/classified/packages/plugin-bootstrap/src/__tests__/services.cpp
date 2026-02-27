#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-bootstrap/src/__tests__/services.test.h"

std::function<array<std::shared_ptr<PluginService>>()> getPluginServices = [=]() mutable
{
    return as<array<std::shared_ptr<PluginService>>>(as<any>((OR((bootstrapPlugin->services), (array<any>())))));
};

void Main(void)
{
    describe(std:("TaskService"), [=]() mutable
    {
        shared<MockRuntime> mockRuntime;
        shared<std::shared_ptr<TaskService>> taskService;
        shared<array<object>> mockTasks;
        beforeEach([=]() mutable
        {
            auto setup = setupActionTest();
            mockRuntime = setup["mockRuntime"];
            mockTasks = array<object>{ object{
                object::pair{std:("id"), std:("task-1")}, 
                object::pair{std:("name"), std:("Complete first task")}, 
                object::pair{std:("description"), std:("First test task")}, 
                object::pair{std:("status"), std:("PENDING")}, 
                object::pair{std:("createdAt"), ((std::make_shared<Date>(Date->now() - 10000)))->toISOString()}, 
                object::pair{std:("updatedAt"), ((std::make_shared<Date>(Date->now() - 5000)))->toISOString()}, 
                object::pair{std:("tags"), array<string>{ std:("queue") }}
            }, object{
                object::pair{std:("id"), std:("task-2")}, 
                object::pair{std:("name"), std:("Make a decision")}, 
                object::pair{std:("description"), std:("Choose between options")}, 
                object::pair{std:("status"), std:("PENDING")}, 
                object::pair{std:("createdAt"), ((std::make_shared<Date>(Date->now() - 20000)))->toISOString()}, 
                object::pair{std:("updatedAt"), ((std::make_shared<Date>(Date->now() - 15000)))->toISOString()}, 
                object::pair{std:("tags"), array<string>{ std:("queue") }}, 
                object::pair{std:("metadata"), object{
                    object::pair{std:("options"), array<object>{ object{
                        object::pair{std:("name"), std:("Option A")}, 
                        object::pair{std:("description"), std:("First option")}
                    }, object{
                        object::pair{std:("name"), std:("Option B")}, 
                        object::pair{std:("description"), std:("Second option")}
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
        it(std:("should be instantiated with a runtime"), [=]() mutable
        {
            expect(taskService)->toBeDefined();
            expect(taskService)->toBeInstanceOf(TaskService);
            expect(TaskService)->toHaveProperty(std:("serviceType"));
            expect(TaskService::serviceType)->toBe(ServiceType->TASK);
            expect(taskService)->toHaveProperty(std:("runtime"));
            expect(taskService)->toHaveProperty(std:("stop"));
            expect(type_of(std::bind(&TaskService::stop, taskService)))->toBe(std:("function"));
        }
        );
        it(std:("should start the service successfully"), [=]() mutable
        {
            auto startPromise = TaskService::start(as<std::shared_ptr<IAgentRuntime>>(mockRuntime));
            expect(startPromise)->toBeInstanceOf(Promise);
            auto service = std::async([=]() { startPromise; });
            expect(service)->toBeDefined();
            expect(service)->toBeInstanceOf(TaskService);
            expect((as<any>(service))["runtime"])->toBe(mockRuntime);
        }
        );
        it(std:("should retrieve pending tasks correctly"), [=]() mutable
        {
            auto checkTasksMethod = (as<any>(taskService))["checkTasks"]["bind"](taskService);
            std::async([=]() { checkTasksMethod(); });
            expect(mockRuntime->getTasks)->toHaveBeenCalledWith(object{
                object::pair{std:("tags"), array<string>{ std:("queue") }}
            });
        }
        );
        it(std:("should process tasks that are ready"), [=]() mutable
        {
            auto pastTask = object{
                object::pair{std:("id"), std:("past-task")}, 
                object::pair{std:("name"), std:("Past scheduled task")}, 
                object::pair{std:("description"), std:("This task was scheduled in the past")}, 
                object::pair{std:("status"), std:("PENDING")}, 
                object::pair{std:("createdAt"), ((std::make_shared<Date>(Date->now() - 10000)))->toISOString()}, 
                object::pair{std:("scheduledFor"), ((std::make_shared<Date>(Date->now() - 5000)))->toISOString()}, 
                object::pair{std:("tags"), array<string>{ std:("queue") }}
            };
            mockRuntime->getTasks = mock()->mockResolvedValue(array<object>{ pastTask });
            auto executeTaskMethod = (as<any>(taskService))["executeTask"]["bind"](taskService);
            shared mockWorkerExecute = mock()->mockResolvedValue(undefined);
            mockRuntime->getTaskWorker = as<any>(mock()->mockImplementation([=](auto taskName) mutable
            {
                if (taskName == std:("Past scheduled task")) {
                    return object{
                        object::pair{std:("name"), taskName}, 
                        object::pair{std:("execute"), mockWorkerExecute}, 
                        object::pair{std:("validate"), mock()->mockResolvedValue(true)}
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
        it(std:("should handle errors during task processing"), [=]() mutable
        {
            auto testTask = object{
                object::pair{std:("id"), std:("error-task")}, 
                object::pair{std:("name"), std:("Error task")}, 
                object::pair{std:("description"), std:("This task will cause an error")}, 
                object::pair{std:("status"), std:("PENDING")}, 
                object::pair{std:("tags"), array<string>{ std:("queue") }}
            };
            shared mockErrorExecute = mock()->mockRejectedValue(std::make_shared<Error>(std:("Worker execution error")));
            mockRuntime->getTaskWorker = as<any>(mock()->mockImplementation([=](auto taskName) mutable
            {
                if (taskName == std:("Error task")) {
                    return object{
                        object::pair{std:("name"), taskName}, 
                        object::pair{std:("execute"), mockErrorExecute}, 
                        object::pair{std:("validate"), mock()->mockResolvedValue(true)}
                    };
                }
                return undefined;
            }
            ));
            spyOn(logger, std:("error"))->mockImplementation([=]() mutable
            {
            }
            );
            auto executeTaskMethod = (as<any>(taskService))["executeTask"]["bind"](taskService);
            std::async([=]() { executeTaskMethod(testTask); });
            expect(mockRuntime->getTaskWorker)->toHaveBeenCalledWith(testTask["name"]);
            expect(mockErrorExecute)->toHaveBeenCalled();
            expect(logger->error)->toHaveBeenCalledWith(std:("[Bootstrap] Error executing task ") + testTask["id"] + std:(":"), expect->any(Error));
        }
        );
    }
    );
    describe(std:("Service Registry"), [=]() mutable
    {
        shared<MockRuntime> mockRuntime;
        beforeEach([=]() mutable
        {
            mock->restore();
            spyOn(logger, std:("warn"))->mockImplementation([=]() mutable
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
        it(std:("should register all services correctly"), [=]() mutable
        {
            auto services = getPluginServices();
            expect(services)->toBeDefined();
            expect(services->get_length())->toBeGreaterThan(0);
            services->forEach([=](auto serviceDefinitionOrClass) mutable
            {
                if (type_of(serviceDefinitionOrClass) == std:("function")) {
                    auto serviceClass = as<any>(serviceDefinitionOrClass);
                    expect(serviceClass)->toHaveProperty(std:("serviceType"));
                    expect(type_of(serviceClass["serviceType"]))->toBe(std:("string"));
                    expect(serviceClass)->toHaveProperty(std:("name"));
                    expect(type_of(serviceClass["name"]))->toBe(std:("string"));
                    expect(serviceClass)->toHaveProperty(std:("start"));
                    expect(type_of(serviceClass["start"]))->toBe(std:("function"));
                } else {
                    auto serviceDesc = as<std::shared_ptr<PluginService>>(serviceDefinitionOrClass);
                    expect(serviceDesc)->toHaveProperty(std:("type"));
                    expect(type_of(serviceDesc->type))->toBe(std:("string"));
                    expect(serviceDesc)->toHaveProperty(std:("name"));
                    expect(type_of(serviceDesc->name))->toBe(std:("string"));
                    expect(serviceDesc)->toHaveProperty(std:("init"));
                    expect(type_of(serviceDesc->init))->toBe(std:("function"));
                }
            }
            );
        }
        );
        it(std:("should initialize file service if available"), [=]() mutable
        {
            auto services = getPluginServices();
            auto fileServiceDefinition = services->find([=](auto s) mutable
            {
                if (type_of(s) == std:("function")) return (as<any>(s))["serviceType"] == std:("file");
                return (as<std::shared_ptr<PluginService>>(s))->type == std:("file");
            }
            );
            if (fileServiceDefinition) {
                auto serviceInstance = (type_of(fileServiceDefinition) == std:("function")) ? std::async([=]() { (as<any>(fileServiceDefinition))["start"](as<std::shared_ptr<IAgentRuntime>>(mockRuntime)); }) : std::async([=]() { (as<std::shared_ptr<PluginService>>(fileServiceDefinition))->init(as<std::shared_ptr<IAgentRuntime>>(mockRuntime)); });
                expect(serviceInstance)->toBeDefined();
                expect(serviceInstance)->toHaveProperty(std:("uploadFile"));
                expect(serviceInstance)->toHaveProperty(std:("getFile"));
                expect(serviceInstance)->toHaveProperty(std:("listFiles"));
                expect(serviceInstance)->toHaveProperty(std:("deleteFile"));
                expect(type_of(serviceInstance["uploadFile"]))->toBe(std:("function"));
                expect(type_of(serviceInstance["getFile"]))->toBe(std:("function"));
                expect(type_of(serviceInstance["listFiles"]))->toBe(std:("function"));
                expect(type_of(serviceInstance["deleteFile"]))->toBe(std:("function"));
            }
        }
        );
        it(std:("should initialize PDF service if available"), [=]() mutable
        {
            auto services = getPluginServices();
            auto pdfServiceDefinition = services->find([=](auto s) mutable
            {
                if (type_of(s) == std:("function")) return (as<any>(s))["serviceType"] == ServiceType->PDF;
                return (as<std::shared_ptr<PluginService>>(s))->type == ServiceType->PDF;
            }
            );
            if (pdfServiceDefinition) {
                auto serviceInstance = (type_of(pdfServiceDefinition) == std:("function")) ? std::async([=]() { (as<any>(pdfServiceDefinition))["start"](as<std::shared_ptr<IAgentRuntime>>(mockRuntime)); }) : std::async([=]() { (as<std::shared_ptr<PluginService>>(pdfServiceDefinition))->init(as<std::shared_ptr<IAgentRuntime>>(mockRuntime)); });
                expect(serviceInstance)->toBeDefined();
                expect(serviceInstance)->toHaveProperty(std:("extractText"));
                expect(type_of(serviceInstance["extractText"]))->toBe(std:("function"));
            }
        }
        );
        it(std:("should initialize image service if available"), [=]() mutable
        {
            auto services = getPluginServices();
            auto imageServiceDefinition = services->find([=](auto s) mutable
            {
                if (type_of(s) == std:("function")) return (as<any>(s))["serviceType"] == std:("image");
                return (as<std::shared_ptr<PluginService>>(s))->type == std:("image");
            }
            );
            if (imageServiceDefinition) {
                auto serviceInstance = (type_of(imageServiceDefinition) == std:("function")) ? std::async([=]() { (as<any>(imageServiceDefinition))["start"](as<std::shared_ptr<IAgentRuntime>>(mockRuntime)); }) : std::async([=]() { (as<std::shared_ptr<PluginService>>(imageServiceDefinition))->init(as<std::shared_ptr<IAgentRuntime>>(mockRuntime)); });
                expect(serviceInstance)->toBeDefined();
                expect(serviceInstance)->toHaveProperty(std:("describeImage"));
                expect(type_of(serviceInstance["describeImage"]))->toBe(std:("function"));
            }
        }
        );
        it(std:("should initialize browser service if available"), [=]() mutable
        {
            auto services = getPluginServices();
            auto browserServiceDefinition = services->find([=](auto s) mutable
            {
                if (type_of(s) == std:("function")) return (as<any>(s))["serviceType"] == ServiceType->BROWSER;
                return (as<std::shared_ptr<PluginService>>(s))->type == ServiceType->BROWSER;
            }
            );
            if (browserServiceDefinition) {
                auto serviceInstance = (type_of(browserServiceDefinition) == std:("function")) ? std::async([=]() { (as<any>(browserServiceDefinition))["start"](as<std::shared_ptr<IAgentRuntime>>(mockRuntime)); }) : std::async([=]() { (as<std::shared_ptr<PluginService>>(browserServiceDefinition))->init(as<std::shared_ptr<IAgentRuntime>>(mockRuntime)); });
                expect(serviceInstance)->toBeDefined();
                expect(serviceInstance)->toHaveProperty(std:("browse"));
                expect(type_of(serviceInstance["browse"]))->toBe(std:("function"));
            }
        }
        );
        it(std:("should handle service initialization errors gracefully"), [=]() mutable
        {
            auto services = getPluginServices();
            auto fileServiceDefinition = services->find([=](auto s) mutable
            {
                if (type_of(s) == std:("function")) return (as<any>(s))["serviceType"] == std:("file");
                return (as<std::shared_ptr<PluginService>>(s))->type == std:("file");
            }
            );
            if (fileServiceDefinition) {
                mockRuntime->getService = mock()->mockImplementation([=]() mutable
                {
                    throw any(std::make_shared<Error>(std:("Service initialization failed")));
                }
                );
                auto serviceInstance = (type_of(fileServiceDefinition) == std:("function")) ? std::async([=]() { (as<any>(fileServiceDefinition))["start"](as<std::shared_ptr<IAgentRuntime>>(mockRuntime)); }) : std::async([=]() { (as<std::shared_ptr<PluginService>>(fileServiceDefinition))->init(as<std::shared_ptr<IAgentRuntime>>(mockRuntime)); });
                expect(serviceInstance)->toBeDefined();
                expect(logger->warn)->toHaveBeenCalled();
                expect(serviceInstance)->toHaveProperty(std:("uploadFile"));
                expect(serviceInstance)->toHaveProperty(std:("getFile"));
                expect(serviceInstance)->toHaveProperty(std:("listFiles"));
                expect(serviceInstance)->toHaveProperty(std:("deleteFile"));
            }
        }
        );
    }
    );
}

MAIN
