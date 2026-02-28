import { type IAgentRuntime, type Service, logger } from "@elizaos/core";
import { BotSDK } from "../../sdk/index";
import { BotActions } from "../../sdk/actions";
import type { BotState, ActionResult } from "../types";
import { botManager } from "./BotManager";

export class RsSdkService implements Service {
  static serviceType = "rs-sdk-service" as const;
  name = "rs-sdk-service";
  runtime: IAgentRuntime | null = null;
  private botConnections: Map<string, { bot: BotActions; sdk: BotSDK }> = new Map();
  private agentIdToBotName: Map<string, string> = new Map();

  /**
   * Static factory method required by Eliza's service system
   */
  static async start(runtime: IAgentRuntime): Promise<RsSdkService> {
    const service = new RsSdkService();
    await service.initialize(runtime);
    return service;
  }

  async initialize(runtime: IAgentRuntime): Promise<void> {
    this.runtime = runtime;
    // Extract bot name from settings
    const botName = runtime.getSetting("RS_SDK_BOT_NAME") as string;
    if (botName) {
      this.agentIdToBotName.set(runtime.agentId, botName);
      
      // Automatically connect to the bot during initialization
      logger.info({ botName, agentId: runtime.agentId }, "RS-SDK service connecting to bot...");
      try {
        const connection = await this.getBotConnection(botName);
        if (connection) {
          logger.info({ botName, agentId: runtime.agentId }, "RS-SDK service connected to bot successfully");
        } else {
          logger.warn({ botName, agentId: runtime.agentId }, "RS-SDK service failed to connect to bot during init - will retry on first action");
        }
      } catch (error) {
        logger.warn({ botName, agentId: runtime.agentId, error }, "RS-SDK service failed to connect to bot during init - will retry on first action");
      }
    } else {
      logger.warn({ agentId: runtime.agentId }, "RS-SDK service initialized without RS_SDK_BOT_NAME - bot name must be set via setBotNameForAgent() before actions can execute");
    }
    logger.info("RS-SDK service initialized", { agentId: runtime.agentId, botName });
  }

  setBotNameForAgent(agentId: string, botName: string): void {
    this.agentIdToBotName.set(agentId, botName);
  }

  async getBotConnection(botName: string): Promise<{ bot: BotActions; sdk: BotSDK } | null> {
    if (!botName || !botName.trim()) {
      logger.error({ botName }, "Invalid bot name provided");
      return null;
    }

    // Check if we already have a connection and verify it's still connected
    const existing = this.botConnections.get(botName);
    if (existing && existing.sdk.isConnected()) {
      return existing;
    }
    // Remove stale connection
    if (existing) {
      this.botConnections.delete(botName);
      logger.warn({ botName }, "Removed stale bot connection");
    }

    // Get or create connection via botManager
    const connection = botManager.get(botName);
    if (connection && connection.sdk.isConnected()) {
      const conn = { bot: connection.bot, sdk: connection.sdk };
      this.botConnections.set(botName, conn);
      return conn;
    }

    // Try to connect
    try {
      const newConnection = await botManager.connect(botName);
      if (!newConnection.sdk.isConnected()) {
        logger.error({ botName }, "Bot connection established but SDK reports disconnected");
        return null;
      }
      const conn = { bot: newConnection.bot, sdk: newConnection.sdk };
      this.botConnections.set(botName, conn);
      return conn;
    } catch (error) {
      logger.error({ error, botName }, "Failed to connect to bot");
      return null;
    }
  }

