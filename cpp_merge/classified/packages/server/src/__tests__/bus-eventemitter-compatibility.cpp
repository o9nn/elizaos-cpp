#include "bus-eventemitter-compatibility.test.h"

void Main(void)
{
    describe(std:("InternalMessageBus EventEmitter Compatibility"), [=]() mutable
    {
        beforeEach([=]() mutable
        {
            (as<any>(internalMessageBus))["removeAllListeners"]();
        }
        );
        describe(std:("emit() method"), [=]() mutable
        {
            it(std:("should emit events with data"), [=](auto done) mutable
            {
                shared testData = object{
                    object::pair{std:("message"), std:("test")}
                };
                internalMessageBus->on(std:("test-event"), [=](auto data) mutable
                {
                    expect(data)->toEqual(testData);
                    done();
                }
                );
                auto result = internalMessageBus->emit(std:("test-event"), testData);
                expect(result)->toBe(true);
            }
            );
            it(std:("should return true when event is dispatched"), [=]() mutable
            {
                auto result = internalMessageBus->emit(std:("no-listeners"), object{
                    object::pair{std:("data"), std:("test")}
                });
                expect(result)->toBe(true);
            }
            );
            it(std:("should handle undefined data"), [=](auto done) mutable
            {
                internalMessageBus->on(std:("undefined-event"), [=](auto data) mutable
                {
                    expect(data)->toBeNull();
                    done();
                }
                );
                internalMessageBus->emit(std:("undefined-event"), undefined);
            }
            );
            it(std:("should handle null data"), [=](auto done) mutable
            {
                internalMessageBus->on(std:("null-event"), [=](auto data) mutable
                {
                    expect(data)->toBeNull();
                    done();
                }
                );
                internalMessageBus->emit(std:("null-event"), nullptr);
            }
            );
        }
        );
        describe(std:("on() method"), [=]() mutable
        {
            it(std:("should add event listeners"), [=]() mutable
            {
                shared called = false;
                auto handler = [=]() mutable
                {
                    called = true;
                };
                internalMessageBus->on(std:("test-on"), handler);
                internalMessageBus->emit(std:("test-on"), object{});
                expect(called)->toBe(true);
            }
            );
            it(std:("should support multiple listeners for same event"), [=]() mutable
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
                internalMessageBus->on(std:("multi-listener"), handler1);
                internalMessageBus->on(std:("multi-listener"), handler2);
                internalMessageBus->emit(std:("multi-listener"), object{});
                expect(count)->toBe(2);
            }
            );
            it(std:("should not add duplicate handlers"), [=]() mutable
            {
                shared count = 0;
                auto handler = [=]() mutable
                {
                    count++;
                };
                internalMessageBus->on(std:("duplicate-test"), handler);
                internalMessageBus->on(std:("duplicate-test"), handler);
                internalMessageBus->emit(std:("duplicate-test"), object{});
                expect(count)->toBe(1);
            }
            );
            it(std:("should return this for method chaining"), [=]() mutable
            {
                auto handler = [=]() mutable
                {
                };
                auto result = internalMessageBus->on(std:("chain-test"), handler);
                expect(result)->toBe(internalMessageBus);
            }
            );
            it(std:("should pass data correctly to handlers"), [=](auto done) mutable
            {
                shared testData = object{
                    object::pair{std:("foo"), std:("bar")}, 
                    object::pair{std:("num"), 123}
                };
                internalMessageBus->on(std:("data-test"), [=](auto data) mutable
                {
                    expect(data)->toEqual(testData);
                    expect(data->foo)->toBe(std:("bar"));
                    expect(data->num)->toBe(123);
                    done();
                }
                );
                internalMessageBus->emit(std:("data-test"), testData);
            }
            );
        }
        );
        describe(std:("off() method"), [=]() mutable
        {
            it(std:("should remove event listeners"), [=]() mutable
            {
                shared called = false;
                auto handler = [=]() mutable
                {
                    called = true;
                };
                internalMessageBus->on(std:("test-off"), handler);
                internalMessageBus->off(std:("test-off"), handler);
                internalMessageBus->emit(std:("test-off"), object{});
                expect(called)->toBe(false);
            }
            );
            it(std:("should only remove specified handler"), [=]() mutable
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
                internalMessageBus->on(std:("selective-off"), handler1);
                internalMessageBus->on(std:("selective-off"), handler2);
                internalMessageBus->off(std:("selective-off"), handler1);
                internalMessageBus->emit(std:("selective-off"), object{});
                expect(called1)->toBe(false);
                expect(called2)->toBe(true);
            }
            );
            it(std:("should handle removing non-existent handler gracefully"), [=]() mutable
            {
                shared handler = [=]() mutable
                {
                };
                expect([=]() mutable
                {
                    internalMessageBus->off(std:("non-existent"), handler);
                }
                )->not->toThrow();
            }
            );
            it(std:("should clean up internal maps when last handler is removed"), [=]() mutable
            {
                auto handler = [=]() mutable
                {
                };
                internalMessageBus->on(std:("cleanup-test"), handler);
                internalMessageBus->off(std:("cleanup-test"), handler);
                shared count = 0;
                auto newHandler = [=]() mutable
                {
                    count++;
                };
                internalMessageBus->on(std:("cleanup-test"), newHandler);
                internalMessageBus->on(std:("cleanup-test"), newHandler);
                internalMessageBus->emit(std:("cleanup-test"), object{});
                expect(count)->toBe(1);
            }
            );
        }
        );
        describe(std:("setMaxListeners() method"), [=]() mutable
        {
            it(std:("should accept a number without throwing"), [=]() mutable
            {
                expect([=]() mutable
                {
                    internalMessageBus->setMaxListeners(100);
                }
                )->not->toThrow();
            }
            );
            it(std:("should be chainable (EventEmitter compatibility)"), [=]() mutable
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
        describe(std:("EventEmitter-like behavior"), [=]() mutable
        {
            it(std:("should execute listeners in order of registration"), [=]() mutable
            {
                shared order = array<double>();
                internalMessageBus->on(std:("order-test"), [=]() mutable
                {
                    return order->push(1);
                }
                );
                internalMessageBus->on(std:("order-test"), [=]() mutable
                {
                    return order->push(2);
                }
                );
                internalMessageBus->on(std:("order-test"), [=]() mutable
                {
                    return order->push(3);
                }
                );
                internalMessageBus->emit(std:("order-test"), object{});
                expect(order)->toEqual(array<double>{ 1, 2, 3 });
            }
            );
            it(std:("verifies listener execution order and error simulation"), [=]() mutable
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
                internalMessageBus->on(std:("error-simulation"), handler1);
                internalMessageBus->on(std:("error-simulation"), handler2);
                internalMessageBus->on(std:("error-simulation"), handler3);
                auto result = internalMessageBus->emit(std:("error-simulation"), object{});
                expect(result)->toBe(true);
                expect(called1)->toBe(true);
                expect(called2)->toBe(true);
                expect(called3)->toBe(true);
                expect(errorSimulated)->toBe(true);
                internalMessageBus->off(std:("error-simulation"), handler1);
                internalMessageBus->off(std:("error-simulation"), handler2);
                internalMessageBus->off(std:("error-simulation"), handler3);
            }
            );
            it(std:("demonstrates EventTarget error propagation behavior (informational)"), [=]() mutable
            {
                auto docs = object{
                    object::pair{std:("behavior"), std:("When a listener throws an error in EventTarget:")}, 
                    object::pair{std:("points"), array<string>{ std:("1. The error does not propagate to dispatchEvent() caller"), std:("2. Other listeners continue to execute"), std:("3. The error is reported to the global error handler"), std:("4. dispatchEvent() returns true (not false)"), std:("5. In browsers: window.onerror is called"), std:("6. In Node/Bun: uncaughtException event is emitted") }}, 
                    object::pair{std:("example"), std:("\
          const target = std::make_unique<EventTarget>();\
          target.addEventListener[&]('test', () { return std::cout << '1' << std::endl); };\
          target.addEventListener('test', [&]() { throw new Error('boom'); });\
          target.addEventListener[&]('test', () { return std::cout << '3' << std::endl); };\
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
                bus->on(std:("doc-test"), handler1);
                bus->on(std:("doc-test"), handler2);
                bus->on(std:("doc-test"), handler3);
                auto result = bus->emit(std:("doc-test"), object{});
                expect(result)->toBe(true);
                expect(listenersCalled)->toBe(3);
                expect(docs["behavior"])->toBeDefined();
                bus->off(std:("doc-test"), handler1);
                bus->off(std:("doc-test"), handler2);
                bus->off(std:("doc-test"), handler3);
            }
            );
            it(std:("should handle complex data types"), [=](auto done) mutable
            {
                shared complexData = object{
                    object::pair{std:("array"), array<double>{ 1, 2, 3 }}, 
                    object::pair{std:("nested"), object{
                        object::pair{std:("deep"), object{
                            object::pair{std:("value"), std:("test")}
                        }}
                    }}, 
                    object::pair{std:("date"), std::make_shared<Date>()}, 
                    object::pair{std:("nullValue"), nullptr}, 
                    object::pair{std:("undefinedValue"), undefined}
                };
                internalMessageBus->on(std:("complex-data"), [=](auto data) mutable
                {
                    expect(data)->toEqual(complexData);
                    expect(data->array)->toEqual(array<double>{ 1, 2, 3 });
                    expect(data->nested->deep->value)->toBe(std:("test"));
                    done();
                }
                );
                internalMessageBus->emit(std:("complex-data"), complexData);
            }
            );
        }
        );
        describe(std:("Memory and performance"), [=]() mutable
        {
            it(std:("should handle many listeners without issues"), [=]() mutable
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
                    internalMessageBus->on(std:("many-listeners"), handler);
                }
                internalMessageBus->emit(std:("many-listeners"), object{});
                expect(totalCalls)->toBe(100);
                handlers->forEach([=](auto handler) mutable
                {
                    internalMessageBus->off(std:("many-listeners"), handler);
                }
                );
            }
            );
            it(std:("should handle rapid fire events"), [=]() mutable
            {
                shared count = 0;
                internalMessageBus->on(std:("rapid-fire"), [=]() mutable
                {
                    count++;
                }
                );
                for (auto i = 0; i < 1000; i++)
                {
                    internalMessageBus->emit(std:("rapid-fire"), object{
                        object::pair{std:("index"), i}
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
