import type { Action } from "@elizaos/core";

// Core movement & interaction
import walkToAction from "./walkTo";
import openDoorAction from "./openDoor";
import talkToNpcAction from "./talkToNpc";
import navigateDialogAction from "./navigateDialog";
import interactObjectAction from "./interactObject";

// Resource gathering
import chopTreeAction from "./chopTree";
import mineRockAction from "./mineRock";
import fishAction from "./fish";

// Combat
import attackNpcAction from "./attackNpc";
import eatAction from "./eat";
import setCombatStyleAction from "./setCombatStyle";
import castSpellAction from "./castSpell";

// Inventory management
import dropItemAction from "./dropItem";
import useItemAction from "./useItem";
import pickupItemAction from "./pickupItem";
import equipItemAction from "./equipItem";
import unequipItemAction from "./unequipItem";
import useItemOnItemAction from "./useItemOnItem";

// Banking
import openBankAction from "./openBank";
import closeBankAction from "./closeBank";
import depositItemAction from "./depositItem";
import withdrawItemAction from "./withdrawItem";

// Shopping
import openShopAction from "./openShop";
import closeShopAction from "./closeShop";
import buyFromShopAction from "./buyFromShop";
import sellToShopAction from "./sellToShop";

// Crafting & processing
import burnLogsAction from "./burnLogs";
import cookFoodAction from "./cookFood";
import fletchLogsAction from "./fletchLogs";
import craftLeatherAction from "./craftLeather";
import smithAtAnvilAction from "./smithAtAnvil";

// Thieving
import pickpocketNpcAction from "./pickpocketNpc";

// Item on object interactions
import useItemOnObjectAction from "./useItemOnObject";

// Export all actions as an array for the plugin
export const rsSdkActions: Action[] = [
  // Core movement & interaction
  walkToAction,
  openDoorAction,
  talkToNpcAction,
  navigateDialogAction,
  interactObjectAction,

  // Resource gathering
  chopTreeAction,
  mineRockAction,
  fishAction,

  // Combat
  attackNpcAction,
  eatAction,
  setCombatStyleAction,
  castSpellAction,

  // Inventory management
  dropItemAction,
  useItemAction,
  pickupItemAction,
  equipItemAction,
  unequipItemAction,
  useItemOnItemAction,

  // Banking
  openBankAction,
  closeBankAction,
  depositItemAction,
  withdrawItemAction,

  // Shopping
  openShopAction,
  closeShopAction,
  buyFromShopAction,
  sellToShopAction,

  // Crafting & processing
  burnLogsAction,
  cookFoodAction,
  fletchLogsAction,
  craftLeatherAction,
  smithAtAnvilAction,

  // Thieving
  pickpocketNpcAction,

  // Item on object interactions
  useItemOnObjectAction,
];

// Named exports
export {
  walkToAction, openDoorAction, talkToNpcAction, navigateDialogAction, interactObjectAction,
  chopTreeAction, mineRockAction, fishAction,
  attackNpcAction, eatAction, setCombatStyleAction, castSpellAction,
  dropItemAction, useItemAction, pickupItemAction, equipItemAction, unequipItemAction, useItemOnItemAction,
  openBankAction, closeBankAction, depositItemAction, withdrawItemAction,
  openShopAction, closeShopAction, buyFromShopAction, sellToShopAction,
  burnLogsAction, cookFoodAction, fletchLogsAction, craftLeatherAction, smithAtAnvilAction,
  pickpocketNpcAction, useItemOnObjectAction,
};
