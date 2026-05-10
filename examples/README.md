# ElizaOS C++ — Examples & Standalone Demos

This directory contains demo programs and standalone integration tests for the
ElizaOS C++ library. These files are **not** part of the main `cpp/tests/` suite;
they are self-contained programs for manual exploration and integration testing.

## Building the Main Demo

```bash
# Build with examples enabled
mkdir -p build && cd build
cmake -DBUILD_EXAMPLES=ON ..
cmake --build .

# Run the main demo
./examples/elizaos_demo
```

## Demo Programs

| File | What it demonstrates |
|---|---|
| `demo.cpp` | Original ElizaOS C++ capability demo |
| `eliza_main.cpp` | Minimal agent boot-up sequence |
| `comprehensive_demo.cpp` | End-to-end multi-module walkthrough |
| `agentcomms_demo.cpp` | agentcomms messaging channels |
| `agentshell_demo.cpp` | agentshell subprocess execution |
| `shell_demo.cpp` | Low-level shell interface |
| `simple_agentcomms_demo.cpp` | Minimal agentcomms example |
| `knowledge_demo.cpp` | Knowledge base insert/query |
| `knowledge_quick_demo.cpp` | Quick knowledge lookup |
| `characters_demo.cpp` | Character/personality loading |
| `hats_demo.cpp` | Hats role system demo |
| `registry_demo.cpp` | Plugin registry demo |
| `spartan_demo.cpp` | Spartan agent demo |
| `the_org_demo.cpp` | The Org multi-agent demo |
| `easycompletion_demo.cpp` | EasyCompletion plugin demo |
| `awesome_eliza_demo.cpp` | Curated awesome-eliza demo |
| `elizas_list_demo.cpp` | ElizaOS ecosystem listing |
| `elizas_world_demo.cpp` | Elizas World environment demo |
| `eliza_3d_hyperfy_starter_demo.cpp` | 3D/Hyperfy world starter demo |
| `demo_stage4.cpp` | Stage-4 capability demo |
| `demo_stage5.cpp` | Stage-5 capability demo |
| `demo_stage6.cpp` | Stage-6 full integration demo |
| `demo_status.cpp` | Implementation status dashboard |

## Standalone Tests

| File | What it tests |
|---|---|
| `agentshell_standalone_test.cpp` | agentshell (no GTest dependency) |
| `spartan_standalone_test.cpp` | Spartan agent (standalone) |
| `simple_shell_test.cpp` | Basic shell subprocess |
| `elizas_list_real_test.cpp` | ElizasList with real HTTP calls |
| `elizas_list_unit_test.cpp` | ElizasList unit tests |
| `elizas_world_standalone_test.cpp` | ElizasWorld standalone |
| `test_agentcomms_cross_interference.cpp` | Cross-channel isolation test |

## What the Core Demo Shows

1. **Agent Configuration**: Creating an agent with identity and bio
2. **State Management**: Adding actors, goals, and recent messages
3. **Agent Loop**: Running a threaded event loop with custom step functions
4. **Pause/Resume**: Demonstrating manual control of the agent loop
5. **Thread Safety**: Safe execution and shutdown

For the formal test suite see [`cpp/tests/`](../cpp/tests/).
For the C++ library overview see [`cpp/README.md`](../cpp/README.md).