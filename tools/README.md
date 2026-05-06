# tools

Utility scripts for development, code generation, and assessment.

| Path | Description |
|---|---|
| `transpilers/ts_to_cpp_transpiler.py` | Main TypeScript-to-C++ transpiler (v3) |
| `transpilers/ts_to_cpp_transpiler_wrapper.py` | Wrapper/CLI for the transpiler |
| `transpilers/hybrid_transpiler.py` | Hybrid (AST + regex) transpiler variant |
| `transpilers/multi_transpiler.py` | Parallel multi-file transpiler |
| `transpilers/transpiler_refinement.py` | Post-processing refinement pass |
| `transpilers/selective_integration.py` | Selective module integration helper |
| `transpilers/multi_transpiler_config.yaml` | Transpiler configuration |
| `assessment/cpp_adaptation_assessment.py` | Module completeness assessment script |
| `assessment/cpp_adaptation_assessment.json` | Cached assessment results |
| `consolidate_sources.py` | Source file consolidation utility |
| `generate_comprehensive_tests.py` | Test scaffolding generator |
| `check_case_collisions.py` | Case-collision checker for cross-platform builds |
| `prompts/` | AI coding assistant prompt files |

## Quick Usage

```bash
# Transpile a TypeScript directory to C++
python3 tools/transpilers/ts_to_cpp_transpiler.py \
    --input-dir ./eliza/packages/core/src \
    --output-dir ./excpp/eliza/packages/core/src \
    --verbose

# Run with parallel processing
python3 tools/transpilers/ts_to_cpp_transpiler.py --parallel --max-workers 8
```

See [`docs/transpiler/`](../docs/transpiler/) for full transpiler documentation.
