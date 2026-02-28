"""
Canonical ElizaOS Agent for HyperliquidBench.

This agent wraps the HyperliquidBench Rust toolchain behind an ElizaOS runtime
with ``handle_message()`` driven actions and providers.  The flow is:

1. Load scenario(s) from dataset JSON/JSONL files
2. Send scenario description as a message to the Eliza runtime
3. The agent generates a plan via the GENERATE_PLAN action
4. The plan is executed by shelling out to ``hl-runner`` (EXECUTE_PLAN action)
5. ``hl-evaluator`` scores the run
6. Results are fed back for optional follow-up iterations
"""

from __future__ import annotations

import asyncio
import json
import logging
import os
import uuid
from datetime import datetime
from pathlib import Path
from typing import TYPE_CHECKING

from elizaos import AgentRuntime
from elizaos.types import Plugin
from elizaos.types.agent import Character
from elizaos.types.memory import Memory
from elizaos.types.primitives import Content, as_uuid, string_to_uuid

from .plugin import hl_bench_plugin
from .types import (
    BenchmarkResult,
    EvaluatorResult,
    HLBenchConfig,
    Plan,
    RunnerResult,
    ScenarioKind,
    TradingScenario,
)

if TYPE_CHECKING:
    pass

logger = logging.getLogger(__name__)

# ── Message handler template ────────────────────────────────────────

HL_MESSAGE_TEMPLATE = """<task>Generate dialog and actions for the character {{agentName}}.</task>

<providers>
{{providers}}
</providers>

<instructions>
You are a professional crypto trader on Hyperliquid DEX.  Your job is to
generate structured trading plans in JSON format that will be executed by
the HyperliquidBench Rust toolchain.

CRITICAL: You MUST output a valid JSON plan conforming to the schema provided
by the HL_CONTEXT provider.  Do NOT wrap the JSON in markdown fences or add
commentary outside the JSON.

Consider position sizing, leverage, risk management, and coverage across
different action types (perp orders, cancels, transfers, leverage changes)
to maximise the benchmark coverage score.

Available actions (require parameters):
- GENERATE_PLAN: Produces a validated JSON plan from your response text
- EXECUTE_PLAN: Runs the plan through the Rust hl-runner and evaluator

ACTION ORDERING:
1. First, output the JSON plan in your response text.
2. Use GENERATE_PLAN to validate and store it.
3. Use EXECUTE_PLAN to execute it and get the score.

When you receive execution results, analyse the score and, if asked,
generate an improved plan for the next iteration.
</instructions>

<output>
Respond using XML format like this:
<response>
  <thought>Brief analysis of the scenario and strategy</thought>
  <actions>REPLY,GENERATE_PLAN,EXECUTE_PLAN</actions>
  <providers>HL_CONTEXT</providers>
  <text>
{"steps":[...your plan JSON here...]}
  </text>
</response>
</output>"""


def _get_openai_plugin() -> Plugin:
    """Get the OpenAI plugin for model handlers."""
    try:
        from elizaos_plugin_openai import get_openai_plugin

        return get_openai_plugin()
    except ImportError:
        raise RuntimeError(
            "elizaos_plugin_openai not found. Please install it:\n"
            "  pip install elizaos-plugin-openai\n"
            "Or set PYTHONPATH to include the plugin directory."
        )


# ── Scenario loading ────────────────────────────────────────────────

