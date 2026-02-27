"""EVM_CONTEXT provider for injecting discovery state and contract catalog."""

from __future__ import annotations

import logging
from typing import TYPE_CHECKING

from elizaos.types import Provider, ProviderResult

if TYPE_CHECKING:
    from elizaos.types import IAgentRuntime, Memory, State

logger = logging.getLogger(__name__)


async def _get_evm_context(
    runtime: IAgentRuntime, _message: Memory, _state: State
) -> ProviderResult:
    """
    Inject EVM exploration context into the agent's state.

    Provides:
    - Current reward / discovery stats
    - Deployed contracts with addresses
    - Undiscovered selectors grouped by contract
    - Chain / network configuration
    - Strategy hints for maximising reward
    """
    strategy = runtime.get_setting("EXPLORATION_STRATEGY")
    env = runtime.get_setting("ANVIL_ENV")

    if strategy is None and env is None:
        return ProviderResult(text="", values={}, data={})

    parts: list[str] = []
    parts.append("## EVM Exploration Context")
    parts.append("")

    # ---- Reward & progress ----
    if strategy is not None:
        state = strategy.state
        parts.append(f"**Reward so far:** {state.total_reward}")
        parts.append(f"**Discovered pairs:** {len(state.discovered)}")
        parts.append(f"**Remaining unique pairs:** {state.remaining_unique}")
        messages_left = strategy.max_messages - state.current_step
        parts.append(f"**Messages remaining:** {messages_left}")
        parts.append("")

        # ---- Deployed contracts ----
        if state.deployed_contracts:
            parts.append("### Deployed Contracts (call functions on these)")
            for addr, ctype in state.deployed_contracts.items():
                parts.append(f"  - `{addr}` ({ctype})")
            parts.append("")

        # ---- Undiscovered selectors ----
        undiscovered = state.get_undiscovered_by_contract()
        if undiscovered:
            parts.append("### Undiscovered Function Selectors to Target")
            for contract_key, missing_fns in undiscovered.items():
                # Cap at 10 per contract to keep context concise
                display_fns = missing_fns[:10]
                if len(missing_fns) > 10:
                    display_fns.append(f"... and {len(missing_fns) - 10} more")
                parts.append(f"  **{contract_key}:** {', '.join(display_fns)}")
            parts.append("")

    # ---- Chain / network info ----
    if env is not None:
        parts.append("### Chain Configuration")
        parts.append(f"  - **RPC URL:** {env.rpc_url}")
        parts.append(f"  - **Chain ID:** {env.chain_id}")
        parts.append(f"  - **Agent address:** {env.agent_address}")
        parts.append(f"  - **Total reward:** {env.total_reward}")
        parts.append("")

    # ---- Already discovered (so LLM avoids repeating) ----
    if strategy is not None and state.discovered:
        parts.append("### Already Discovered (DO NOT repeat these)")
        by_addr: dict[str, list[str]] = {}
        for addr, sel in state.discovered:
            by_addr.setdefault(addr[:12] + "...", []).append(sel)
        for addr_key, sels in list(by_addr.items())[:15]:
            parts.append(f"  {addr_key}: {', '.join(sels[:8])}")
        parts.append("")

    # ---- Concrete next steps (actionable, not vague) ----
    parts.append("### Concrete Next Steps to Earn Rewards")
    parts.append("Pick ONE of these approaches per step:")
    parts.append("")
    parts.append("1. **Call view functions on deployed ERC20/ERC721 (easy wins):**")
    parts.append("   - ERC20: name()=0x06fdde03, symbol()=0x95d89b41, decimals()=0x313ce567")
    parts.append("   - ERC20: totalSupply()=0x18160ddd, balanceOf(addr)=0x70a08231")
    parts.append("   - ERC721: ownerOf(uint256)=0x6352211e, tokenURI(uint256)=0xc87b56dd")
    parts.append("   - ERC721: balanceOf(addr)=0x70a08231, getApproved(uint256)=0x081812fc")
    parts.append("")
    parts.append("2. **Call precompiles with DIFFERENT first-4-bytes than already used:**")
    parts.append("   - Send data starting with 0xAAAABBBB to precompile 0x04 (identity)")
    parts.append("   - Use different padding for ecAdd (0x06), ecMul (0x07)")
    parts.append("")
    parts.append("3. **Deploy a NEW contract type** (e.g., ERC1155, minimal proxy, create2)")
    parts.append("")
    parts.append("CRITICAL: Check 'Already Discovered' above. Do NOT repeat those pairs.")
    parts.append("Keep code SHORT (<100 lines). Do NOT inline large bytecodes.")

    text = "\n".join(parts)

    # ---- Build structured data ----
    values: dict[str, str] = {}
    data: dict[str, str | int | dict[str, str]] = {}

    if strategy is not None:
        values["totalReward"] = str(strategy.state.total_reward)
        values["discoveredCount"] = str(len(strategy.state.discovered))
        values["remainingUnique"] = str(strategy.state.remaining_unique)
        data["totalReward"] = strategy.state.total_reward
        data["discoveredCount"] = len(strategy.state.discovered)
        data["deployedContracts"] = dict(strategy.state.deployed_contracts)

    if env is not None:
        values["rpcUrl"] = env.rpc_url
        values["chainId"] = str(env.chain_id)
        values["agentAddress"] = env.agent_address

    return ProviderResult(text=text, values=values, data=data)


evm_context_provider = Provider(
    name="EVM_CONTEXT",
    description=(
        "Provides EVM exploration context: discovery state, deployed contracts, "
        "undiscovered selectors, chain configuration, and strategy hints."
    ),
    position=50,  # Before RECENT_MESSAGES
    private=False,
    get=_get_evm_context,
)
