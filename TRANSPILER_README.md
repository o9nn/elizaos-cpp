# TypeScript to C++ Experimental Transpiler

## Overview

This document describes the experimental TypeScript-to-C++ transpiler tool designed to rapidly convert ElizaOS TypeScript implementations into approximate C++ equivalents. This tool is part of an evaluation to compare direct TS→C++ conversion against a 2-step TS→Z++→C++ approach.

## Purpose

The transpiler serves as an **experimental method** to:

1. **Rapidly generate** approximate C++ code from TypeScript sources
2. **Maintain relative paths** of original file/folder structures for easy troubleshooting
3. **Evaluate effectiveness** of blind transpiling techniques vs. complete rewrites
4. **Compare approaches** between direct conversion and multi-stage conversion

⚠️ **Important**: This tool generates approximate code that will require manual refinement. It is **not** intended to produce production-ready code automatically.

## Features

### Type Conversion
- **Basic types**: `string` → `std::string`, `number` → `double`, `boolean` → `bool`
- **Collections**: `Array<T>` → `std::vector<T>`, `Map<K,V>` → `std::unordered_map<K,V>`
- **Optional types**: `T?` → `std::optional<T>`
- **Union types**: `A | B` → `std::variant<A, B>`
- **Promises**: `Promise<T>` → `std::future<T>`
- **Function types**: `Function` → `std::function`

### Code Structure Conversion
- **Interfaces** → C++ structs with appropriate member types
- **Classes** → C++ classes with public/private sections
- **Type aliases** → C++ `using` declarations
- **Async functions** → Functions returning `std::future<T>`
- **Imports** → C++ `#include` directives
- **Export statements** → Removed (handled by namespace)

### File Organization
- **Maintains directory structure**: `src/path/file.ts` → `excpp/src/path/file.{hpp,cpp}`
- **Generates header files**: `.hpp` files with declarations
- **Generates implementation files**: `.cpp` files with placeholder implementations
- **Namespace wrapping**: All code wrapped in `elizaos` namespace

## Installation & Usage

### Prerequisites
- Python 3.6 or higher
- Read access to TypeScript source files

### Basic Usage

```bash
# Make script executable
chmod +x ts_to_cpp_transpiler.py

# Convert all TypeScript files in current repo
python3 ts_to_cpp_transpiler.py

# Convert specific directory
python3 ts_to_cpp_transpiler.py --input-dir ./otaku/src --output-dir ./excpp/otaku

# Enable verbose output
python3 ts_to_cpp_transpiler.py --verbose
```

### Command-Line Options

| Option | Description | Default |
|--------|-------------|---------|
| `--input-dir DIR` | Input directory containing TypeScript files | `.` (current directory) |
| `--output-dir DIR` | Output directory for C++ files | `./excpp` |
| `--verbose` | Enable verbose output | `False` |

## Output Structure

The transpiler generates:

1. **Header files (`.hpp`)**: 
   - Include guards with `#pragma once`
   - Standard library includes
   - Struct/class declarations
   - Function signatures
   - Type aliases

2. **Implementation files (`.cpp`)**:
   - Include corresponding header
   - Namespace wrapper
   - Placeholder implementation comments

## Conversion Results

### Repository-Wide Statistics

**Total Conversion (Current Run):**
- **Files Processed**: 3,317 TypeScript files
- **Files Skipped**: 625 (tests, configs, specs)
- **Errors**: 0
- **Output Files**: 6,540 (3,270 `.hpp` + 3,270 `.cpp`)

### Directory Coverage

The transpiler successfully converted all major modules:
- ✅ `otaku/` - DeFi agent implementation
- ✅ `eliza/` - Core Eliza engine
- ✅ `SWEagent/` - Software engineering agent
- ✅ `classified/` - Classified module
- ✅ `plugin-specification/` - Plugin specs
- ✅ `auto.fun/` - Auto.fun platform
- ✅ `elizaos.github.io/` - Website and UI components
- ✅ `mobile/` - Mobile implementations
- ✅ And 20+ other modules...

### File Size Distribution

Example output for `otaku` module:
```
otaku/src/
├── index.ts → index.{hpp,cpp}
├── character.ts → character.{hpp,cpp}
├── build-utils.ts → build-utils.{hpp,cpp}
├── utils/entity.ts → utils/entity.{hpp,cpp}
└── plugins/
    ├── plugin-cdp/...
    ├── plugin-etherscan/...
    └── ...
```

## Known Limitations

### What the Transpiler Does NOT Handle

1. **Function implementations**: Only generates stubs with TODO comments
2. **Complex logic**: Arrow functions, closures, complex expressions
3. **Runtime behavior**: Event loops, async/await semantics
4. **TypeScript-specific features**: Decorators, mixins, conditional types
5. **Build configuration**: CMakeLists.txt, package management
6. **Dependencies**: External libraries must be manually mapped

### Files Automatically Skipped

- Test files (`*.test.ts`, `*.spec.ts`)
- Configuration files (`tsconfig.json`, `vite.config.ts`, etc.)
- Type definition files (`*.d.ts`)
- Build/tooling scripts

## Manual Refinement Required

After transpilation, the following must be manually implemented:

