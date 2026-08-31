#include "village_event_bus.hpp"

#include <iostream>
#include <string>

namespace {
int checks = 0;
int failures = 0;

void check(bool condition, const std::string& name) {
    ++checks;
    if (!condition) {
        ++failures;
        std::cerr << "FAIL: " << name << '\n';
    }
}
}

int main() {
    const std::string directed = R"({
      "tic": 42,
      "event_type": "stimulus.directed",
      "source": "authenticity.operator",
      "target": "echo",
      "payload": "{\"message\":\"Respond from Echo's context\",\"correlation_id\":\"corr-42\"}"
    })";
    const auto event = elizaos::parseVillageEvent(directed);
    check(event.tic == 42, "tic parsed");
    check(event.typeStr == "stimulus.directed", "type parsed");
    check(event.source == "authenticity.operator", "source parsed");
    check(event.target == "echo", "target parsed");
    check(event.payload ==
              "{\"message\":\"Respond from Echo's context\",\"correlation_id\":\"corr-42\"}",
          "JSON-string payload unwrapped exactly once");

    const auto heartbeat = elizaos::parseVillageEvent(
        R"({"tic":43,"event_type":"heartbeat","source":"event_bus","target":null,"payload":{"alive":true}})");
    check(heartbeat.target.empty(), "nullable target becomes empty");
    check(heartbeat.payload == "{\"alive\":true}", "object payload remains canonical JSON");

    std::cout << "village_event_bus_test checks=" << checks
              << " failures=" << failures << '\n';
    return failures == 0 ? 0 : 1;
}
