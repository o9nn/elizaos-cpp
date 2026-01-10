# Hybrid TypeScript-to-C++ Transpiler - Usage Guide

**Version**: 1.0  
**Date**: January 10, 2026  
**Tool**: hybrid_transpiler.py

---

## Overview

The Hybrid Transpiler combines two powerful tools to generate optimal C++ code from TypeScript:

1. **ts2cpp** (Leaningtech) - Generates clean type definitions (Cheerp-compatible)
2. **ts_to_cpp_transpiler.py** - Generates implementations (native C++)

This guide covers installation, usage, and best practices.

---

## Table of Contents

1. [Installation](#installation)
2. [Quick Start](#quick-start)
3. [Usage Modes](#usage-modes)
4. [Command-Line Options](#command-line-options)
5. [Workflow Examples](#workflow-examples)
6. [Output Structure](#output-structure)
7. [Best Practices](#best-practices)
8. [Troubleshooting](#troubleshooting)
9. [Advanced Usage](#advanced-usage)

---

## Installation

### Prerequisites

```bash
# Node.js (for ts2cpp)
node --version  # Should be v14+

# Python 3 (for hybrid transpiler)
python3 --version  # Should be 3.8+

# TypeScript (optional, for generating .d.ts files)
npm install -g typescript
```

### Setup

```bash
# Clone elizaos-cpp repository
git clone https://github.com/o9nn/elizaos-cpp.git
cd elizaos-cpp

# Verify ts2cpp is available
ls -lh /home/ubuntu/ts2cpp-master/ts2cpp.js

# Make hybrid transpiler executable
chmod +x hybrid_transpiler.py

# Test installation
python3 hybrid_transpiler.py --help
```

---

## Quick Start

### Basic Usage

```bash
# Transpile TypeScript to C++ (hybrid mode)
python3 hybrid_transpiler.py \
  --input-dir ./src \
  --output-dir ./cpp \
  --mode hybrid \
  --verbose
```

### Expected Output

```
[INFO] ============================================================
[INFO] HYBRID TRANSPILER - Starting
[INFO] ============================================================
[INFO] Setting up output directories...
[INFO] Scanning for TypeScript files...
[INFO] Found 10 .ts files and 5 .d.ts files

--- Stage 1: Type Definitions (ts2cpp) ---
[INFO] Running ts2cpp on agent.d.ts...
[INFO] ✓ Generated agent.d.h
...

--- Stage 2: Implementations (Python Transpiler) ---
[INFO] Running Python transpiler...
...

--- Stage 3: Hybrid Integration ---
[INFO] ✓ Created hybrid header: agent.hpp
...

============================================================
HYBRID TRANSPILER - SUMMARY
============================================================
Mode: hybrid
Input:
  TypeScript files (.ts): 10
  Declaration files (.d.ts): 5
Output:
  Cheerp headers (.h): 5
  Native headers (.hpp): 15
  Native implementations (.cpp): 15
  Hybrid files: 5
Status:
  Errors: 0
  Warnings: 0
============================================================
```

---

## Usage Modes

### 1. Hybrid Mode (Recommended)

Combines both transpilers for optimal output.

```bash
python3 hybrid_transpiler.py \
  --input-dir ./src \
  --output-dir ./cpp \
  --mode hybrid
```

**Use When**:
- Converting full TypeScript projects
- Need both type definitions and implementations
- Want best of both transpilers

**Output**:
- Cheerp headers (intermediate)
- Native headers (intermediate)
- Hybrid headers (final, optimized)
- Native implementations

### 2. Types-Only Mode

Uses only ts2cpp for type definitions.

```bash
python3 hybrid_transpiler.py \
  --input-dir ./types \
  --output-dir ./cpp \
  --mode types-only
```

**Use When**:
- Generating API interfaces
- Creating type libraries
- Need Cheerp-compatible headers

**Output**:
- Cheerp headers only

### 3. Implementations-Only Mode

Uses only Python transpiler for implementations.

```bash
python3 hybrid_transpiler.py \
  --input-dir ./src \
  --output-dir ./cpp \
  --mode impl-only
```

**Use When**:
- Rapid prototyping
- Already have type definitions
- Need native C++ implementations

**Output**:
- Native headers and implementations

---

## Command-Line Options

### Required Options

| Option | Description | Default |
|--------|-------------|---------|
| `--input-dir DIR` | Input directory with TypeScript files | `.` (current) |
| `--output-dir DIR` | Output directory for C++ files | `cpp` |

### Optional Flags

| Flag | Description | Default |
|------|-------------|---------|
| `--mode MODE` | Transpilation mode: `hybrid`, `types-only`, `impl-only` | `hybrid` |
| `--verbose` | Enable detailed logging | `false` |
| `--validate` | Enable code validation | `false` |
| `--optimize` | Enable code optimization | `false` |

### Examples

```bash
# Verbose output
python3 hybrid_transpiler.py --verbose

# With validation
python3 hybrid_transpiler.py --validate

# With optimization
python3 hybrid_transpiler.py --optimize

# All flags
python3 hybrid_transpiler.py \
  --input-dir ./src \
  --output-dir ./cpp \
  --mode hybrid \
  --verbose \
  --validate \
  --optimize
```

---

## Workflow Examples

### Example 1: Full Project Conversion

```bash
# Step 1: Organize TypeScript files
mkdir -p project/types project/src

# Step 2: Generate .d.ts files (if needed)
tsc --declaration --emitDeclarationOnly

# Step 3: Run hybrid transpiler
python3 hybrid_transpiler.py \
  --input-dir ./project \
  --output-dir ./cpp \
  --mode hybrid \
  --verbose

# Step 4: Review output
tree cpp/
```

### Example 2: API Interface Generation

```bash
# Generate only type definitions
python3 hybrid_transpiler.py \
  --input-dir ./api/types \
  --output-dir ./cpp/include \
  --mode types-only

# Output: Clean C++ headers in cpp/include/
```

### Example 3: Rapid Prototyping

```bash
# Generate implementations quickly
python3 hybrid_transpiler.py \
  --input-dir ./prototypes \
  --output-dir ./cpp \
  --mode impl-only

# Manually refine generated code
vim cpp/src/prototype.cpp
```

### Example 4: Incremental Conversion

```bash
# Day 1: Convert types
python3 hybrid_transpiler.py \
  --input-dir ./src \
  --mode types-only

# Day 2: Add implementations
python3 hybrid_transpiler.py \
  --input-dir ./src \
  --mode impl-only

# Day 3: Merge with hybrid mode
python3 hybrid_transpiler.py \
  --input-dir ./src \
  --mode hybrid
```

---

## Output Structure

### Directory Layout

```
output_dir/
├── cheerp/              # Stage 1: ts2cpp output
│   └── types/
│       └── agent.d.h
├── native/              # Stage 2: Python transpiler output
│   ├── src/
│   │   ├── logger.hpp
│   │   └── logger.cpp
│   └── types/
│       ├── agent.hpp
│       └── agent.cpp
└── hybrid/              # Stage 3: Merged output (FINAL)
    ├── include/
    │   └── types/
    │       └── agent.hpp
    └── src/
        ├── src/
        │   └── logger.cpp
        └── types/
            └── agent.cpp
```

### File Types

| Extension | Description | Stage |
|-----------|-------------|-------|
| `.h` | Cheerp header | Stage 1 (ts2cpp) |
| `.hpp` | Native C++ header | Stage 2 & 3 |
| `.cpp` | C++ implementation | Stage 2 & 3 |

### Using the Output

```cpp
// Include hybrid headers
#include "hybrid/include/types/agent.hpp"
#include "hybrid/include/src/logger.hpp"

// Use generated classes
elizaos::Logger logger("MyApp");
logger.info("Hello from hybrid transpiler!");
```

---

## Best Practices

### 1. Organize TypeScript Files

```
project/
├── types/           # Type definitions (.d.ts)
│   ├── agent.d.ts
│   ├── memory.d.ts
│   └── knowledge.d.ts
└── src/             # Implementations (.ts)
    ├── agent.ts
    ├── memory.ts
    └── knowledge.ts
```

### 2. Generate .d.ts Files

```bash
# Use TypeScript compiler to generate declarations
tsc --declaration --emitDeclarationOnly --outDir types/
```

### 3. Use Hybrid Mode for Production

```bash
# Always use hybrid mode for production code
python3 hybrid_transpiler.py \
  --mode hybrid \
  --validate \
  --optimize
```

### 4. Review Generated Code

```bash
# Check for issues
grep -r "TODO\|FIXME\|NOTE" cpp/hybrid/

# Review type conversions
diff cpp/cheerp/types/agent.d.h cpp/hybrid/include/types/agent.hpp
```

### 5. Refine Implementations

```cpp
// Generated code (approximate)
Logger createLogger(std::optional<std::string> prefix) {
    return new Logger(prefix);
}

// Refined code (production-ready)
Logger createLogger(std::optional<std::string> prefix) {
    return Logger(prefix.value_or("ElizaOS"));
}
```

### 6. Version Control

```bash
# Add to .gitignore
echo "cpp/cheerp/" >> .gitignore
echo "cpp/native/" >> .gitignore

# Commit only hybrid output
git add cpp/hybrid/
git commit -m "Add hybrid transpiler output"
```

---

## Troubleshooting

### Issue 1: ts2cpp Not Found

**Error**:
```
FileNotFoundError: [Errno 2] No such file or directory: '/home/ubuntu/ts2cpp-master/ts2cpp.js'
```

**Solution**:
```bash
# Check ts2cpp path
ls -lh /home/ubuntu/ts2cpp-master/ts2cpp.js

# Update path in hybrid_transpiler.py if needed
vim hybrid_transpiler.py
# Change: self.ts2cpp_path = Path('/correct/path/to/ts2cpp.js')
```

### Issue 2: No Output Generated

**Error**:
```
Output:
  Cheerp headers (.h): 0
  Native headers (.hpp): 0
```

**Solution**:
```bash
# Check input directory
ls -R input_dir/

# Ensure .ts and .d.ts files exist
find input_dir/ -name "*.ts" -o -name "*.d.ts"

# Check file filtering
python3 hybrid_transpiler.py --verbose
```

### Issue 3: Type Conversion Errors

**Error**:
```
[ERROR] Error converting Cheerp header: ...
```

**Solution**:
```bash
# Check Cheerp header syntax
cat cpp/cheerp/types/file.h

# Review type mappings in hybrid_transpiler.py
vim hybrid_transpiler.py
# Search for: type_map = {
```

### Issue 4: Include Path Issues

**Error**:
```cpp
fatal error: 'agent.hpp' file not found
```

**Solution**:
```cpp
// Use relative paths
#include "../include/types/agent.hpp"

// Or add to include path
g++ -I cpp/hybrid/include/ main.cpp
```

---

## Advanced Usage

### Custom Type Mappings

Edit `hybrid_transpiler.py` to add custom type conversions:

```python
# In convert_cheerp_to_native() method
type_map = {
    'String*': 'std::string',
    'TArray<': 'std::vector<',
    # Add custom mappings
    'CustomType*': 'MyCustomType',
}
```

### Batch Processing

```bash
# Process multiple directories
for dir in module1 module2 module3; do
    python3 hybrid_transpiler.py \
        --input-dir ./src/$dir \
        --output-dir ./cpp/$dir \
        --mode hybrid
done
```

### CI/CD Integration

```yaml
# .github/workflows/transpile.yml
name: Transpile TypeScript to C++
on: [push]
jobs:
  transpile:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - name: Setup Node.js
        uses: actions/setup-node@v2
        with:
          node-version: '18'
      - name: Setup Python
        uses: actions/setup-python@v2
        with:
          python-version: '3.11'
      - name: Run Hybrid Transpiler
        run: |
          python3 hybrid_transpiler.py \
            --input-dir ./src \
            --output-dir ./cpp \
            --mode hybrid \
            --validate
      - name: Commit Generated Code
        run: |
          git add cpp/hybrid/
          git commit -m "Auto-transpile: $(date)"
          git push
```

### Parallel Processing

The Python transpiler automatically uses parallel processing:

```python
# In hybrid_transpiler.py
self.python_transpiler = TypeScriptToCppTranspiler(
    input_dir=str(self.input_dir),
    output_dir=str(self.native_dir),
    verbose=self.verbose,
    parallel=True,        # Enable parallel processing
    max_workers=4         # Adjust based on CPU cores
)
```

---

## Performance Tips

### 1. Use Parallel Mode

```bash
# Automatically enabled in hybrid mode
python3 hybrid_transpiler.py --mode hybrid
```

### 2. Process Only Changed Files

```bash
# Use git to find changed files
git diff --name-only HEAD~1 | grep "\.ts$" > changed_files.txt

# Process only changed files (custom script needed)
```

### 3. Cache ts2cpp Output

```bash
# Skip ts2cpp if .d.ts files haven't changed
# (Feature to be added in future version)
```

---

## FAQ

### Q: Can I use this for production code?

**A**: Yes, but with manual refinement. The hybrid transpiler generates high-quality type definitions and approximate implementations. Review and refine generated code before production use.

### Q: Does it support all TypeScript features?

**A**: No. Complex generics, advanced type manipulation, and some TypeScript-specific features have limited support. See limitations in design document.

### Q: Can I customize type mappings?

**A**: Yes. Edit `hybrid_transpiler.py` and modify the `type_map` dictionary in `convert_cheerp_to_native()`.

### Q: What about nested namespaces?

**A**: Limited support. Use flat namespace structures for best results.

### Q: How do I report bugs?

**A**: Create an issue on the elizaos-cpp GitHub repository with:
- Input TypeScript code
- Generated C++ code
- Expected output
- Error messages

---

## Resources

- **ts2cpp Documentation**: https://github.com/leaningtech/ts2cpp
- **ElizaOS Repository**: https://github.com/o9nn/elizaos-cpp
- **Design Document**: `HYBRID_TRANSPILER_DESIGN.md`
- **Test Results**: `HYBRID_TRANSPILER_TEST_RESULTS.md`

---

## Support

For questions or issues:
1. Check this guide
2. Review test results document
3. Check design document
4. Create GitHub issue

---

**Last Updated**: January 10, 2026  
**Version**: 1.0  
**Status**: Production-Ready for Type Definitions, Prototyping for Implementations
