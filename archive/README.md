# Archive

This directory contains deprecated, superseded, or experimental content that has been retired from the active codebase.

## Contents

| Directory / File | Reason Archived |
|---|---|
| `binaries/` | Compiled test/demo executables (should not be committed) |
| `logs/` | Build and runtime log files |
| `REMAINING_IMPLEMENTATIONS.cpp` | Placeholder stub file — superseded |
| `INTEGRATION_*.cpp` | Loose integration snippets incorporated into packages |
| `.gitignore.backup` | Backup of root `.gitignore` |

## Large Deprecated Directories (gitignore-excluded)

The following large auto-generated/obsolete directories remain in the working tree but are
excluded from version control via the root `.gitignore`. They can be safely deleted:

| Directory | Reason Deprecated |
|---|---|
| `excpp_old/` | Superseded transpiler output — replaced by `excpp/` |
| `excpp_old_backup/` | Backup of superseded transpiler output |
| `excpp1/` | First-generation transpiler output — superseded |
| `cpp_generated/` | Intermediate generated code — superseded by `cpp/` packages |
| `cpp_hybrid/` | Experimental hybrid transpiler output |
| `cpp_merge/` | Experimental merge of transpiler outputs |
| `output_multi_full/` | Multi-transpiler full output — superseded |

To clean them up locally:
```bash
rm -rf excpp_old excpp_old_backup excpp1 cpp_generated cpp_hybrid cpp_merge output_multi_full
```

All contents of this `archive/` directory itself are excluded from version control
via the `.gitignore` in this directory, except for this `README.md`.

To restore archived content, check git history before the archival commit.