def load_scenarios_from_tasks(
    bench_root: Path,
    task_files: list[str] | None = None,
) -> list[TradingScenario]:
    """
    Load trading scenarios from the ``dataset/tasks/`` directory.

    Each JSONL line or JSON file becomes a ``TradingScenario`` with a
    ``plan_spec`` pointing to the original file (so the Rust runner can
    also load it directly if needed).
    """
    tasks_dir = bench_root / "dataset" / "tasks"
    scenarios: list[TradingScenario] = []

    if task_files:
        files = [tasks_dir / f for f in task_files]
    else:
        files = sorted(tasks_dir.glob("*.jsonl")) + sorted(tasks_dir.glob("*.json"))

    for filepath in files:
        if not filepath.exists():
            logger.warning("Task file not found: %s", filepath)
            continue

        if filepath.suffix == ".jsonl":
            with open(filepath) as fh:
                for line_no, line in enumerate(fh, start=1):
                    line = line.strip()
                    if not line:
                        continue
                    plan_data = json.loads(line)
                    steps = plan_data.get("steps", [])
                    # Infer allowed coins from the plan
                    coins = _extract_coins_from_plan(steps)
                    spec = f"{filepath}:{line_no}"
                    scenarios.append(
                        TradingScenario(
                            scenario_id=f"{filepath.stem}_line{line_no}",
                            kind=ScenarioKind.COVERAGE,
                            description=(
                                f"Coverage scenario from {filepath.name} line {line_no}: "
                                f"execute a {len(steps)}-step plan covering "
                                f"{_describe_step_kinds(steps)}"
                            ),
                            allowed_coins=coins if coins else ["ETH", "BTC"],
                            max_steps=max(len(steps) + 2, 7),
                            plan_spec=spec,
                        )
                    )
        else:
            plan_data = json.loads(filepath.read_text())
            steps = plan_data.get("steps", [])
            coins = _extract_coins_from_plan(steps)
            scenarios.append(
                TradingScenario(
                    scenario_id=filepath.stem,
                    kind=ScenarioKind.COVERAGE,
                    description=(
                        f"Coverage scenario from {filepath.name}: "
                        f"execute a {len(steps)}-step plan covering "
                        f"{_describe_step_kinds(steps)}"
                    ),
                    allowed_coins=coins if coins else ["ETH", "BTC"],
                    max_steps=max(len(steps) + 2, 7),
                    plan_spec=str(filepath),
                )
            )

    return scenarios


def make_coverage_scenario(
    allowed_coins: list[str] | None = None,
    max_steps: int = 5,
    builder_code: str | None = None,
) -> TradingScenario:
    """Create a free-form coverage scenario (agent decides the plan)."""
    coins = allowed_coins or ["ETH", "BTC"]
    return TradingScenario(
        scenario_id=f"coverage_{datetime.now().strftime('%Y%m%d_%H%M%S')}",
        kind=ScenarioKind.COVERAGE,
        description=(
            f"Generate a coverage plan with at most {max_steps} steps that touches "
            f"distinct venue actions for scoring coverage.  Use coins: {', '.join(coins)}.  "
            f"Maximise the number of unique signatures across perp orders (ALO, GTC, IOC), "
            f"cancels (last, all), USD class transfers, and leverage changes."
        ),
        allowed_coins=coins,
        max_steps=max_steps,
        builder_code=builder_code,
    )


def _extract_coins_from_plan(steps: list[dict[str, object]]) -> list[str]:
    """Extract unique coin symbols from a plan's steps."""
    coins: set[str] = set()
    for step in steps:
        if "perp_orders" in step:
            orders_data = step["perp_orders"]
            if isinstance(orders_data, dict):
                for order in orders_data.get("orders", []):
                    if isinstance(order, dict) and "coin" in order:
                        coins.add(str(order["coin"]).upper())
        for key in ("cancel_last", "cancel_oids", "cancel_all", "set_leverage"):
            if key in step:
                payload = step[key]
                if isinstance(payload, dict) and "coin" in payload:
                    coin_val = payload["coin"]
                    if coin_val:
                        coins.add(str(coin_val).upper())
    return sorted(coins) if coins else []


def _describe_step_kinds(steps: list[dict[str, object]]) -> str:
    """Return a human-readable summary of step kinds in a plan."""
    kinds: list[str] = []
    for step in steps:
        if isinstance(step, dict):
            kinds.extend(step.keys())
    return ", ".join(kinds) if kinds else "unknown steps"


# ── Agent ───────────────────────────────────────────────────────────

