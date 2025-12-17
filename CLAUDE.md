# CLAUDE.md - ElizaOS C++ Project Guide

## Project Overview

ElizaOS C++ is a high-performance C++ implementation of the ElizaOS agent framework for building sophisticated autonomous agents with advanced cognitive capabilities, distributed cognition, and adaptive attention allocation.

**Status:** 90% Complete - Production Ready
**Language:** C++17
**Build System:** CMake (3.16+)

## Quick Commands

```bash
# Build the project
mkdir build && cd build && cmake .. && make -j$(nproc)

# Run tests
./cpp/tests/elizaos_tests
ctest --verbose

# Run transpiler (TypeScript to C++)
python3 ts_to_cpp_transpiler.py --input-dir ./otaku/src --output-dir ./excpp/otaku/src --verbose

# Run transpiler with parallel processing
python3 ts_to_cpp_transpiler.py --parallel --max-workers 8
```

## Project Structure

```
elizaos-cpp/
├── cpp/                    # Core C++ implementation
│   ├── include/elizaos/    # Header files
│   ├── src/                # Source files
│   └── tests/              # Unit tests (305+ tests)
├── excpp/                  # Transpiled C++ from TypeScript (experimental)
├── ts_to_cpp_transpiler.py # TypeScript to C++ transpiler (v3.0)
├── .github/
│   ├── workflows/          # CI/CD workflows
│   │   └── transpiler.yml  # Transpiler GitHub Action
│   └── agents/             # Agent definitions and prompts
└── [module directories]    # TypeScript source modules (eliza, otaku, etc.)
```

## Key Modules

### Core C++ Implementation (`cpp/`)
- **agentloop/**: Event-driven agent execution with pause/resume/step
- **agentmemory/**: Persistent storage with embedding-based retrieval
- **agentcomms/**: Inter-agent messaging and external interfaces
- **agentlogger/**: Comprehensive logging and introspection
- **core/**: AI decision-making engine and reasoning
- **eliza/**: Conversation engine (987 lines)
- **characters/**: Personality system (2,092 lines)
- **knowledge/**: Knowledge base with semantic search
- **agentbrowser/**: Web automation (2,270 lines)

### Transpiler System
- **ts_to_cpp_transpiler.py**: Experimental TypeScript-to-C++ transpiler
- **excpp/**: Output directory for transpiled code
- **.github/workflows/transpiler.yml**: Automated transpilation workflow

## Development Guidelines

### Code Standards
- Modern C++17 with RAII, smart pointers, STL
- Thread-safe operations for multi-threaded agent systems
- Comprehensive error handling with exceptions
- Headers in `include/elizaos/`, sources in `src/`

### Implementation Philosophy
- **Rigorous Implementation**: Complete functionality, not placeholders
- **Production Quality**: Robust, fully-functional implementations
- **Complete Features**: All methods and supporting infrastructure

### Avoid
- Mock/placeholder implementations
- Simplified solutions that reduce functionality
- TODO comments without implementation plans
- Partial implementations without documented next steps

## TypeScript to C++ Transpiler

### Overview
The transpiler (`ts_to_cpp_transpiler.py`) converts TypeScript to approximate C++ code:
- Generates `.hpp` headers and `.cpp` implementations
- Preserves directory structure
- Maps TS types to C++ equivalents (string→std::string, number→double, etc.)
- Wraps code in `elizaos` namespace

### Type Mappings
| TypeScript | C++ |
|------------|-----|
| `string` | `std::string` |
| `number` | `double` |
| `boolean` | `bool` |
| `any` | `std::any` |
| `T[]` | `std::vector<T>` |
| `Map<K,V>` | `std::unordered_map<K,V>` |
| `Promise<T>` | `std::future<T>` |
| `T \| null` | `std::optional<T>` |

### Known Limitations
- Function bodies need manual refinement
- Complex TypeScript patterns may not convert correctly
- Build system (CMakeLists.txt) not auto-generated
- External dependencies must be manually mapped

## GitHub Actions

### Transpiler Workflow (`.github/workflows/transpiler.yml`)
- Triggers on: push to main/develop, pull requests, manual dispatch
- Runs `ts_to_cpp_transpiler.py`
- Validates generated files for TypeScript code leakage
- Commits and pushes generated files (on push/dispatch)
- Uploads artifacts for 30 days

### Manual Dispatch Options
- `input_directory`: Source TypeScript directory (default: `.`)
- `output_directory`: Output C++ directory (default: `excpp`)
- `verbose`: Enable verbose output

## Testing

```bash
# All tests
ctest

# Specific categories
ctest -R "Memory"     # Memory system
ctest -R "Loop"       # Agent loop
ctest -R "Core"       # Core functionality
ctest -R "Eliza"      # Conversation engine
```

## Key Documentation Files

- `CPP_FUNCTIONAL_COMPLETENESS_REPORT.md` - 90% completion analysis
- `NEXT_STEPS_IMPLEMENTATION.md` - Roadmap to 100%
- `TRANSPILER_README.md` - Transpiler documentation
- `TECH_ARCHITECTURE.md` - System architecture with diagrams
- `.github/agents/TRANSPILER_TS_CPP.md` - Transpiler agent definition

## Common Tasks

### Adding a new module
1. Create header in `cpp/include/elizaos/`
2. Create source in `cpp/src/`
3. Add to `CMakeLists.txt`
4. Write tests in `cpp/tests/`

### Running the transpiler on a specific module
```bash
python3 ts_to_cpp_transpiler.py \
    --input-dir ./eliza/packages/core/src \
    --output-dir ./excpp/eliza/packages/core/src \
    --verbose
```

### Fixing transpiler output
1. Review generated `.hpp` files for TypeScript syntax leakage
2. Check for patterns: `=>`, `await`, `===`, string literals in types
3. Fix type declarations and add missing includes
4. Implement function bodies manually

## Contact & Resources

- **GitHub Issues**: https://github.com/anthropics/claude-code/issues
- **Original TypeScript ElizaOS**: https://github.com/elizaos/eliza
