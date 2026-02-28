#!/usr/bin/env python3
"""Run the experience benchmark suite.

Usage:
    # Direct mode (existing behavior - no LLM required):
    python run_benchmark.py
    python run_benchmark.py --experiences 2000 --queries 200 --output results.json

    # Eliza agent mode (requires OPENAI_API_KEY):
    python run_benchmark.py --mode eliza-agent
    python run_benchmark.py --mode eliza-agent --learning-cycles 20 --output results.json

Modes:
    direct:      Direct ExperienceService testing (default, no LLM)
    eliza-agent: Full Eliza agent loop (Provider -> Model -> Action -> Evaluator)
"""

import argparse
import asyncio
import os
import sys
from pathlib import Path

# Add paths
sys.path.insert(0, str(Path(__file__).parent))
sys.path.insert(0, str(Path(__file__).parent.parent.parent / "plugins" / "plugin-experience" / "python"))

from elizaos_experience_bench.runner import ExperienceBenchmarkRunner
from elizaos_experience_bench.types import BenchmarkConfig, BenchmarkMode


def _load_env_file(env_path: Path) -> None:
    """Minimal .env loader (no external dependency)."""
    if not env_path.exists():
        return
    try:
        content = env_path.read_text(encoding="utf-8")
    except Exception:
        return
    for raw_line in content.splitlines():
        line = raw_line.strip()
        if not line or line.startswith("#"):
            continue
        if "=" not in line:
            continue
        key, value = line.split("=", 1)
        key = key.strip()
        if key.startswith("export "):
            key = key[len("export "):].strip()
        value = value.strip().strip('"').strip("'")
        if not key:
            continue
        if key not in os.environ:
            os.environ[key] = value


def run_direct(args: argparse.Namespace) -> None:
    """Run the direct (non-agent) benchmark mode."""
    config = BenchmarkConfig(
        num_experiences=args.experiences,
        num_retrieval_queries=args.queries,
        num_learning_cycles=args.learning_cycles,
        seed=args.seed,
    )

    runner = ExperienceBenchmarkRunner(config)
    runner.run_and_report(output_path=args.output)


async def run_eliza_agent(args: argparse.Namespace) -> None:
    """Run the Eliza agent benchmark mode."""
    # Load .env for API keys
    repo_root = Path(__file__).resolve().parents[2]
    _load_env_file(repo_root / ".env")

    if not os.environ.get("OPENAI_API_KEY"):
        print(
            "ERROR: OPENAI_API_KEY is not set.\n"
            "The eliza-agent mode requires a real LLM.\n"
            "Add it to the repo-root .env or export it."
        )
        sys.exit(1)

    # Use cheaper models by default for benchmarking
    os.environ.setdefault("OPENAI_LARGE_MODEL", "gpt-4o-mini")
    os.environ.setdefault("OPENAI_SMALL_MODEL", "gpt-4o-mini")

    config = BenchmarkConfig(
        num_experiences=args.experiences,
        num_retrieval_queries=args.queries,
        num_learning_cycles=args.learning_cycles,
        seed=args.seed,
    )

    print("=" * 60)
    print("ElizaOS Experience Benchmark - Agent Mode")
    print("=" * 60)
    print("This tests the full Eliza canonical flow:")
    print("  EXPERIENCE_CONTEXT Provider -> Model -> RECORD/QUERY Actions -> Evaluator")
    print()

    def on_progress(phase: str, completed: int, total: int) -> None:
        pct = completed / total * 100 if total > 0 else 0
        bar_len = 30
        filled = int(bar_len * completed / total) if total > 0 else 0
        bar = "█" * filled + "░" * (bar_len - filled)
        print(f"\r  {phase}: [{bar}] {completed}/{total} ({pct:.1f}%)", end="", flush=True)
        if completed >= total:
            print()

    def get_openai_plugin_factory():  # noqa: ANN202
        from elizaos_plugin_openai import get_openai_plugin

        return get_openai_plugin()

    runner = ExperienceBenchmarkRunner(config)
    result = await runner.run_eliza_agent(
        model_plugin_factory=get_openai_plugin_factory,
        progress_callback=on_progress,
    )

    if args.output:
        import json

        report = _serialize_agent_result(result)
        with open(args.output, "w") as f:
            json.dump(report, f, indent=2, default=str)
        print(f"\n[ExperienceBench] Report written to {args.output}")


def _serialize_agent_result(result: "BenchmarkResult") -> dict:
    """Serialize agent benchmark result to JSON-friendly dict."""
    from elizaos_experience_bench.types import BenchmarkResult

    out: dict = {
        "mode": "eliza_agent",
        "total_experiences": result.total_experiences,
    }
    if result.eliza_agent:
        out["eliza_agent"] = {
            "learning_success_rate": result.eliza_agent.learning_success_rate,
            "total_experiences_recorded": result.eliza_agent.total_experiences_recorded,
            "total_experiences_in_service": result.eliza_agent.total_experiences_in_service,
            "avg_learning_latency_ms": result.eliza_agent.avg_learning_latency_ms,
            "agent_recall_rate": result.eliza_agent.agent_recall_rate,
            "agent_keyword_incorporation_rate": result.eliza_agent.agent_keyword_incorporation_rate,
            "avg_retrieval_latency_ms": result.eliza_agent.avg_retrieval_latency_ms,
            "direct_recall_rate": result.eliza_agent.direct_recall_rate,
            "direct_mrr": result.eliza_agent.direct_mrr,
        }
    if result.retrieval:
        out["direct_retrieval"] = {
            "precision_at_k": result.retrieval.precision_at_k,
            "recall_at_k": result.retrieval.recall_at_k,
            "mean_reciprocal_rank": result.retrieval.mean_reciprocal_rank,
            "hit_rate_at_k": result.retrieval.hit_rate_at_k,
        }
    return out


def main() -> None:
    """Main entry point."""
    parser = argparse.ArgumentParser(description="Experience Plugin Benchmark")
    parser.add_argument(
        "--mode",
        choices=["direct", "eliza-agent"],
        default="direct",
        help=(
            "Benchmark mode: 'direct' tests ExperienceService directly (default), "
            "'eliza-agent' tests through a real Eliza agent with LLM"
        ),
    )
    parser.add_argument("--experiences", type=int, default=1000, help="Number of synthetic experiences")
    parser.add_argument("--queries", type=int, default=100, help="Number of retrieval queries")
    parser.add_argument("--learning-cycles", type=int, default=20, help="Number of learning cycle scenarios")
    parser.add_argument("--seed", type=int, default=42, help="Random seed")
    parser.add_argument("--output", type=str, default=None, help="Output JSON path")
    args = parser.parse_args()

    if args.mode == "direct":
        run_direct(args)
    elif args.mode == "eliza-agent":
        asyncio.run(run_eliza_agent(args))
    else:
        parser.error(f"Unknown mode: {args.mode}")


if __name__ == "__main__":
    main()
