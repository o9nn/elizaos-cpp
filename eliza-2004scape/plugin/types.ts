export const RS_SDK_SERVICE_NAME = "rs-sdk-service";

export interface BotState {
  player: {
    name: string;
    combatLevel: number;
    worldX: number;
    worldZ: number;
    level: number;
    runEnergy: number;
    combat: {
      inCombat: boolean;
      targetIndex: number;
      lastDamageTick: number;
    };
  };
  skills: Array<{
    name: string;
    level: number;
    baseLevel: number;
    experience: number;
  }>;
  inventory: Array<{
    name: string;
    count: number;
    slot: number;
  }>;
  equipment: Array<{
    name: string;
    count: number;
    slot: number;
  }>;
  nearbyNpcs: Array<{
    name: string;
    combatLevel: number;
    x: number;
    z: number;
    distance: number;
    options: string[];
  }>;
  nearbyLocs: Array<{
    name: string;
    x: number;
    z: number;
    distance: number;
    options: string[];
  }>;
  groundItems: Array<{
    name: string;
    count: number;
    x: number;
    z: number;
    distance: number;
  }>;
  gameMessages: Array<{
    text: string;
    sender: string;
    tick: number;
  }>;
  combatEvents: Array<{
    tick: number;
    type: "damage_taken" | "damage_dealt" | "kill";
    damage: number;
  }>;
  tick: number;
  inGame: boolean;
}

export interface ActionResult {
  success: boolean;
  message?: string;
}

/** Event log entry for tracking bot actions over time */
export interface EventLogEntry {
  step: number;
  timestamp: number;
  action: string;
  result: "success" | "failure";
  message: string;
}
