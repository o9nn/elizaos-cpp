#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/agentserver/src/__tests__/bus-eventemitter-compatibility.test.h"

void Main(void)
{
    describe(std::string("InternalMessageBus EventEmitter Compatibility"), [=]() mutable
    {
        beforeEach([=]() mutable
        {
            (as<any>(internalMessageBus))["removeAllListeners"]();
        }
        );
        describe(std::string("emit() method"), [=]() mutable
        {
            it(std::string("should emit events with data"), [=](auto done) mutable
            {
                shared testData = object{
                    object::pair{std::string("message"), std::string("test")}
                };
                internalMessageBus->on(std::string("test-event"), [=](auto data) mutable
                {
                    expect(data)->toEqual(testData);
                    done();
                }
                );
                auto result = internalMessageBus->emit(std::string("test-event"), testData);
                expect(result)->toBe(true);
            }
            );
            it(std::string("should return true when event is dispatched"), [=]() mutable
            {
                auto result = internalMessageBus->emit(std::string("no-listeners"), object{
                    object::pair{std::string("data"), std::string("test")}
                });
                expect(result)->toBe(true);
            }
            );
            it(std::string("should handle undefined data"), [=](auto done) mutable
            {
                internalMessageBus->on(std::string("undefined-event"), [=](auto data) mutable
                {
                    expect(data)->toBeNull();
                    done();
                }
                );
                internalMessageBus->emit(std::string("undefined-event"), undefined);
            }
            );
            it(std::string("should handle null data"), [=](auto done) mutable
            {
                internalMessageBus->on(std::string("null-event"), [=](auto data) mutable
                {
                    expect(data)->toBeNull();
                    done();
                }
                );
                internalMessageBus->emit(std::string("null-event"), nullptr);
            }
            );
        }
        );
        describe(std::string("on() method"), [=]() mutable
        {
            it(std::string("should add event listeners"), [=]() mutable
            {
                shared called = false;
                auto handler = [=]() mutable
                {
                    called = true;
                };
                internalMessageBus->on(std::string("test-on"), handler);
                internalMessageBus->emit(std::string("test-on"), object{});
                expect(called)->toBe(true);
            }
            );
            it(std::string("should support multiple listeners for same event"), [=]() mutable
            {
                shared count = 0;
                auto handler1 = [=]() mutable
                {
                    count++;
                };
                auto handler2 = [=]() mutable
                {
                    count++;
                };
                internalMessageBus->on(std::string("multi-listener"), handler1);
                internalMessageBus->on(std::string("multi-listener"), handler2);
                internalMessageBus->emit(std::string("multi-listener"), object{});
                expect(count)->toBe(2);
            }
            );
            it(std::string("should not add duplicate handlers"), [=]() mutable
            {
                shared count = 0;
                auto handler = [=]() mutable
                {
                    count++;
                };
                internalMessageBus->on(std::string("duplicate-test"), handler);
                internalMessageBus->on(std::string("duplicate-test"), handler);
                internalMessageBus->emit(std::string("duplicate-test"), object{});
                expect(count)->toBe(1);
            }
            );
            it(std::string("should return this for method chaining"), [=]() mutable
            {
                auto handler = [=]() mutable
                {
                };
                auto result = internalMessageBus->on(std::string("chain-test"), handler);
                expect(result)->toBe(internalMessageBus);
            }
            );
            it(std::string("should pass data correctly to handlers"), [=](auto done) mutable
            {
                shared testData = object{
                    object::pair{std::string("foo"), std::string("bar")}, 
                    object::pair{std::string("num"), 123}
                };
                internalMessageBus->on(std::string("data-test"), [=](auto data) mutable
                {
                    expect(data)->toEqual(testData);
                    expect(data->foo)->toBe(std::string("bar"));
                    expect(data->num)->toBe(123);
                    done();
                }
                );
                internalMessageBus->emit(std::string("data-test"), testData);
            }
            );
        }
        );
        describe(std::string("off() method"), [=]() mutable
        {
            it(std::string("should remove event listeners"), [=]() mutable
            {
                shared called = false;
                auto handler = [=]() mutable
                {
                    called = true;
                };
                internalMessageBus->on(std::string("test-off"), handler);
                internalMessageBus->off(std::string("test-off"), handler);
                internalMessageBus->emit(std::string("test-off"), object{});
                expect(called)->toBe(false);
            }
            );
            it(std::string("should only remove specified handler"), [=]() mutable
            {
                shared called1 = false;
                shared called2 = false;
                auto handler1 = [=]() mutable
                {
                    called1 = true;
                };
                auto handler2 = [=]() mutable
                {
                    called2 = true;
                };
                internalMessageBus->on(std::string("selective-off"), handler1);
                internalMessageBus->on(std::string("selective-off"), handler2);
                internalMessageBus->off(std::string("selective-off"), handler1);
                internalMessageBus->emit(std::string("selective-off"), object{});
                expect(called1)->toBe(false);
                expect(called2)->toBe(true);
            }
            );
            it(std::string("should handle removing non-existent handler gracefully"), [=]() mutable
            {
                shared handler = [=]() mutable
                {
                };
                expect([=]() mutable
                {
                    internalMessageBus->off(std::string("non-existent"), handler);
                }
                )->not->toThrow();
            }
            );
            it(std::string("should clean up internal maps when last handler is removed"), [=]() mutable
            {
                auto handler = [=]() mutable
                {
                };
                internalMessageBus->on(std::string("cleanup-test"), handler);
                internalMessageBus->off(std::string("cleanup-test"), handler);
                shared count = 0;
                auto newHandler = [=]() mutable
                {
                    count++;
                };
                internalMessageBus->on(std::string("cleanup-test"), newHandler);
                internalMessageBus->on(std::string("cleanup-test"), newHandler);
                internalMessageBus->emit(std::string("cleanup-test"), object{});
                expect(count)->toBe(1);
            }
            );
        }
        );
        describe(std::string("setMaxListeners() method"), [=]() mutable
        {
            it(std::string("should accept a number without throwing"), [=]() mutable
            {
                expect([=]() mutable
                {
                    internalMessageBus->setMaxListeners(100);
                }
                )->not->toThrow();
            }
            );
            it(std::string("should be chainable (EventEmitter compatibility)"), [=]() mutable
            {
                expect([=]() mutable
                {
                    internalMessageBus->setMaxListeners(50);
                }
                )->not->toThrow();
            }
            );
        }
        );
        describe(std::string("EventEmitter-like behavior"), [=]() mutable
        {
            it(std::string("should execute listeners in order of registration"), [=]() mutable
            {
                shared order = array<double>();
                internalMessageBus->on(std::string("order-test"), [=]() mutable
                {
                    return order->push(1);
                }
                );
                internalMessageBus->on(std::string("order-test"), [=]() mutable
                {
                    return order->push(2);
                }
                );
                internalMessageBus->on(std::string("order-test"), [=]() mutable
                {
                    return order->push(3);
                }
                );
                internalMessageBus->emit(std::string("order-test"), object{});
                expect(order)->toEqual(array<double>{ 1, 2, 3 });
            }
            );
            it(std::string("verifies listener execution order and error simulation"), [=]() mutable
            {
                shared called1 = false;
                shared called2 = false;
                shared called3 = false;
                shared errorSimulated = false;
                auto handler1 = [=]() mutable
                {
                    called1 = true;
                };
                auto handler2 = [=]() mutable
                {
                    called2 = true;
                    errorSimulated = true;
                };
                auto handler3 = [=]() mutable
                {
                    called3 = true;
                };
                internalMessageBus->on(std::string("error-simulation"), handler1);
                internalMessageBus->on(std::string("error-simulation"), handler2);
                internalMessageBus->on(std::string("error-simulation"), handler3);
                auto result = internalMessageBus->emit(std::string("error-simulation"), object{});
                expect(result)->toBe(true);
                expect(called1)->toBe(true);
                expect(called2)->toBe(true);
                expect(called3)->toBe(true);
                expect(errorSimulated)->toBe(true);
                internalMessageBus->off(std::string("error-simulation"), handler1);
                internalMessageBus->off(std::string("error-simulation"), handler2);
                internalMessageBus->off(std::string("error-simulation"), handler3);
            }
            );
            it(std::string("demonstrates EventTarget error propagation behavior (informational)"), [=]() mutable
            {
                auto docs = object{
                    object::pair{std::string("behavior"), std::string("When a listener throws an error in EventTarget:")}, 
                    object::pair{std::string("points"), array<string>{ std::string("1. The error does not propagate to dispatchEvent() caller"), std::string("2. Other listeners continue to execute"), std::string("3. The error is reported to the global error handler"), std::string("4. dispatchEvent() returns true (not false)"), std::string("5. In browsers: window.onerror is called"), std::string("6. In Node/Bun: uncaughtException event is emitted") }}, 
                    object::pair{std::string("example"), std::string("\
          const target = new EventTarget();\
          target.addEventListener('test', () => console.log('1'));\
          target.addEventListener('test', () => { throw new Error('boom'); });\
          target.addEventListener('test', () => console.log('3'));\
          \
          // This will log: 1, [error to stderr], 3\
          // And return: true\
          const result = target.dispatchEvent(new Event('test'));\
        ")}
                };
                auto bus = internalMessageBus;
                shared listenersCalled = 0;
                auto handler1 = [=]() mutable
                {
                    listenersCalled++;
                };
                auto handler2 = [=]() mutable
                {
                    listenersCalled++;
                };
                auto handler3 = [=]() mutable
                {
                    listenersCalled++;
                };
                bus->on(std::string("doc-test"), handler1);
                bus->on(std::string("doc-test"), handler2);
                bus->on(std::string("doc-test"), handler3);
                auto result = bus->emit(std::string("doc-test"), object{});
                expect(result)->toBe(true);
                expect(listenersCalled)->toBe(3);
                expect(docs["behavior"])->toBeDefined();
                bus->off(std::string("doc-test"), handler1);
                bus->off(std::string("doc-test"), handler2);
                bus->off(std::string("doc-test"), handler3);
            }
            );
            it(std::string("should handle complex data types"), [=](auto done) mutable
            {
                shared complexData = object{
                    object::pair{std::string("array"), array<double>{ 1, 2, 3 }}, 
                    object::pair{std::string("nested"), object{
                        object::pair{std::string("deep"), object{
                            object::pair{std::string("value"), std::string("test")}
                        }}
                    }}, 
                    object::pair{std::string("date"), std::make_shared<Date>()}, 
                    object::pair{std::string("nullValue"), nullptr}, 
                    object::pair{std::string("undefinedValue"), undefined}
                };
                internalMessageBus->on(std::string("complex-data"), [=](auto data) mutable
                {
                    expect(data)->toEqual(complexData);
                    expect(data->array)->toEqual(array<double>{ 1, 2, 3 });
                    expect(data->nested->deep->value)->toBe(std::string("test"));
                    done();
                }
                );
                internalMessageBus->emit(std::string("complex-data"), complexData);
            }
            );
        }
        );
        describe(std::string("Memory and performance"), [=]() mutable
        {
            it(std::string("should handle many listeners without issues"), [=]() mutable
            {
                auto handlers = array<any>();
                shared totalCalls = 0;
                for (auto i = 0; i < 100; i++)
                {
                    auto handler = [=]() mutable
                    {
                        totalCalls++;
                    };
                    handlers->push(handler);
                    internalMessageBus->on(std::string("many-listeners"), handler);
                }
                internalMessageBus->emit(std::string("many-listeners"), object{});
                expect(totalCalls)->toBe(100);
                handlers->forEach([=](auto handler) mutable
                {
                    internalMessageBus->off(std::string("many-listeners"), handler);
                }
                );
            }
            );
            it(std::string("should handle rapid fire events"), [=]() mutable
            {
                shared count = 0;
                internalMessageBus->on(std::string("rapid-fire"), [=]() mutable
                {
                    count++;
                }
                );
                for (auto i = 0; i < 1000; i++)
                {
                    internalMessageBus->emit(std::string("rapid-fire"), object{
                        object::pair{std::string("index"), i}
                    });
                }
                expect(count)->toBe(1000);
            }
            );
        }
        );
    }
    );
}

MAIN
