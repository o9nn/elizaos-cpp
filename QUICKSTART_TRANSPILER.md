# TypeScript to C++ Transpiler - Quick Start Guide

## TL;DR

```bash
# Convert all TypeScript files to C++ in excpp/ directory
python3 ts_to_cpp_transpiler.py

# Convert specific module
python3 ts_to_cpp_transpiler.py --input-dir ./otaku --output-dir ./excpp/otaku --verbose

# Results: 3,317 TS files → 6,540 C++ files (.hpp + .cpp pairs)
```

## What It Does

✅ **Converts** TypeScript files to approximate C++ equivalents  
✅ **Maintains** relative directory structure (`src/foo.ts` → `excpp/src/foo.{hpp,cpp}`)  
✅ **Maps** types (`string` → `std::string`, `Promise<T>` → `std::future<T>`, etc.)  
✅ **Generates** header declarations and implementation stubs  
✅ **Preserves** interfaces, classes, and type definitions  

⚠️ **Does NOT** implement function bodies (manual work required)

## File Examples

### Input: `entity.ts`
```typescript
interface EntityWalletResult {
  success: true;
  walletAddress: string;
  metadata?: Record<string, any>;
}

type EntityWalletResponse = EntityWalletResult | EntityWalletError;

async function getEntityWallet(
  runtime: IAgentRuntime,
  message: Memory,
): Promise<EntityWalletResponse> {
  // ... implementation
}
```

### Output: `entity.hpp`
```cpp
#pragma once
#include <string>
#include <optional>
#include <unordered_map>
#include <future>

namespace elizaos {

struct EntityWalletResult {
    std::string walletAddress;
    std::optional<std::unordered_map<std::string, std::any>> metadata;
};

using EntityWalletResponse = std::variant<EntityWalletResult, EntityWalletError>;

std::future<EntityWalletResponse> getEntityWallet(
    IAgentRuntime runtime,
    Memory message
);

} // namespace elizaos
```

### Output: `entity.cpp`
```cpp
#include "entity.hpp"

namespace elizaos {

// TODO: Implement function bodies
// Manual implementation required

} // namespace elizaos
```

## Type Conversion Cheat Sheet

| TypeScript | C++ |
|------------|-----|
| `string` | `std::string` |
| `number` | `double` |
| `boolean` | `bool` |
| `any` | `std::any` |
| `void` | `void` |
| `T[]` or `Array<T>` | `std::vector<T>` |
| `Map<K,V>` | `std::unordered_map<K,V>` |
| `Set<T>` | `std::unordered_set<T>` |
| `T \| U` | `std::variant<T, U>` |
| `T?` | `std::optional<T>` |
| `Promise<T>` | `std::future<T>` |
| `Function` | `std::function` |
| `Record<K,V>` | `std::unordered_map<K,V>` |

## Typical Workflow

1. **Run transpiler**:
   ```bash
   python3 ts_to_cpp_transpiler.py --input-dir ./module --verbose
   ```

2. **Review output**:
   ```bash
   ls -R excpp/module
   ```

3. **Pick a file to refine**:
   ```bash
   vim excpp/module/src/something.cpp
   ```

4. **Implement function bodies** (the TODO sections)

5. **Add to build system**:
   ```cmake
   # In CMakeLists.txt
   add_library(module excpp/module/src/something.cpp)
   ```

6. **Compile and fix errors**:
   ```bash
   cd build && cmake .. && make
   ```

7. **Repeat** for other files

## Statistics

**Latest Run Results:**
- Input: 3,942 TypeScript files found
- Processed: 3,317 files (84%)
- Skipped: 625 files (tests, configs)
- Output: 6,540 files (3,270 headers + 3,270 implementations)
- Errors: 0

## Known Issues & Workarounds

### Issue: Generated code doesn't compile
**This is expected!** The transpiler generates approximations. You must:
1. Implement function bodies
2. Add missing includes
3. Fix type mismatches
4. Handle async operations properly
5. Manage memory with smart pointers

### Issue: Complex TypeScript patterns not converted
Arrow functions, closures, decorators, and advanced TypeScript features are not handled.  
**Workaround**: Manually rewrite these sections in C++.

### Issue: Too many files generated
Use `--input-dir` to process specific modules:
```bash
python3 ts_to_cpp_transpiler.py --input-dir ./specific_module
```

## Next Steps

1. Review `TRANSPILER_README.md` for comprehensive documentation
2. Select a module to evaluate
3. Compare effort vs. complete rewrite
4. Document findings for TS→C++ vs. TS→Z++→C++ comparison

## Directory Structure

```
excpp/                          # Auto-generated output
├── otaku/                      # DeFi agent module
│   ├── src/
│   │   ├── index.{hpp,cpp}
│   │   ├── character.{hpp,cpp}
│   │   └── plugins/...
├── eliza/                      # Core Eliza engine
│   └── packages/core/src/...
├── SWEagent/                   # Software engineering agent
├── elizaos.github.io/          # Website components
└── [30+ other modules]
```

## Support & Troubleshooting

- Read `TRANSPILER_README.md` for detailed information
- Check `ts_to_cpp_transpiler.py` comments for implementation details
- Use `--verbose` flag for debugging
- Review generated files to understand conversion patterns

---

**Remember**: This is an experimental tool for rapid conversion evaluation, not production code generation!
