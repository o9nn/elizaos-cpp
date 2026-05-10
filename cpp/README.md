# ElizaOS C++ — Core Library

This directory contains the production C++ implementation of the ElizaOS agent framework.

## Directory Structure

```
cpp/
├── include/elizaos/    # Public headers (one per module)
├── src/                # Utility / shared sources
│   └── model/capi/     # C API bridge
├── packages/           # Module implementations
│   ├── core/           # Foundational agent subsystems
│   ├── infrastructure/ # I/O, logging, browser, shell
│   ├── applications/   # Conversation engine & knowledge
│   ├── plugins/        # Plugin system & specifications
│   ├── starters/       # Project starter templates
│   └── integration/    # External service integrations
├── external/           # Vendored third-party code
├── generated/          # Auto-generated tests & scaffolding
└── tests/              # Unit & integration test suite
```

---

## Module Map

### Core (`packages/core/`)

| Module | Header | Description |
|---|---|---|
| `agentloop` | `agentloop.hpp` | Event-driven agent execution loop with pause/resume/step |
| `agentmemory` | `agentmemory.hpp` | Persistent storage with embedding-based retrieval and ECAN attention |
| `agentaction` | `agentaction.hpp` | Action dispatch and handler registration |
| `agentagenda` | `agentagenda.hpp` | Task scheduling, workflow sequencing, dependency resolution |
| `core` | `core.hpp` | AI decision engine, state composition, pattern recognition |

### Infrastructure (`packages/infrastructure/`)

| Module | Header | Description |
|---|---|---|
| `agentcomms` | `agentcomms.hpp` | Inter-agent messaging, Discord/Twitter/admin panel interfaces |
| `agentlogger` | `agentlogger.hpp` | Structured logging, introspection, telemetry |
| `agentbrowser` | `agentbrowser.hpp` | Web automation and scraping (2,270 lines) |
| `agentshell` | `agentshell.hpp` | Shell command execution and subprocess management |

### Applications (`packages/applications/`)

| Module | Header | Description |
|---|---|---|
| `eliza` | `eliza.hpp` | Full conversation engine (987 lines) |
| `characters` | `characters.hpp` | Personality / character system (2,092 lines) |
| `knowledge` | `knowledge.hpp` | Knowledge base with semantic search |
| `goal_manager` | `goal_manager.hpp` | Goal tracking and priority management |

### Plugins (`packages/plugins/`)

| Module | Header | Description |
|---|---|---|
| `plugin_specification` | `plugin_specification.hpp` | Plugin manifest and API contract |
| `plugins_automation` | `plugins_automation.hpp` | Plugin auto-discovery and lifecycle |
| `eliza_plugin_starter` | `eliza_plugin_starter.hpp` | Plugin project scaffold |
| `dgen_kobold_provider` | — | KoboldCPP inference provider |

### Starters (`packages/starters/`)

| Module | Header | Description |
|---|---|---|
| `eliza_starter` | `eliza_starter.hpp` | Minimal agent starter template |
| `eliza_nextjs_starter` | `eliza_nextjs_starter.hpp` | Next.js-integrated starter |
| `eliza_3d_hyperfy_starter` | `eliza_3d_hyperfy_starter.hpp` | 3D/Hyperfy world starter |

### Integration (`packages/integration/`)

| Module | Header | Description |
|---|---|---|
| `otaku` | `otaku.hpp` | Anime/media knowledge integration |
| `otc_agent` | `otc_agent.hpp` | OTC trading agent |
| `mcp_gateway` | `mcp_gateway.hpp` | MCP protocol gateway |
| `auto_fun` | `auto_fun.hpp` | auto.fun marketplace integration |
| `autonomous_starter` | `autonomous_starter.hpp` | Fully autonomous agent base |
| `sweagent` / `SWEagent` | `sweagent.hpp` | Software engineering agent |

---

## Python Counterparts

Each core/infrastructure module has a Python reference implementation in the repository root:

| C++ Module | Python Source |
|---|---|
| `agentloop` | [`agentloop/`](../agentloop/) |
| `agentmemory` | [`agentmemory/`](../agentmemory/) |
| `agentcomms` | [`agentcomms/`](../agentcomms/) |
| `agentlogger` | [`agentlogger/`](../agentlogger/) |
| `agentaction` | [`agentaction/`](../agentaction/) |
| `agentagenda` | [`agentagenda/`](../agentagenda/) |
| `agentbrowser` | [`agentbrowser/`](../agentbrowser/) |
| `agentshell` | [`agentshell/`](../agentshell/) |

---

## Build

### Prerequisites

- CMake 3.20+
- C++17-compatible compiler (GCC 9+, Clang 10+, MSVC 2019+)
- Optional: `libcurl`, `libgumbo` (for agentbrowser HTTP/HTML features)

### Quick Start

```bash
# From repository root
mkdir -p build && cd build
cmake ..
make -j$(nproc)
```

### Run All Tests

```bash
cd build
ctest --verbose
```

### Run a Specific Module Test

```bash
cd build
make agentloop_test
./cpp/tests/agentloop_test
```

---

## Headers

All public headers live in `include/elizaos/`. Include them via:

```cpp
#include "elizaos/agentloop.hpp"
#include "elizaos/agentmemory.hpp"
// ...
```

Link against the corresponding CMake target, e.g. `elizaos-agentloop`.

---

## Transpiler-Generated Scaffolding

The [`excpp/`](../excpp/) directory contains auto-generated C++ scaffolding produced by
[`tools/transpilers/ts_to_cpp_transpiler.py`](../tools/transpilers/ts_to_cpp_transpiler.py)
from the TypeScript source modules. These files are approximate conversions and require
manual refinement before use in production — see [`docs/transpiler/`](../docs/transpiler/)
for full documentation.

---

## See Also

- [Root README](../README.md) — Project overview
- [CLAUDE.md](../CLAUDE.md) — AI coding guidelines and quick commands
- [docs/architecture/](../docs/architecture/) — Architecture guides and design documents
- [docs/transpiler/](../docs/transpiler/) — TypeScript-to-C++ transpiler documentation
- [docs/reports/](../docs/reports/) — Historical implementation reports and status
- [examples/](../examples/) — Demo and standalone test programs