  async executeAction(
    agentId: string,
    actionName: string,
    parameters: Record<string, string | number>
  ): Promise<ActionResult> {
    // Extract bot name from agentId - no fallback to prevent wrong bot execution
    const botName = this.getBotNameFromAgentId(agentId);
    if (!botName) {
      return {
        success: false,
        message: `No bot name mapped for agent ID "${agentId}". Bot name must be set when creating the runtime.`,
      };
    }

    const connection = await this.getBotConnection(botName);
    if (!connection) {
      return {
        success: false,
        message: `Bot "${botName}" is not connected or connection failed`,
      };
    }

    // Verify connection is still active before executing
    if (!connection.sdk.isConnected()) {
      this.botConnections.delete(botName);
      return {
        success: false,
        message: `Bot "${botName}" connection lost. Please reconnect.`,
      };
    }

    const { bot, sdk } = connection;

    // Default timeout for bot actions (60 seconds)
    const ACTION_TIMEOUT = 60000;

    // Helper to wrap actions with timeout
    const withTimeout = async <T>(promise: Promise<T>, timeoutMs: number, action: string): Promise<T> => {
      const timeoutPromise = new Promise<never>((_, reject) => {
        setTimeout(() => reject(new Error(`Action "${action}" timed out after ${timeoutMs}ms`)), timeoutMs);
      });
      return Promise.race([promise, timeoutPromise]);
    };

    // Normalize action name to lowercase for case-insensitive matching
    const normalizedAction = actionName.toLowerCase();
    
    try {
      switch (normalizedAction) {
        case "choptree": {
          const result = await withTimeout(bot.chopTree(), ACTION_TIMEOUT, actionName);
          return {
            success: result.success,
            message: result.message,
          };
        }
        case "walkto": {
          const x = parameters.x as number;
          const z = parameters.z as number;
          if (typeof x !== "number" || typeof z !== "number") {
            return {
              success: false,
              message: "walkTo requires x and z coordinates",
            };
          }
          // Validate coordinates are finite numbers within reasonable range
          if (!Number.isFinite(x) || !Number.isFinite(z)) {
            return {
              success: false,
              message: `Invalid coordinates: x=${x}, z=${z}. Coordinates must be finite numbers.`,
            };
          }
          // RuneScape coordinates are typically in range 0-10000
          if (x < -10000 || x > 10000 || z < -10000 || z > 10000) {
            return {
              success: false,
              message: `Coordinates out of range: (${x}, ${z}). Valid range is approximately -10000 to 10000.`,
            };
          }
          await withTimeout(bot.walkTo(x, z), ACTION_TIMEOUT, actionName);
          return {
            success: true,
            message: `Walked to (${x}, ${z})`,
          };
        }
        case "minerock": {
          // Mining rocks uses interactLoc - find nearest rock/ore/boulder
          const mineState = sdk.getState();
          if (!mineState) return { success: false, message: "Cannot get bot state" };
          const rock = mineState.nearbyLocs.find(l =>
            /rock|ore|boulder|stone|copper|tin|iron|coal|mithril|adamant|rune|clay/i.test(l.name)
          );
          if (!rock) {
            return { success: false, message: `No mineable rock nearby. Objects: ${mineState.nearbyLocs.slice(0, 5).map(l => l.name).join(", ")}` };
          }
          // Walk to rock if far, then mine (option 1)
          if (rock.distance > 3) {
            await withTimeout(bot.walkTo(rock.x, rock.z), ACTION_TIMEOUT, "walkToRock");
          }
          // Find the "Mine" option or use first option
          const mineOption = rock.optionsWithIndex?.find(o => /mine/i.test(o.text));
          const opIndex = mineOption ? mineOption.opIndex : (rock.optionsWithIndex?.[0]?.opIndex ?? 1);
          const mineResult = await withTimeout(sdk.sendInteractLoc(rock.x, rock.z, rock.id, opIndex), ACTION_TIMEOUT, actionName);
          return {
            success: mineResult.success,
            message: mineResult.success ? `Mining ${rock.name}` : mineResult.message,
          };
        }
        case "attacknpc": {
          const npcName = parameters.npcName as string;
          if (!npcName || typeof npcName !== "string" || !npcName.trim()) {
            return {
              success: false,
              message: "attackNpc requires a valid npcName parameter",
            };
          }
          const state = sdk.getState();
          if (!state) {
            return {
              success: false,
              message: "Cannot get bot state. Bot may not be in game.",
            };
          }
          if (!state.nearbyNpcs || !Array.isArray(state.nearbyNpcs)) {
            return {
              success: false,
              message: "Invalid state: nearbyNpcs not available",
            };
          }
          const npc = state.nearbyNpcs.find((n) =>
            n.name.toLowerCase().includes(npcName.toLowerCase())
          );
          if (!npc) {
            return {
              success: false,
              message: `No NPC named "${npcName}" found nearby. Available NPCs: ${state.nearbyNpcs.slice(0, 5).map(n => n.name).join(", ")}`,
            };
          }
          const result = await withTimeout(bot.attackNpc(npc), ACTION_TIMEOUT, actionName);
          return {
            success: result.success,
            message: result.message,
          };
        }
        case "openbank": {
          const result = await withTimeout(bot.openBank(), ACTION_TIMEOUT, actionName);
          return {
            success: result.success,
            message: result.message,
          };
        }
        case "fish": {
          // Fishing spots are NPCs - interact with the nearest one
          const fishState = sdk.getState();
          if (!fishState) return { success: false, message: "Cannot get bot state" };
          const fishSpot = fishState.nearbyNpcs.find(n =>
            /fishing spot|rod fishing spot|net fishing spot/i.test(n.name)
          );
          if (!fishSpot) {
            return { success: false, message: `No fishing spot nearby. NPCs: ${fishState.nearbyNpcs.slice(0, 5).map(n => n.name).join(", ")}` };
          }
          // Option 1 is typically "Net" or "Lure", option 2 is "Bait" or "Cage"
          const fishResult = await withTimeout(sdk.sendInteractNpc(fishSpot.index, 1), ACTION_TIMEOUT, actionName);
          return {
            success: fishResult.success,
            message: fishResult.success ? `Fishing at ${fishSpot.name}` : fishResult.message,
          };
        }
        case "equipitem": {
          const itemName = parameters.itemName as string;
          if (!itemName || typeof itemName !== "string") {
            return {
              success: false,
              message: "equipItem requires an itemName parameter",
            };
          }
          const result = await withTimeout(bot.equipItem(itemName), ACTION_TIMEOUT, actionName);
          return {
            success: result.success,
            message: result.message,
          };
        }
        case "eatfood":
        case "eat": {
          const foodName = parameters.foodName as string || parameters.itemName as string;
          if (!foodName || typeof foodName !== "string") {
            return {
              success: false,
              message: "eat requires a foodName parameter",
            };
          }
          const result = await withTimeout(bot.eatFood(foodName), ACTION_TIMEOUT, actionName);
          return {
            success: result.success,
            message: result.message,
          };
        }
        case "opendoor": {
          const result = await withTimeout(bot.openDoor(), ACTION_TIMEOUT, actionName);
          return {
            success: result.success,
            message: result.message,
          };
        }
        case "dropitem": {
          const itemName = parameters.itemName as string;
          if (!itemName || typeof itemName !== "string") {
            return { success: false, message: "dropItem requires an itemName parameter" };
          }
          // Find the item in inventory by name
          const dropState = sdk.getState();
          if (!dropState) {
            return { success: false, message: "Cannot get bot state" };
          }
          const dropTarget = dropState.inventory.find(i =>
            i.name.toLowerCase().includes(itemName.toLowerCase())
          );
          if (!dropTarget) {
            const invItems = dropState.inventory.slice(0, 8).map(i => i.name).join(", ");
            return { success: false, message: `Item "${itemName}" not found in inventory. Have: ${invItems}` };
          }
          const dropResult = await withTimeout(sdk.sendDropItem(dropTarget.slot), ACTION_TIMEOUT, actionName);
          return {
            success: dropResult.success,
            message: dropResult.success ? `Dropped ${dropTarget.name}` : dropResult.message,
          };
        }
        case "useitem": {
          const useItemName = parameters.itemName as string;
          if (!useItemName || typeof useItemName !== "string") {
            return { success: false, message: "useItem requires an itemName parameter" };
          }
          const useState = sdk.getState();
          if (!useState) {
            return { success: false, message: "Cannot get bot state" };
          }
          const useTarget = useState.inventory.find(i =>
            i.name.toLowerCase().includes(useItemName.toLowerCase())
          );
          if (!useTarget) {
            const invItems = useState.inventory.slice(0, 8).map(i => i.name).join(", ");
            return { success: false, message: `Item "${useItemName}" not found in inventory. Have: ${invItems}` };
          }
          // Use the first option (index 0) on the item
          const firstOption = useTarget.optionsWithIndex[0];
          if (!firstOption) {
            return { success: false, message: `No options available on ${useTarget.name}` };
          }
          const useResult = await withTimeout(sdk.sendUseItem(useTarget.slot, firstOption.opIndex), ACTION_TIMEOUT, actionName);
          return {
            success: useResult.success,
            message: useResult.success ? `Used ${useTarget.name} (${firstOption.text})` : useResult.message,
          };
        }
        case "talktonpc": {
          const npcName = parameters.npcName as string;
          if (!npcName || typeof npcName !== "string") {
            return { success: false, message: "talkToNpc requires an npcName parameter" };
          }
          const talkState = sdk.getState();
          if (!talkState) return { success: false, message: "Cannot get bot state" };
          const talkNpc = talkState.nearbyNpcs.find(n => n.name.toLowerCase().includes(npcName.toLowerCase()));
          if (!talkNpc) {
            return { success: false, message: `No NPC named "${npcName}" nearby. Available: ${talkState.nearbyNpcs.slice(0, 5).map(n => n.name).join(", ")}` };
          }
          const talkResult = await withTimeout(bot.talkTo(talkNpc), ACTION_TIMEOUT, actionName);
          return { success: talkResult.success, message: talkResult.message };
        }
        case "pickupitem": {
          const pickItemName = parameters.itemName as string;
          if (!pickItemName || typeof pickItemName !== "string") {
            return { success: false, message: "pickupItem requires an itemName parameter" };
          }
          const result = await withTimeout(bot.pickupItem(pickItemName), ACTION_TIMEOUT, actionName);
          return { success: result.success, message: result.message };
        }
        case "deposititem": {
          const depItemName = parameters.itemName as string;
          if (!depItemName || typeof depItemName !== "string") {
            return { success: false, message: "depositItem requires an itemName parameter" };
          }
          // Check if bank is open first
          const bankState = sdk.getState();
          if (!bankState?.interface?.isOpen) {
            return { success: false, message: "Bank is not open. Use OPEN_BANK first, then DEPOSIT_ITEM." };
          }
          const depAmount = typeof parameters.amount === "number" ? parameters.amount : -1;
          const result = await withTimeout(bot.depositItem(depItemName, depAmount), ACTION_TIMEOUT, actionName);
          return { success: result.success, message: result.message };
        }
        case "burnlogs": {
          const result = await withTimeout(bot.burnLogs(), ACTION_TIMEOUT, actionName);
          return { success: result.success, message: result.message };
        }
        case "unequipitem": {
          const unItemName = parameters.itemName as string;
          if (!unItemName || typeof unItemName !== "string") {
            return { success: false, message: "unequipItem requires an itemName parameter" };
          }
          const result = await withTimeout(bot.unequipItem(unItemName), ACTION_TIMEOUT, actionName);
          return { success: result.success, message: result.message };
        }
        case "buyfromshop": {
          const buyItemName = parameters.itemName as string;
          if (!buyItemName || typeof buyItemName !== "string") {
            return { success: false, message: "buyFromShop requires an itemName parameter" };
          }
          const result = await withTimeout(bot.buyFromShop(buyItemName), ACTION_TIMEOUT, actionName);
          return { success: result.success, message: result.message };
        }
        case "selltoshop": {
          const sellItemName = parameters.itemName as string;
          if (!sellItemName || typeof sellItemName !== "string") {
            return { success: false, message: "sellToShop requires an itemName parameter" };
          }
          const result = await withTimeout(bot.sellToShop(sellItemName), ACTION_TIMEOUT, actionName);
          return { success: result.success, message: result.message };
        }
        case "cookfood": {
          const cookItemName = parameters.itemName as string;
          if (!cookItemName || typeof cookItemName !== "string") {
            return { success: false, message: "cookFood requires an itemName parameter" };
          }
          // Use item on a nearby fire or range
          const result = await withTimeout(bot.useItemOnLoc(cookItemName, /fire|range|stove/i), ACTION_TIMEOUT, actionName);
          return { success: result.success, message: result.message };
        }
        case "navigatedialog": {
          const optionIndex = typeof parameters.optionIndex === "number" ? parameters.optionIndex : 0;
          const dialogResult = await withTimeout(sdk.sendClickDialog(optionIndex), ACTION_TIMEOUT, actionName);
          return { success: dialogResult.success, message: dialogResult.success ? `Clicked dialog option ${optionIndex}` : dialogResult.message };
        }
        case "setcombatstyle": {
          const style = parameters.style as number;
          if (typeof style !== "number" || style < 0 || style > 3) {
            return { success: false, message: "setCombatStyle requires style 0-3" };
          }
          const result = await withTimeout(sdk.sendSetCombatStyle(style), ACTION_TIMEOUT, actionName);
          const styleNames = ["Accurate (Attack)", "Aggressive (Strength)", "Defensive (Defence)", "Controlled (shared)"];
          return { success: result.success, message: result.success ? `Combat style set to ${styleNames[style]}` : result.message };
        }
        case "fletchlogs": {
          const result = await withTimeout(bot.fletchLogs(), ACTION_TIMEOUT, actionName);
          return { success: result.success, message: result.message };
        }
        case "craftleather": {
          const result = await withTimeout(bot.craftLeather(), ACTION_TIMEOUT, actionName);
          return { success: result.success, message: result.message };
        }
        case "smithatanvil": {
          const product = (parameters.product as string) || "dagger";
          const result = await withTimeout(bot.smithAtAnvil(product), ACTION_TIMEOUT, actionName);
          return { success: result.success, message: result.message };
        }
        case "castspell": {
          const spellNpcName = parameters.npcName as string;
          const spellComponent = parameters.spellComponent as number;
          if (!spellNpcName) return { success: false, message: "castSpell requires npcName" };
          const spellState = sdk.getState();
          if (!spellState) return { success: false, message: "Cannot get bot state" };
          const spellNpc = spellState.nearbyNpcs.find(n => n.name.toLowerCase().includes(spellNpcName.toLowerCase()));
          if (!spellNpc) return { success: false, message: `NPC "${spellNpcName}" not nearby` };
          const result = await withTimeout(bot.castSpellOnNpc(spellNpc, spellComponent), ACTION_TIMEOUT, actionName);
          return { success: result.success, message: result.message };
        }
        case "closebank": {
          const result = await withTimeout(bot.closeBank(), ACTION_TIMEOUT, actionName);
          return { success: result.success, message: result.message };
        }
        case "closeshop": {
          const result = await withTimeout(bot.closeShop(), ACTION_TIMEOUT, actionName);
          return { success: result.success, message: result.message };
        }
        case "withdrawitem": {
          const withdrawName = parameters.itemName as string;
          if (!withdrawName) return { success: false, message: "withdrawItem requires itemName" };
          // Find item in bank by name
          const bankItem = sdk.findBankItem(new RegExp(withdrawName, "i"));
          if (!bankItem) return { success: false, message: `Item "${withdrawName}" not found in bank` };
          const result = await withTimeout(bot.withdrawItem(bankItem.slot), ACTION_TIMEOUT, actionName);
          return { success: result.success, message: result.message };
        }
        case "openshop": {
          const result = await withTimeout(bot.openShop(), ACTION_TIMEOUT, actionName);
          return { success: result.success, message: result.message };
        }
        case "interactobject": {
          const objName = parameters.objectName as string;
          const optionTextParam = parameters.optionText as string | undefined;
          if (!objName) return { success: false, message: "interactObject requires objectName" };
          const objState = sdk.getState();
          if (!objState) return { success: false, message: "Cannot get bot state" };
          const loc = objState.nearbyLocs.find(l => l.name.toLowerCase().includes(objName.toLowerCase()));
          if (!loc) {
            const available = objState.nearbyLocs.slice(0, 8).map(l => l.name).join(", ");
            return { success: false, message: `Object "${objName}" not nearby. Available: ${available}` };
          }
          // If optionText specified, find matching option; otherwise use first option
          let option = loc.optionsWithIndex?.[0];
          if (optionTextParam) {
            const matchedOption = loc.optionsWithIndex?.find(o =>
              o.text.toLowerCase().includes(optionTextParam.toLowerCase())
            );
            if (matchedOption) {
              option = matchedOption;
            } else {
              const availableOpts = loc.optionsWithIndex?.map(o => o.text).join(", ") || "none";
              return { success: false, message: `Option "${optionTextParam}" not found on ${loc.name}. Available options: ${availableOpts}` };
            }
          }
          const opIndex = option ? option.opIndex : 1;
          const result = await withTimeout(sdk.sendInteractLoc(loc.x, loc.z, loc.id, opIndex), ACTION_TIMEOUT, actionName);
          return { success: result.success, message: result.success ? `Interacted with ${loc.name}${option ? ` (${option.text})` : ""}` : result.message };
        }
        case "pickpocketnpc": {
          const ppNpcName = parameters.npcName as string;
          if (!ppNpcName || typeof ppNpcName !== "string") {
            return { success: false, message: "pickpocketNpc requires a npcName parameter" };
          }
          const ppState = sdk.getState();
          if (!ppState) return { success: false, message: "Cannot get bot state" };
          const ppNpc = ppState.nearbyNpcs.find(n =>
            n.name.toLowerCase().includes(ppNpcName.toLowerCase())
          );
          if (!ppNpc) {
            return { success: false, message: `No NPC named "${ppNpcName}" nearby. Available: ${ppState.nearbyNpcs.slice(0, 5).map(n => n.name).join(", ")}` };
          }
          // Find the "Pickpocket" option on this NPC
          const pickpocketOpt = ppNpc.optionsWithIndex?.find(o => /pickpocket/i.test(o.text));
          if (!pickpocketOpt) {
            const availableOpts = ppNpc.optionsWithIndex?.map(o => o.text).join(", ") || "none";
            return { success: false, message: `NPC "${ppNpc.name}" has no Pickpocket option. Available options: ${availableOpts}` };
          }
          const ppResult = await withTimeout(sdk.sendInteractNpc(ppNpc.index, pickpocketOpt.opIndex), ACTION_TIMEOUT, actionName);
          return {
            success: ppResult.success,
            message: ppResult.success ? `Pickpocketed ${ppNpc.name}` : ppResult.message,
          };
        }
        case "useitemonobject": {
          const uioItemName = parameters.itemName as string;
          const uioObjectName = parameters.objectName as string;
          if (!uioItemName || typeof uioItemName !== "string") {
            return { success: false, message: "useItemOnObject requires an itemName parameter" };
          }
          if (!uioObjectName || typeof uioObjectName !== "string") {
            return { success: false, message: "useItemOnObject requires an objectName parameter" };
          }
          const uioState = sdk.getState();
          if (!uioState) return { success: false, message: "Cannot get bot state" };
          // Find item in inventory
          const uioItem = uioState.inventory.find(i =>
            i.name.toLowerCase().includes(uioItemName.toLowerCase())
          );
          if (!uioItem) {
            const invItems = uioState.inventory.slice(0, 8).map(i => i.name).join(", ");
            return { success: false, message: `Item "${uioItemName}" not in inventory. Have: ${invItems}` };
          }
          // Find object nearby
          const uioLoc = uioState.nearbyLocs.find(l =>
            l.name.toLowerCase().includes(uioObjectName.toLowerCase())
          );
          if (!uioLoc) {
            const available = uioState.nearbyLocs.slice(0, 8).map(l => l.name).join(", ");
            return { success: false, message: `Object "${uioObjectName}" not nearby. Available: ${available}` };
          }
          // Walk closer if needed
          if (uioLoc.distance > 3) {
            await withTimeout(bot.walkTo(uioLoc.x, uioLoc.z), ACTION_TIMEOUT, "walkToObject");
          }
          // Use item on location
          const uioResult = await withTimeout(sdk.sendUseItemOnLoc(uioItem.slot, uioLoc.x, uioLoc.z, uioLoc.id), ACTION_TIMEOUT, actionName);
          return {
            success: uioResult.success,
            message: uioResult.success ? `Used ${uioItem.name} on ${uioLoc.name}` : uioResult.message,
          };
        }
        case "useitemonitem": {
          const srcName = parameters.sourceItem as string;
          const tgtName = parameters.targetItem as string;
          if (!srcName || !tgtName) return { success: false, message: "useItemOnItem requires sourceItem and targetItem" };
          const itemState = sdk.getState();
          if (!itemState) return { success: false, message: "Cannot get bot state" };
          const srcItem = itemState.inventory.find(i => i.name.toLowerCase().includes(srcName.toLowerCase()));
          const tgtItem = itemState.inventory.find(i => i.name.toLowerCase().includes(tgtName.toLowerCase()) && i.slot !== srcItem?.slot);
          if (!srcItem) return { success: false, message: `Source "${srcName}" not in inventory` };
          if (!tgtItem) return { success: false, message: `Target "${tgtName}" not in inventory` };
          const result = await withTimeout(sdk.sendUseItemOnItem(srcItem.slot, tgtItem.slot), ACTION_TIMEOUT, actionName);
          return { success: result.success, message: result.success ? `Used ${srcItem.name} on ${tgtItem.name}` : result.message };
        }
        default:
          return {
            success: false,
            message: `Unknown action: ${actionName}`,
          };
      }
    } catch (error) {
      logger.error({ error, actionName, parameters }, "Failed to execute bot action");
      return {
        success: false,
        message: error instanceof Error ? error.message : "Unknown error",
      };
    }
  }

