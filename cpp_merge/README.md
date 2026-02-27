# ElizaOS C++ Merge Directory

This directory contains the merged integration of transpiled TypeScript code with the core C++ implementation of ElizaOS.

## Directory Structure

The merge directory preserves the original TypeScript directory hierarchy while integrating the existing C++ core functionality:

```
cpp_merge/
├── CMakeLists.txt              # Main build configuration
├── fix_transpiler_errors.py    # Script to fix common transpiler issues
├── README.md                   # This file
├── common/                     # Common utilities (unique root files)
├── eliza/                      # Core Eliza conversation engine
│   ├── include/                # Header files from cpp
│   ├── cpp_core/               # Existing cpp package code
│   └── packages/               # Transpiled TypeScript packages
├── SWEagent/                   # Software Engineering Agent
├── auto.fun/                   # Auto.fun platform integration
├── autofun-idl/                # Auto.fun IDL definitions
├── autonomous-starter/         # Autonomous agent starter
├── classified/                 # Classified module
├── discrub-ext/                # Discord content moderation
├── eliza-3d-hyperfy-starter/   # 3D virtual world integration
├── eliza-nextjs-starter/       # Next.js integration starter
├── elizaos.github.io/          # GitHub.io static site
├── elizas-list/                # Eliza instance directory
├── elizas-world/               # Virtual world integration
├── LiveVideoChat/              # Real-time video chat
├── mcp-gateway/                # Model Context Protocol gateway
├── mobile/                     # Mobile platform support
├── otaku/                      # DeFi agent implementation
├── otc-agent/                  # OTC trading agent
├── packages/                   # Core packages
├── plugin-specification/       # Plugin interface definitions
├── registry/                   # Service registry
├── sandbox-template-cloud/     # Sandbox templates
├── spartan/                    # Spartan protocol support
├── the-org/                    # Organization management
├── trust_scoreboard/           # Trust scoring system
└── vercel-api/                 # Vercel API integration
```

## Integration Philosophy

Each subdirectory contains:

1. **Transpiled TypeScript Code**: Files generated from the TypeScript to C++ transpiler
2. **cpp_core/**: The original C++ implementation from the `cpp/packages/` directory
3. **include/**: Header files from `cpp/include/elizaos/`

The integration aims to:
- Preserve the TypeScript directory hierarchy for consistency
- Integrate the production-quality C++ core implementations
- Fix common transpiler errors and syntax issues
- Enable seamless compilation with CMake

## Building

```bash
# From the cpp_merge directory
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## Transpiler Error Fixes

The `fix_transpiler_errors.py` script addresses common issues from TypeScript to C++ conversion:

- Arrow function syntax (`=>`) → C++ lambdas
- `process.*` references → C++ equivalents
- `import.meta` references → removed/replaced
- `===` and `!==` → `==` and `!=`
- Template literals → string concatenation
- Optional chaining (`?.`) → explicit checks
- Nullish coalescing (`??`) → `||` operator
- TypeScript type annotations → removed
- JavaScript string methods → C++ equivalents

Run the fix script:
```bash
python3 fix_transpiler_errors.py .
```

## Module Integration Status

| Module | Transpiled | cpp_core | Headers | Status |
|--------|-----------|----------|---------|--------|
| eliza | ✅ | ✅ | ✅ | Merged |
| SWEagent | ✅ | ✅ | ✅ | Merged |
| auto.fun | ✅ | ✅ | ✅ | Merged |
| autonomous-starter | ✅ | ✅ | ✅ | Merged |
| mcp-gateway | ✅ | ✅ | ✅ | Merged |
| otaku | ✅ | ✅ | ✅ | Merged |
| otc-agent | ✅ | ✅ | ✅ | Merged |
| eliza-3d-hyperfy-starter | ✅ | ✅ | ✅ | Merged |
| eliza-nextjs-starter | ✅ | ✅ | ✅ | Merged |
| plugin-specification | ✅ | ✅ | ✅ | Merged |
| spartan | ✅ | - | ✅ | Transpiled |
| the-org | ✅ | - | ✅ | Transpiled |
| trust_scoreboard | ✅ | - | ✅ | Transpiled |
| vercel-api | ✅ | - | ✅ | Transpiled |
| common | ✅ | - | - | Transpiled |

## Notes

- Files marked with `/* NOTE: Auto-converted from TypeScript - may need refinement */` may require manual review
- Complex TypeScript patterns may not convert correctly and need manual attention
- The cpp_core implementations are production-quality and should be preferred when functionality overlaps
- External dependencies must be manually mapped to C++ equivalents

## Related Documentation

- [TRANSPILER_README.md](../TRANSPILER_README.md) - Transpiler documentation
- [CPP_FUNCTIONAL_COMPLETENESS_REPORT.md](../CPP_FUNCTIONAL_COMPLETENESS_REPORT.md) - 90% completion analysis
- [TECH_ARCHITECTURE.md](../TECH_ARCHITECTURE.md) - System architecture
