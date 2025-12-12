# Placeholder File Renaming Plan

## Completed Implementations to Rename (>500 lines)

These are fully implemented modules that should be renamed from "placeholder.cpp" to proper implementation names:

| Module | Current File | Lines | New Filename | Priority |
|--------|-------------|-------|--------------|----------|
| **characters** | cpp/characters/src/placeholder.cpp | 1,347 | characters.cpp | HIGH |
| **vercel_api** | cpp/vercel_api/src/placeholder.cpp | 1,308 | vercel_api.cpp | HIGH |
| **eliza** | cpp/eliza/src/placeholder.cpp | 1,306 | eliza.cpp | HIGH |
| **characterfile** | cpp/characterfile/src/placeholder.cpp | 967 | characterfile.cpp | HIGH |
| **agentbrowser** | cpp/agentbrowser/src/placeholder.cpp | 893 | agentbrowser.cpp | HIGH |
| **elizaos_github_io** | cpp/elizaos_github_io/src/placeholder.cpp | 846 | elizaos_github_io.cpp | HIGH |
| **knowledge** | cpp/knowledge/src/placeholder.cpp | 841 | knowledge.cpp | HIGH |
| **plugin_specification** | cpp/plugin_specification/src/placeholder.cpp | 836 | plugin_specification.cpp | HIGH |
| **livevideochat** | cpp/livevideochat/src/placeholder.cpp | 687 | livevideochat.cpp | HIGH |
| **website** | cpp/website/src/placeholder.cpp | 588 | website.cpp | MEDIUM |

## Partial Implementations to Rename (100-500 lines)

These have substantial code and should be renamed:

| Module | Current File | Lines | New Filename | Priority |
|--------|-------------|-------|--------------|----------|
| **mcp_gateway** | cpp/mcp_gateway/src/placeholder.cpp | 413 | mcp_gateway.cpp | MEDIUM |
| **eliza_3d_hyperfy_starter** | cpp/eliza_3d_hyperfy_starter/src/placeholder.cpp | 333 | eliza_3d_hyperfy_starter.cpp | MEDIUM |
| **sweagent** | cpp/sweagent/src/placeholder.cpp | 287 | sweagent.cpp | MEDIUM |

## Already Renamed (New Implementations)

These were just implemented and already have proper names:

| Module | Current File | Lines | Status |
|--------|-------------|-------|--------|
| **otaku** | cpp/otaku/src/otaku.cpp | 300+ | ✅ Already renamed |
| **otc_agent** | cpp/otc_agent/src/otc_agent.cpp | 350+ | ✅ Already renamed |

Note: These modules still have old placeholder.cpp files (50 and 37 lines) that should be deleted.

## Keep as Placeholder (True Placeholders <50 lines)

These are genuinely incomplete and should keep the placeholder name:

| Module | Current File | Lines | Status |
|--------|-------------|-------|--------|
| **brandkit** | cpp/brandkit/src/placeholder.cpp | 8 | Keep as placeholder |
| **eliza_nextjs_starter** | cpp/eliza_nextjs_starter/src/placeholder.cpp | 8 | Keep as placeholder |
| **eliza_plugin_starter** | cpp/eliza_plugin_starter/src/placeholder.cpp | 8 | Keep as placeholder |
| **hat** | cpp/hat/src/placeholder.cpp | 8 | Keep as placeholder |
| **the_org** | cpp/the_org/src/placeholder.cpp | 8 | Keep as placeholder |
| **trust_scoreboard** | cpp/trust_scoreboard/src/placeholder.cpp | 8 | Keep as placeholder |
| **workgroups** | cpp/workgroups/src/placeholder.cpp | 8 | Keep as placeholder |
| **classified** | cpp/classified/src/placeholder.cpp | 23 | Keep as placeholder |

## Renaming Strategy

### Phase 1: High Priority (10 modules, >500 lines each)
1. Rename placeholder.cpp to {module}.cpp
2. Update CMakeLists.txt to reference new filename
3. Verify build still works
4. Commit changes

### Phase 2: Medium Priority (3 modules, 100-500 lines)
1. Same process as Phase 1

### Phase 3: Cleanup
1. Delete old placeholder.cpp files for otaku and otc_agent
2. Verify no references to old files remain

## CMakeLists.txt Updates Required

Each renamed file requires updating the corresponding CMakeLists.txt:

**Before**:
```cmake
add_library(elizaos-{module} STATIC
    src/placeholder.cpp
)
```

**After**:
```cmake
add_library(elizaos-{module} STATIC
    src/{module}.cpp
)
```

## Verification Steps

After each rename:
1. `cmake -B build -DCMAKE_BUILD_TYPE=Release`
2. `cmake --build build --config Release`
3. `cd build && ctest --output-on-failure`
4. Verify no errors

## Total Files to Rename

- **High Priority**: 10 files
- **Medium Priority**: 3 files
- **Cleanup**: 2 files (delete old placeholders)
- **Total**: 15 operations

## Expected Benefits

1. **Clarity**: Code structure reflects actual implementation status
2. **Maintainability**: Easier to identify what's complete vs incomplete
3. **Professionalism**: Proper naming conventions
4. **Discoverability**: Developers can find implementations easily
5. **Accuracy**: Repository metrics will be more accurate