class ElizaHyperliquidAgent:
    """
    Canonical ElizaOS agent for HyperliquidBench.

    This agent:
    - Creates an ``AgentRuntime`` with a trader character
    - Registers the HyperliquidBench plugin (GENERATE_PLAN, EXECUTE_PLAN, HL_CONTEXT)
    - Registers the OpenAI model provider plugin
    - Uses ``message_service.handle_message()`` for canonical message processing
    - Iterates: scenario → plan generation → execution → scoring → feedback
    """

    def __init__(
        self,
        config: HLBenchConfig | None = None,
        model_name: str | None = None,
        verbose: bool = False,
    ) -> None:
        self._config = config or HLBenchConfig()
        if model_name:
            self._config.model_name = model_name
        self._verbose = verbose or self._config.verbose
        self._runtime: AgentRuntime | None = None

    async def _initialize_runtime(self) -> AgentRuntime:
        """Initialise the full ElizaOS runtime with character and plugins."""
        os.environ.setdefault("OPENAI_SMALL_MODEL", self._config.model_name)
        os.environ.setdefault("OPENAI_LARGE_MODEL", self._config.model_name)

        character = Character(
            name="HyperliquidTrader",
            username="hl_trader",
            bio=(
                "A Hyperliquid trading agent that generates and validates "
                "trading plans for the HyperliquidBench benchmark."
            ),
            system=(
                "You are a professional crypto trader on Hyperliquid DEX. "
                "Generate structured trading plans in JSON format. Consider "
                "position sizing, leverage, and risk management. Maximise "
                "coverage by using diverse action types."
            ),
            settings={
                "extra": {
                    "CHECK_SHOULD_RESPOND": False,
                    "ACTION_PLANNING": True,
                },
            },
            templates={
                "messageHandlerTemplate": HL_MESSAGE_TEMPLATE,
            },
        )

        openai_plugin = _get_openai_plugin()

        runtime = AgentRuntime(
            character=character,
            plugins=[
                openai_plugin,
                hl_bench_plugin,
            ],
            disable_basic_capabilities=False,
            enable_extended_capabilities=False,
            check_should_respond=False,
            action_planning=True,
            log_level="DEBUG" if self._verbose else "INFO",
        )

        await runtime.initialize()

        logger.info(
            "ElizaOS runtime initialised with %d actions, %d providers",
            len(runtime.actions),
            len(runtime.providers),
        )

        return runtime

    def _inject_scenario_context(self, scenario: TradingScenario) -> None:
        """Set runtime settings consumed by HL_CONTEXT provider and actions."""
        if self._runtime is None:
            return

        self._runtime.set_setting("CURRENT_SCENARIO", scenario)
        self._runtime.set_setting("BENCH_ROOT", self._config.bench_root)
        self._runtime.set_setting("BENCH_CONFIG", self._config)
        self._runtime.set_setting("CURRENT_PLAN_JSON", None)
        self._runtime.set_setting("CURRENT_PLAN_DICT", None)
        self._runtime.set_setting("LAST_RESULT_JSON", None)
        self._runtime.set_setting("PLAN_EXECUTED", False)

    async def solve_scenario(
        self, scenario: TradingScenario
    ) -> BenchmarkResult:
        """
        Run a single benchmark scenario through the Eliza agent.

        1. Initialise runtime (once)
        2. Inject scenario context
        3. Send scenario as message → agent generates plan → plan executed
        4. Iterate if configured for multiple attempts
        """
        if self._runtime is None:
            self._runtime = await self._initialize_runtime()

        self._inject_scenario_context(scenario)

        room_id = string_to_uuid(f"hl-bench-{scenario.scenario_id}")
        user_id = string_to_uuid("benchmark-harness")

        action_callback_results: list[Content] = []
        last_feedback = ""

        async def action_callback(content: Content) -> list[Memory]:
            action_callback_results.append(content)
            return []

        best_result: dict[str, object] | None = None

        for iteration in range(self._config.max_iterations):
            logger.info(
                "Scenario %s – iteration %d/%d",
                scenario.scenario_id,
                iteration + 1,
                self._config.max_iterations,
            )

            try:
                if iteration == 0:
                    message_text = (
                        f"Please generate and execute a trading plan for this scenario:\n\n"
                        f"{scenario.description}\n\n"
                        f"Allowed coins: {', '.join(scenario.allowed_coins)}\n"
                        f"Max steps: {scenario.max_steps}\n"
                    )
                    if scenario.builder_code:
                        message_text += f"Builder code: {scenario.builder_code}\n"
                    message_text += (
                        "\nUse GENERATE_PLAN to validate your plan, then EXECUTE_PLAN "
                        "to run it and get the score."
                    )
                else:
                    message_text = (
                        last_feedback
                        if last_feedback
                        else "Continue: analyse the previous result and generate an improved plan."
                    )

                message = Memory(
                    id=as_uuid(str(uuid.uuid4())),
                    entity_id=user_id,
                    room_id=room_id,
                    content=Content(text=message_text),
                    created_at=int(datetime.now().timestamp() * 1000),
                )

                result = await self._runtime.message_service.handle_message(
                    self._runtime,
                    message,
                    action_callback,
                )

                # Collect feedback for next iteration
                feedback_parts: list[str] = []
                for c in action_callback_results:
                    if c.text:
                        feedback_parts.append(c.text)
                last_feedback = "\n\n".join(feedback_parts).strip()
                action_callback_results.clear()

                if self._verbose:
                    logger.debug("Message handled, did_respond=%s", result.did_respond)

                # Check if plan was executed
                plan_executed = self._runtime.get_setting("PLAN_EXECUTED")
                if plan_executed:
                    last_result_str: str | None = self._runtime.get_setting("LAST_RESULT_JSON")
                    if last_result_str:
                        best_result = json.loads(last_result_str)
                    # Reset for next iteration
                    self._runtime.set_setting("PLAN_EXECUTED", False)
                    self._runtime.set_setting("CURRENT_PLAN_JSON", None)

            except Exception as exc:
                logger.error("Error in iteration %d: %s", iteration + 1, exc)
                last_feedback = f"Error in previous iteration: {exc}.  Please try again."

        # Build BenchmarkResult from the best result
        plan_dict = self._runtime.get_setting("CURRENT_PLAN_DICT") or {"steps": []}
        plan = Plan(steps=[])  # simplified – we store the raw dict

        runner_result = RunnerResult(
            success=False, out_dir="", run_meta_path="", per_action_path="",
            stdout="", stderr="", exit_code=-1,
        )
        evaluator_result: EvaluatorResult | None = None
        error_message: str | None = None

        if best_result:
            runner_data = best_result.get("runner", {})
            if isinstance(runner_data, dict):
                runner_result = RunnerResult(
                    success=bool(runner_data.get("success", False)),
                    out_dir=str(runner_data.get("outDir", "")),
                    run_meta_path=str(Path(str(runner_data.get("outDir", ""))) / "run_meta.json"),
                    per_action_path=str(Path(str(runner_data.get("outDir", ""))) / "per_action.jsonl"),
                    stdout="",
                    stderr=str(runner_data.get("stderr", "")),
                    exit_code=int(runner_data.get("exitCode", -1)),
                )
            eval_data = best_result.get("evaluator")
            if isinstance(eval_data, dict):
                sigs = eval_data.get("uniqueSignatures", [])
                evaluator_result = EvaluatorResult(
                    success=bool(eval_data.get("success", False)),
                    final_score=float(eval_data.get("finalScore", 0.0)),
                    base=float(eval_data.get("base", 0.0)),
                    bonus=float(eval_data.get("bonus", 0.0)),
                    penalty=float(eval_data.get("penalty", 0.0)),
                    unique_signatures=list(sigs) if isinstance(sigs, list) else [],
                    eval_score_path=str(Path(runner_result.out_dir) / "eval_score.json"),
                    stdout="",
                    stderr="",
                    exit_code=int(eval_data.get("exitCode", -1)),
                )
        else:
            error_message = "No plan was successfully executed"

        return BenchmarkResult(
            scenario_id=scenario.scenario_id,
            plan=plan,
            runner=runner_result,
            evaluator=evaluator_result,
            error_message=error_message,
        )

    async def run_benchmark(
        self,
        scenarios: list[TradingScenario] | None = None,
    ) -> list[BenchmarkResult]:
        """
        Run the full benchmark across multiple scenarios.

        If no scenarios are provided, loads them from ``dataset/tasks/``.
        """
        if scenarios is None:
            scenarios = load_scenarios_from_tasks(self._config.bench_root)

        if not scenarios:
            # Fall back to a single free-form coverage scenario
            scenarios = [make_coverage_scenario()]

        results: list[BenchmarkResult] = []
        for scenario in scenarios:
            logger.info("━━━ Running scenario: %s ━━━", scenario.scenario_id)
            result = await self.solve_scenario(scenario)
            results.append(result)

            if result.evaluator:
                logger.info(
                    "  Score: %.3f  (base=%.1f bonus=%.1f penalty=%.1f)",
                    result.evaluator.final_score,
                    result.evaluator.base,
                    result.evaluator.bonus,
                    result.evaluator.penalty,
                )
            elif result.error_message:
                logger.warning("  Error: %s", result.error_message)

        return results

    async def cleanup(self) -> None:
        """Clean up runtime resources."""
        if self._runtime is not None:
            await self._runtime.stop()
            self._runtime = None