  async getBotState(agentId: string): Promise<BotState | null> {
    const botName = this.getBotNameFromAgentId(agentId);
    if (!botName) {
      logger.error({ agentId }, "Cannot get bot state: no bot name mapped for agent ID");
      return null;
    }

    const connection = await this.getBotConnection(botName);
    if (!connection) {
      logger.error({ botName, agentId }, "Cannot get bot state: bot not connected");
      return null;
    }

    // Verify connection is still active
    if (!connection.sdk.isConnected()) {
      this.botConnections.delete(botName);
      logger.error({ botName }, "Cannot get bot state: connection lost");
      return null;
    }

    const { sdk } = connection;
    const state = sdk.getState();

    if (!state) {
      logger.warn({ botName }, "Bot state is null - bot may not be in game");
      return null;
    }

    if (!state.player) {
      logger.warn({ botName }, "Bot player state is null");
      return null;
    }

    return {
      tick: state.tick,
      inGame: state.inGame,
      player: {
        name: state.player.name,
        combatLevel: state.player.combatLevel,
        worldX: state.player.worldX,
        worldZ: state.player.worldZ,
        level: state.player.level,
        runEnergy: state.player.runEnergy,
        combat: {
          inCombat: state.player.combat?.inCombat ?? false,
          targetIndex: state.player.combat?.targetIndex ?? -1,
          lastDamageTick: state.player.combat?.lastDamageTick ?? -1,
        },
      },
      skills: state.skills.map((s) => ({
        name: s.name,
        level: s.level,
        baseLevel: s.baseLevel,
        experience: s.experience,
      })),
      inventory: state.inventory.map((item) => ({
        name: item.name,
        count: item.count,
        slot: item.slot,
      })),
      equipment: state.equipment.map((item) => ({
        name: item.name,
        count: item.count,
        slot: item.slot,
      })),
      nearbyNpcs: state.nearbyNpcs.map((npc) => ({
        name: npc.name,
        combatLevel: npc.combatLevel,
        x: npc.x,
        z: npc.z,
        distance: npc.distance,
        options: (npc.optionsWithIndex ?? [])
          .map(o => o.text)
          .filter(t => !/^examine$/i.test(t)),
      })),
      nearbyLocs: state.nearbyLocs.map((loc) => ({
        name: loc.name,
        x: loc.x,
        z: loc.z,
        distance: loc.distance,
        options: (loc.optionsWithIndex ?? [])
          .map(o => o.text)
          .filter(t => !/^examine$/i.test(t)),
      })),
      groundItems: state.groundItems.map((item) => ({
        name: item.name,
        count: item.count,
        x: item.x,
        z: item.z,
        distance: item.distance,
      })),
      gameMessages: state.gameMessages.map((msg) => ({
        text: msg.text,
        sender: msg.sender,
        tick: msg.tick,
      })),
      combatEvents: state.combatEvents.map((evt) => ({
        tick: evt.tick,
        type: evt.type,
        damage: evt.damage,
      })),
    };
  }

  private getBotNameFromAgentId(agentId: string): string | null {
    // Get bot name from mapping
    const botName = this.agentIdToBotName.get(agentId);
    if (botName) {
      return botName;
    }
    // Fallback: try to get from runtime settings
    if (this.runtime) {
      const settingBotName = this.runtime.getSetting("RS_SDK_BOT_NAME") as string;
      if (settingBotName && settingBotName.trim()) {
        this.agentIdToBotName.set(agentId, settingBotName);
        return settingBotName;
      }
    }
    // No fallback - return null to force explicit error
    return null;
  }

  get capabilityDescription(): string {
    return "Provides control over RuneScape bots including movement, combat, skills, and inventory management";
  }

  async stop(): Promise<void> {
    // Disconnect all bot connections
    for (const [botName, connection] of this.botConnections) {
      try {
        connection.sdk.disconnect();
        logger.info({ botName }, "Disconnected bot on service stop");
      } catch (error) {
        logger.error({ botName, error }, "Error disconnecting bot on service stop");
      }
    }
    this.botConnections.clear();
    this.agentIdToBotName.clear();
    this.runtime = null;
  }
}
