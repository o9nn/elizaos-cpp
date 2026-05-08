---
name: elizaos-cpp-ksm-creator
description: Specialized Copilot skill for /skill-creator(/ksm-evolve("elizaos-cpp")) repair, validation, and Manus synergy cycles.
---

# ElizaOS-C++ KSM Creator Copilot Skill

This Copilot skill mirrors the Manus skill composition:

```text
/skill-creator( /ksm-evolve( "elizaos-cpp" ) )
```

Use it when the task asks to **repair**, **evolve**, **complete**, **restructure**, **test**, or **crystallize reusable knowledge** for the ElizaOS-C++ cognitive backend. The skill is intentionally narrower than the general `elizaos-cpp` agent. It exists to keep local code-generation work aligned with the broader Manus KSM cycle, especially when a change touches incomplete C++ implementations, CMake/test wiring, cognitive architecture centers, avatar protocol state, or reusable skill memory.

## Activation Conditions

Activate this skill when any of the following signals appear in the task or codebase:

| Signal | Copilot Response |
|---|---|
| The user mentions `/skill-creator(/ksm-evolve("elizaos-cpp"))`, KSM, living centers, or skill crystallization. | Treat the work as a structured evolution cycle rather than an isolated patch. |
| A module contains stubs, TypeScript-transpilation residue, brittle compatibility code, or missing C++ behavior. | Complete the canonical C++17 API with focused tests and avoid parallel replacement architectures. |
| A build or test failure spans CMake, generated subtrees, package targets, or partial-checkout assumptions. | Repair the smallest structural seam, document validation, and preserve full-checkout behavior. |
| A repair reveals a reusable future pattern. | Suggest whether it belongs in repository docs, `.github/agents`, the Manus `eliza-os` skill, or the Manus `elizaos-cpp-ksm-creator` skill. |

## Manus–Copilot Role Split

Manus and Copilot should operate as complementary parts of one loop, not as competing strategists. Manus frames the cycle, selects living centers, performs broad repository archaeology, validates the repair envelope, and decides when a discovery becomes durable skill memory. Copilot accelerates local implementation inside that frame by completing functions, tightening C++ semantics, adding focused tests, and preserving repository conventions.

| Layer | Manus Leads | Copilot Leads | Synergy Rule |
|---|---|---|---|
| Cycle framing | Target center, scope, success predicates, cross-repo context | Reads and preserves the active frame | Do not redirect the center silently. |
| Implementation | Structural repair strategy and validation envelope | Local C++17 edits, RAII/thread-safety fixes, focused refactors | Prefer canonical APIs over duplicate shims. |
| Testing | Validation tiers and pass/fail interpretation | Deterministic unit/integration tests tied to changed behavior | Never weaken assertions to hide a defect. |
| Skill memory | Decides whether a lesson is reusable and where it belongs | Suggests concise repository/Copilot notes when helpful | Keep agent history out of source comments unless it explains code intent. |
| Commit hygiene | Coherent staged diff, validation report, push/handoff | Avoid unrelated formatting, generated dependency churn, and broad rewrites | One commit should map to one KSM center. |

## 12-Step Copilot Operating Loop

When this skill is active, work through the following loop. The loop is compatible with Manus KSM cycle artifacts, but Copilot can also use it mentally during a local coding session.

| Step | Copilot Behavior |
|---|---|
| 1. Observe | Inspect the current file, nearest headers, target CMake, and existing tests before editing. |
| 2. Identify Center | Name the living center involved: characters, memory, Echobeats, endocrine state, protocol, CMake, tests, or skill memory. |
| 3. Detect Weakness | State the concrete weakness: missing behavior, data loss, deadlock, schema drift, brittle target, or untested path. |
| 4. Think | Choose the smallest structure-preserving repair that completes the existing design. |
| 5. Discover Gaps | Identify any adjacent assertions or tests needed to make the repair observable. |
| 6. Validate Before | Prefer reproducing the failure or locating the missing test before changing multiple files. |
| 7. Mutate | Implement production C++17 using repository-native types, RAII, deterministic behavior, and thread-safe locking. |
| 8. Create Framework | Add or extend focused tests for the repaired behavior rather than relying only on broad builds. |
| 9. Observe New Life | Build and run the narrowest relevant test target first. |
| 10. Observe State | Summarize changed behavior, validation, and any remaining risks. |
| 11. Create Memory | Suggest a skill/doc update only when the lesson is reusable. |
| 12. Return | Hand back the next weakest center or a clean commit-ready diff. |

## ElizaOS-C++ Living Centers

The following centers are the preferred repair anchors. Copilot should avoid scattering a single repair across unrelated centers unless the current Manus cycle explicitly calls for it.

| Center | Weakness Signals | Validation Anchor |
|---|---|---|
| Characters | Trait mutation gaps, JSON round-trip loss, placeholder helpers, manager deadlocks | `characters_test` and character package tests |
| Memory | Persistence gaps, unsafe compatibility shims, non-deterministic retrieval | Memory tests and transcript replay |
| Cognitive Cycle / Echobeats | Unwired phases, state drift, missing mode-transition coverage | Cognitive-pipeline E2E tests |
| Endocrine / Emotion | Unbounded hormone updates, mismatched expression state | Endocrine tests and avatar bridge snapshots |
| Protocol / WebSocket | Schema drift or missing backend/frontend state fields | Serialization and protocol round-trip tests |
| Build Graph / CMake | Missing optional targets, generated subtree assumptions, brittle install rules | Configure plus focused target build |
| Skill Memory | Repair insights not captured for future agents | Updated `.github/agents`, repository docs, or Manus skill files |

## Conflict-Resolution Rule

If a local completion seems to conflict with the active Manus KSM strategy, stop and summarize the conflict instead of choosing a divergent path. Typical conflicts include introducing a new subsystem when a canonical API should be completed, disabling tests instead of repairing behavior, committing partial-checkout shims into a full checkout, or making broad formatting rewrites that obscure the structural repair.

## Output Expectations

When returning a result under this skill, be concise and concrete. Name the center, files touched, tests run, and remaining risks. Prefer a small table for validation evidence. If the change is skill-worthy, explicitly say where the memory should be crystallized.
