# SDK Design Notes

## Architecture

The system has four layers:

1. **BotActions (porcelain)**
   - High level ergonomic functions skipTutorial, chopTree, attackNpc, etc.
   - Returns when intended action is complete
   - Legible failure messages
   - Pathfinding orchestration
   - encodes domain knowledge, actively constructed and maintained as this project evolves
   - *Can be updated without any serverside changes*

2. **BotSDK (plumbing)**
   - Returns when server acknowledges it's been sent
   - State queries, waitForCondition
   - Lower domain knowledge, similar to actual game client
   - Should evolve more slowly

3. **Gateway**
   - Routing, session management, multi-client broadcast

4. **Bot Client (executor)**
   - Executes primitive actions (mapping directly to game packets)
   - Sends raw state

### Design Principle

**Bot client should only have pure primitives** - actions that map 1:1 to game protocol packets.
All "smart" behavior (multi-step sequences, decision-making) lives in BotActions.

### Plumbing (`index.ts` - BotSDK)
- Low-level API where actions resolve on game acknowledgment (fast)
- Includes basic state queries and local pathfinding
- Protocol changes require gateway/client updates, should change more slowly 

```typescript
await sdk.sendInteractLoc(tree.x, tree.z, tree.id, 1);
await sdk.sendWalk(x, z, running);
await sdk.sendClickComponent(componentId);            // IF_BUTTON packet
await sdk.sendClickComponentWithOption(componentId, 1); // INV_BUTTON packet
```

### Porcelain (`actions.ts` - BotActions)
- High-level, domain-aware API that wraps plumbing
- Actions resolve when the **effect is complete** (more ergonomic to develop against)
- Tries to encode domain knowledge and handles edge cases

```typescript
const result = await bot.chopTree();       // Waits for logs in inventory
if(!result.success) {
    console.log(result.message);
}
await bot.walkTo(x, z);     // Pathfinding + arrival confirmation
await bot.skipTutorial();   // Dialog navigation + NPC interaction
await bot.smithAtAnvil('dagger'); // Full smithing workflow
```

## Interface Component Actions

Three SDK methods for clicking interface components:

| Action | Packet | Use Case |
|--------|--------|----------|
| `sendClickComponent(id)` | IF_BUTTON | Simple buttons, spellcasting |
| `sendClickComponentWithOption(id, opt)` | INV_BUTTON | Components with options (smithing, crafting, make-x) |
| `sendClickInterfaceOption(index)` | IF_BUTTON | Convenience - looks up componentId from `state.interface.options[index]` |

## Known Limitations / TODOs

### `acceptCharacterDesign`
Currently uses hidden client state (`designGender`, `designKits`, `designColours`).
The SDK cannot set design values before accepting.

**Future improvement**: Parameterize as `acceptCharacterDesign(gender, kits[7], colours[5])`
so the action is fully self-contained.

## Key Learnings

### 1. Game Messages Persist in Buffer
Old messages like "You can't light a fire here" persist. Filter by tick:

```typescript
const startTick = this.sdk.getState()?.tick || 0;
// Only check messages where msg.tick > startTick
```

### 2. Level-Up Dialogs Are Multi-Page
Keep clicking every few ticks while open:

```typescript
if (state.dialog.isOpen && (state.tick - lastClick) >= 3) {
    this.sdk.sendClickDialog(0).catch(() => {});
}
```

### 3. Choose the Right Success Signal

| Action | Reliable Signal |
|--------|-----------------|
| Firemaking | XP gain |
| Woodcutting | Logs in inventory OR tree disappears |
| Pickup | Item in inventory |
| Walking | Player position matches destination |
| Shop Buy | Item appears in inventory |
| Equip | Item leaves inventory |

## Available BotActions Methods

### Movement & Interaction
| Method | Description |
|--------|-------------|
| `walkTo(x, z, tolerance?)` | Pathfinding + arrival |
| `talkTo(target)` | Talk to NPC, opens dialog |
| `openDoor(target?)` | Opens a door |
| `navigateDialog(choices)` | Click through dialog options |
| `skipTutorial()` | Navigate tutorial dialogs/NPCs |

### Skills & Resources
| Method | Description |
|--------|-------------|
| `chopTree(target?)` | Chops tree, waits for logs |
| `burnLogs(target?)` | Burns logs with tinderbox |
| `pickupItem(target)` | Picks up ground item |
| `fletchLogs(product?)` | Fletches logs into items |
| `craftLeather(product?)` | Crafts leather items |
| `smithAtAnvil(product?)` | Smiths bars into items |

### Shop & Bank
| Method | Description |
|--------|-------------|
| `openShop(target?)` | Opens shop interface |
| `buyFromShop(target, amount?)` | Buys from shop |
| `sellToShop(target, amount?)` | Sells to shop |
| `closeShop()` | Closes shop interface |
| `openBank()` | Opens bank interface |
| `depositItem(target, amount?)` | Deposits to bank |
| `withdrawItem(slot, amount?)` | Withdraws from bank |
| `closeBank()` | Closes bank interface |

### Equipment & Combat
| Method | Description |
|--------|-------------|
| `equipItem(target)` | Equips from inventory |
| `unequipItem(target)` | Unequips item |
| `eatFood(target)` | Eats food |
| `attackNpc(target, timeout?)` | Attacks NPC |
| `castSpellOnNpc(target, spell, timeout?)` | Casts spell on NPC |

### Helpers
| Method | Description |
|--------|-------------|
| `dismissBlockingUI()` | Closes dialogs/modals |
| `waitForSkillLevel(skill, level)` | Waits for skill level |
| `waitForInventoryItem(pattern)` | Waits for item |
| `waitForDialogClose()` | Waits for dialog to close |
| `waitForIdle()` | Waits for player to be idle |

## Files

| File | Purpose |
|------|---------|
| `sdk/index.ts` | BotSDK - low-level WebSocket API |
| `sdk/actions.ts` | BotActions - high-level domain actions |
| `sdk/types.ts` | Type definitions |
| `sdk/pathfinding.ts` | Pathfinding utilities |
| `gateway/gateway.ts` | Gateway - routing, sessions |
| `webclient/src/bot/BotSDK.ts` | Bot Client - pure executor |
