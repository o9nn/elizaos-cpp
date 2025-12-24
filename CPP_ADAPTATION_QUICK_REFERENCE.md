# Quick Reference: C++ Adaptation Tools

## Assessment Tool

### Basic Usage
```bash
# Run full assessment
python3 cpp_adaptation_assessment.py

# Save to file
python3 cpp_adaptation_assessment.py --output report.md

# Generate JSON data
python3 cpp_adaptation_assessment.py --json data.json

# Both outputs
python3 cpp_adaptation_assessment.py --output report.md --json data.json
```

### What It Does
- Scans TypeScript sources for reference
- Analyzes cpp/ implementations
- Examines excpp/ transpiler scaffolding
- Calculates completeness scores
- Identifies enhancement opportunities

### Output Format
- Module-by-module breakdown
- Completeness percentages
- Line counts (TS, cpp, excpp)
- Status classification (complete/partial/minimal/missing)
- Recommendations for next steps

---

## Integration Tool

### Commands

#### 1. Show Module Structure
```bash
python3 selective_integration.py --module MODULE_NAME --show-structure
```
**Example:**
```bash
python3 selective_integration.py --module otaku --show-structure
```
**Output:** Shows directory tree and file counts

#### 2. Compare Implementations
```bash
python3 selective_integration.py --module MODULE_NAME --compare
```
**Example:**
```bash
python3 selective_integration.py --module eliza --compare
```
**Output:** Shows cpp/ vs excpp/ comparison with available files

#### 3. Dry Run (Plan Integration)
```bash
python3 selective_integration.py \
  --module MODULE_NAME \
  --files PATTERN1 PATTERN2 ... \
  --dry-run
```
**Example:**
```bash
python3 selective_integration.py \
  --module otaku \
  --files "src/managers/*.hpp" "src/types/*.hpp" \
  --dry-run
```
**Output:** Shows what would be copied without actually copying

#### 4. Actual Integration
```bash
python3 selective_integration.py \
  --module MODULE_NAME \
  --files PATTERN1 PATTERN2 ...
```
**Example:**
```bash
python3 selective_integration.py \
  --module otaku \
  --files "src/managers/cdp-transaction-manager.*"
```
**Output:** Copies and refines files from excpp/ to cpp/

#### 5. Integration Without Refinement
```bash
python3 selective_integration.py \
  --module MODULE_NAME \
  --files PATTERN \
  --no-refine
```
**Use when:** You want raw transpiler output without automatic cleanup

---

## Common Workflows

### Workflow 1: Start New Module Enhancement

```bash
# 1. Assess current state
python3 selective_integration.py --module mymodule --compare

# 2. See what's available
python3 selective_integration.py --module mymodule --show-structure

# 3. Plan type definitions integration
python3 selective_integration.py \
  --module mymodule \
  --files "src/types/*.hpp" \
  --dry-run

# 4. Copy type definitions
python3 selective_integration.py \
  --module mymodule \
  --files "src/types/*.hpp"

# 5. Plan core implementation
python3 selective_integration.py \
  --module mymodule \
  --files "src/*.hpp" "src/*.cpp" \
  --dry-run

# 6. Copy core files
python3 selective_integration.py \
  --module mymodule \
  --files "src/*.hpp" "src/*.cpp"
```

### Workflow 2: Enhance Existing Module

```bash
# 1. Check what's missing
python3 selective_integration.py --module existingmodule --compare

# 2. Identify specific components
python3 selective_integration.py --module existingmodule --show-structure

# 3. Selectively add components
python3 selective_integration.py \
  --module existingmodule \
  --files "src/managers/*.hpp" "src/managers/*.cpp"

# 4. Add plugins
python3 selective_integration.py \
  --module existingmodule \
  --files "src/plugins/plugin-*/src/*.hpp"
```

### Workflow 3: Full Module Analysis

```bash
# Run assessment on all modules
python3 cpp_adaptation_assessment.py --output full_report.md --json data.json

# Review report
cat full_report.md | less

# Find high-priority modules
grep -A5 "HIGH-PRIORITY" full_report.md

# Check specific module
python3 selective_integration.py --module TARGET_MODULE --compare
```

---

## File Pattern Examples

### Basic Patterns
```bash
# All headers in directory
--files "*.hpp"

# All implementations in directory
--files "*.cpp"

# Both headers and implementations
--files "*.hpp" "*.cpp"

# Specific file
--files "manager.hpp"
```

### Directory Patterns
```bash
# All files in src/
--files "src/*.hpp" "src/*.cpp"

# All files in subdirectories
--files "src/types/*.hpp"

# Multiple directories
--files "src/types/*.hpp" "src/utils/*.hpp"

# Recursive patterns
--files "src/**/*.hpp"  # All .hpp in src/ and subdirs
```

### Module-Specific Examples