### 1. Function Bodies
All function implementations are stubbed:
```cpp
// TODO: Implement function bodies
// Original TypeScript code has been analyzed
// Manual implementation required for complete functionality
```

### 2. Error Handling
Convert TypeScript try/catch to C++ exception handling:
```cpp
try {
    // Implementation
} catch (const std::exception& e) {
    // Error handling
}
```

### 3. Async Operations
Replace Promise-based code with appropriate C++ async patterns:
- `std::future` / `std::promise`
- Callback patterns
- Event loops (e.g., Boost.Asio)

### 4. Memory Management
Add proper RAII and smart pointer usage:
```cpp
auto resource = std::make_shared<Resource>();
std::unique_ptr<Handler> handler = std::make_unique<Handler>();
```

### 5. Build System Integration
Create or update `CMakeLists.txt` to build the new code:
```cmake
add_library(module_name
    excpp/module/src/file1.cpp
    excpp/module/src/file2.cpp
)
target_include_directories(module_name PUBLIC excpp/module/include)
```

## Comparison Framework

### Evaluation Criteria

To evaluate the effectiveness of direct TS→C++ vs. TS→Z++→C++ approaches, consider:

1. **Initial Conversion Time**: How long to generate C++ code?
2. **Manual Fix Rate**: What % of code needs manual refinement?
3. **Compilation Success**: How many files compile without errors?
4. **Functional Correctness**: How much logic needs reimplementation?
5. **Code Quality**: How maintainable is the generated code?

### Suggested Evaluation Process

```bash
# 1. Run transpiler
python3 ts_to_cpp_transpiler.py --verbose

# 2. Count generated files
find excpp -name "*.hpp" | wc -l
find excpp -name "*.cpp" | wc -l

# 3. Attempt compilation (expect errors)
cd excpp/module
mkdir build && cd build
cmake ..
make 2>&1 | tee compile_errors.log

# 4. Analyze error types
grep -E "error:" compile_errors.log | sort | uniq -c

# 5. Measure fix effort
# Track time spent fixing each file/module
# Document patterns that require most work
```

## Example Conversions

### Example 1: Interface to Struct

**TypeScript Input:**
```typescript
interface EntityWalletResult {
  success: true;
  walletAddress: string;
  metadata?: Record<string, any>;
}
```

**C++ Output:**
```cpp
struct EntityWalletResult {
    std::string walletAddress;
    std::optional<std::unordered_map<std::string, std::any>> metadata;
};
```

### Example 2: Async Function

**TypeScript Input:**
```typescript
async function getEntityWallet(
  runtime: IAgentRuntime,
  message: Memory,
  actionName: string,
  callback?: HandlerCallback,
): Promise<EntityWalletResponse>
```

**C++ Output:**
```cpp
std::future<EntityWalletResponse> getEntityWallet(
    IAgentRuntime runtime,
    Memory message,
    const std::string& actionName,
    std::optional<HandlerCallback> callback
);
```

### Example 3: Type Union

**TypeScript Input:**
```typescript
type EntityWalletResponse = EntityWalletResult | EntityWalletError;
```

**C++ Output:**
```cpp
using EntityWalletResponse = std::variant<EntityWalletResult, EntityWalletError>;
```

## Troubleshooting

### Common Issues

**Issue**: Script fails with encoding error
```
Solution: Ensure files are UTF-8 encoded
```

**Issue**: Generated code has syntax errors
```
Solution: This is expected. Manual refinement is required.
```

**Issue**: Too many files generated
```
Solution: Use --input-dir to process specific modules
```

**Issue**: Missing includes
```
Solution: Manually add required #include directives
```

## Next Steps

After running the transpiler:

1. **Review generated files** in `excpp/` directory
2. **Select a sample module** for detailed evaluation
3. **Attempt compilation** to identify common error patterns
4. **Document fix patterns** for systematic refinement
5. **Compare results** with alternative conversion approaches
6. **Measure effort** required to achieve working code

## Future Enhancements

Potential improvements to the transpiler:

- [ ] Better function body conversion
- [ ] Smart pointer inference
- [ ] Error handling pattern conversion
- [ ] Async/await to C++ coroutine conversion
- [ ] Automatic CMakeLists.txt generation
- [ ] Dependency mapping configuration
- [ ] Pattern-based code improvement passes
- [ ] Integration with C++ formatters (clang-format)

## Contributing

To improve the transpiler:

1. Identify common conversion patterns that fail
2. Add pattern handlers to the script
3. Test on various TypeScript files
4. Document conversion rules
5. Submit improvements

## License

This experimental tool is part of the ElizaOS C++ project and follows the same license.

## Conclusion

This transpiler provides a **starting point** for rapid conversion of TypeScript to C++. It successfully maintains file structure and converts basic type information, but requires significant manual refinement for production use. The tool's value lies in:

1. **Rapid prototyping** of C++ structure from TypeScript
2. **Evaluation data** for comparing conversion approaches
3. **Template generation** for manual implementation
4. **Learning tool** for understanding TypeScript→C++ mappings

**Remember**: The goal is not perfect automatic conversion, but rather to generate approximate code that serves as a foundation for manual refinement and comparison against other approaches.
