"""
HL_CONTEXT provider – injects the current trading scenario, account state,
and available operations into the agent's context window.
"""

from __future__ import annotations

import json
from pathlib import Path
from typing import TYPE_CHECKING

from elizaos.types import Provider, ProviderResult

if TYPE_CHECKING:
    from elizaos.types import IAgentRuntime, Memory, State

# JSON plan schema (mirrors crates/hl-runner/src/llm/prompts.rs)
_PLAN_SCHEMA = """\
{
  "steps": [
    {"perp_orders": {"orders": [{"coin": "ETH", "side": "buy"|"sell", "tif": "GTC"|"ALO"|"IOC", "sz": number, "reduceOnly": bool, "builderCode": string, "px": number|string, "trigger": {"kind": "none"}}], "builderCode": string}},
    {"cancel_last": {"coin": string}},
    {"cancel_oids": {"coin": string, "oids": [number]}},
    {"cancel_all":  {"coin": string}},
    {"usd_class_transfer": {"toPerp": bool, "usdc": number}},
    {"set_leverage": {"coin": string, "leverage": number, "cross": bool}},
    {"sleep_ms": {"duration_ms": number}}
  ]
}"""

_PLAN_RULES = """\
- Use only the allowed coins.
- Sizes must be positive and reasonably small (e.g., 0.001 to 1).
- Keep leverage between 1 and 20.
- "trigger.kind" must always be "none".
- Prices can be absolute numbers or "mid±X%" strings (e.g., "mid-0.5%", "mid+1.0%").
- Return compact JSON without comments or markdown fences.
- Total steps must be <= the provided max."""


def _load_domains_summary(bench_root: Path) -> str:
    """Read domains-hl.yaml and return a human-readable summary."""
    domains_path = bench_root / "dataset" / "domains-hl.yaml"
    if not domains_path.exists():
        return "(domains file not found)"
    try:
        import yaml  # type: ignore[import-untyped]

        with open(domains_path) as fh:
            data = yaml.safe_load(fh)
        parts: list[str] = []
        domains = data.get("domains", {})
        for name, info in domains.items():
            weight = info.get("weight", 1.0)
            patterns = ", ".join(info.get("allow", []))
            parts.append(f"  {name} (weight={weight}): {patterns}")
        return "\n".join(parts) if parts else "(no domains)"
    except Exception:
        # Fallback: just read raw text
        return domains_path.read_text()[:500]


async def _get_hl_context(
    runtime: IAgentRuntime, _message: Memory, _state: State
) -> ProviderResult:
    """
    Build the Hyperliquid trading context for the current scenario.

    Reads ``CURRENT_SCENARIO``, ``BENCH_ROOT``, and ``LAST_RESULT`` from
    runtime settings.
    """
    from benchmarks.HyperliquidBench.types import TradingScenario

    scenario: TradingScenario | None = runtime.get_setting("CURRENT_SCENARIO")
    bench_root: Path | None = runtime.get_setting("BENCH_ROOT")
    last_result_json: str | None = runtime.get_setting("LAST_RESULT_JSON")

    if scenario is None:
        return ProviderResult(text="", values={}, data={})

    if bench_root is None:
        bench_root = Path(__file__).resolve().parents[2]

    parts: list[str] = []
    parts.append("## Hyperliquid Trading Context")
    parts.append("")
    parts.append(f"**Scenario:** {scenario.description}")
    parts.append(f"**Kind:** {scenario.kind.value}")
    parts.append(f"**Allowed coins:** {', '.join(scenario.allowed_coins)}")
    parts.append(f"**Max steps:** {scenario.max_steps}")
    if scenario.builder_code:
        parts.append(f"**Builder code:** {scenario.builder_code}")
    parts.append("")

    # Domain information
    parts.append("### Scoring Domains")
    parts.append(_load_domains_summary(bench_root))
    parts.append("")

    # Plan schema
    parts.append("### Plan JSON Schema")
    parts.append("```")
    parts.append(_PLAN_SCHEMA)
    parts.append("```")
    parts.append("")
    parts.append("### Rules")
    parts.append(_PLAN_RULES)
    parts.append("")

    # Available actions
    parts.append("### Available Actions")
    parts.append("- **GENERATE_PLAN**: Generate a trading plan as JSON")
    parts.append("- **EXECUTE_PLAN**: Execute a plan via the Rust hl-runner binary")
    parts.append("")

    # Previous result feedback
    if last_result_json:
        parts.append("### Previous Execution Result")
        parts.append(f"```json\n{last_result_json}\n```")
        parts.append("")

    text = "\n".join(parts)

    values: dict[str, str | int | list[str]] = {
        "scenarioId": scenario.scenario_id,
        "scenarioKind": scenario.kind.value,
        "allowedCoins": scenario.allowed_coins,
        "maxSteps": scenario.max_steps,
    }
    if scenario.builder_code:
        values["builderCode"] = scenario.builder_code

    data: dict[str, object] = {
        "scenario": {
            "id": scenario.scenario_id,
            "kind": scenario.kind.value,
            "description": scenario.description,
            "allowedCoins": scenario.allowed_coins,
            "maxSteps": scenario.max_steps,
            "builderCode": scenario.builder_code,
        },
        "planSchema": _PLAN_SCHEMA,
        "planRules": _PLAN_RULES,
    }
    if last_result_json:
        data["lastResult"] = json.loads(last_result_json)

    return ProviderResult(text=text, values=values, data=data)


hl_context_provider = Provider(
    name="HL_CONTEXT",
    description=(
        "Provides the current Hyperliquid trading scenario, plan schema, "
        "scoring domains, and previous execution results"
    ),
    position=50,
    private=False,
    get=_get_hl_context,
)