#### otaku (DeFi Agent)
```bash
# Core types
--files "src/plugins/plugin-cdp/constants/*.hpp"

# Transaction manager
--files "src/managers/cdp-transaction-manager.*"

# Specific plugin
--files "src/plugins/plugin-relay/src/*.hpp" \
        "src/plugins/plugin-relay/src/*.cpp"

# All plugin types
--files "src/plugins/*/src/types/*.hpp"
```

#### eliza (Conversation Engine)
```bash
# Core types
--files "src/types/*.hpp"

# Conversation management
--files "src/conversation/*.hpp" "src/conversation/*.cpp"

# Pattern matching
--files "src/patterns/*.hpp" "src/patterns/*.cpp"

# Memory integration
--files "src/memory/*.hpp" "src/memory/*.cpp"
```

---

## Tips & Best Practices

### 1. Always Dry Run First
```bash
# See what you'll get before copying
python3 selective_integration.py --module mymodule --files "src/*" --dry-run
```

### 2. Start with Type Definitions
```bash
# Types are usually ready to use
python3 selective_integration.py --module mymodule --files "src/types/*.hpp"
```

### 3. Copy Headers Before Implementations
```bash
# Get structure first
python3 selective_integration.py --module mymodule --files "src/*.hpp"

# Then add implementations
python3 selective_integration.py --module mymodule --files "src/*.cpp"
```

### 4. Incremental Integration
```bash
# Don't copy everything at once
# Do one component at a time:

# Step 1: Types
python3 selective_integration.py --module mymodule --files "src/types/*.hpp"

# Step 2: Core
python3 selective_integration.py --module mymodule --files "src/core.*"

# Step 3: Managers
python3 selective_integration.py --module mymodule --files "src/managers/*"

# Step 4: Plugins (one by one)
python3 selective_integration.py --module mymodule --files "src/plugins/plugin-a/*"
```

### 5. Skip Frontend Code
```bash
# Frontend components aren't useful for C++ backend
# Skip: src/frontend/*, src/components/*, src/hooks/*
# Focus on: src/managers/*, src/plugins/*, src/services/*, src/types/*
```

---

## Troubleshooting

### Issue: Module Not Found
```
Error: Module otaku not found in excpp/
```
**Solution:** Check module name spelling, use exact name from excpp/ directory

### Issue: No Files Match Pattern
```
No files matched pattern: src/xyz/*.hpp
```
**Solution:** 
1. Run `--show-structure` to see available files
2. Check pattern syntax
3. Verify directory exists in excpp/module/

### Issue: Permission Denied
```
Error: Permission denied writing to cpp/module/
```
**Solution:** Check write permissions, may need to create directory first

### Issue: Build Errors After Integration
**Solution:**
1. Review copied files for syntax issues
2. Check include paths in CMakeLists.txt
3. Add missing dependencies
4. Refine transpiler output (remove `as any`, fix types)

---

## Next Steps After Integration

### 1. Review Copied Files
```bash
# Check what was copied
ls -la cpp/MODULE/src/

# View file content
cat cpp/MODULE/src/types/mytype.hpp
```

### 2. Refine Implementations
- Remove auto-generated comments
- Replace TODO stubs with real implementations
- Add error handling
- Ensure thread safety

### 3. Add to Build System
```cmake
# cpp/MODULE/CMakeLists.txt
add_library(MODULE
    src/file1.cpp
    src/file2.cpp
)

target_link_libraries(MODULE
    PRIVATE nlohmann_json::nlohmann_json
)
```

### 4. Write Tests
```cpp
// cpp/MODULE/tests/test_component.cpp
#include <gtest/gtest.h>
#include "elizaos/MODULE/component.hpp"

TEST(Component, BasicTest) {
    // Test implementation
}
```

### 5. Build and Test
```bash
cd build
cmake ..
make MODULE -j$(nproc)
ctest -R MODULE --verbose
```

---

## Quick Module Assessment

```bash
# One-liner to check a module
python3 selective_integration.py --module MODULE_NAME --compare | grep -E "(Files|Lines|ratio)"
```

**Example Output:**
```
cpp/otaku:
  Files: 1
  Lines: 233
excpp/otaku:
  Files: 682
  Lines: 35,789
Implementation ratio: 0.7%
```

---

## Getting Help

### Tool Help
```bash
# Assessment tool
python3 cpp_adaptation_assessment.py --help

# Integration tool
python3 selective_integration.py --help
```

### Documentation
- **Full Assessment:** CPP_ADAPTATION_ASSESSMENT.md
- **Strategy & Roadmap:** CPP_ADAPTATION_NEXT_STEPS.md
- **Practical Example:** OTAKU_ENHANCEMENT_EXAMPLE.md
- **Summary:** CPP_ADAPTATION_SUMMARY.md
- **This Guide:** CPP_ADAPTATION_QUICK_REFERENCE.md

---

**Last Updated:** December 24, 2024  
**Tools Version:** 1.0
