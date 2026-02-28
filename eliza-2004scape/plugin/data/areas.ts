/** Game area definitions with coordinate bounds, features, and navigation */

export interface GameArea {
  name: string;
  bounds: { minX: number; maxX: number; minZ: number; maxZ: number };
  description: string;
  features: string[];
  trainableSkills: string[];
  keyNpcs: string[];
  adjacentAreas: { name: string; direction: string; walkCoords: { x: number; z: number }; description: string }[];
}

export const GAME_AREAS: GameArea[] = [
  {
    name: "Lumbridge",
    bounds: { minX: 3190, maxX: 3270, minZ: 3190, maxZ: 3270 },
    description: "Starting town. Safe area with basic training. WARNING: There is NO BANK in Lumbridge in 2004scape! Nearest banks: Draynor Village (west), Al Kharid (east, 10gp toll), Varrock West (north).",
    features: ["NO BANK HERE - go to Draynor/Al Kharid/Varrock", "general store", "Bob's Axes shop (bronze axe 16gp, bronze pickaxe 1gp)", "goblins (east)", "cows (northeast, gate at 3253,3270)", "chickens (north, gate at 3237,3295)", "trees everywhere", "Cook's range in castle", "Men/Women for pickpocketing (Thieving)", "knife spawn at (3224, 3202)"],
    trainableSkills: ["Woodcutting (trees)", "Cooking (range in castle)", "Firemaking", "Combat (chickens north, goblins east, cows NE)", "Thieving (pickpocket Men/Women)", "Prayer (bury bones from kills)", "Fletching (pick up knife at 3224,3202)"],
    keyNpcs: ["Cook (Lumbridge Castle - quest)", "Duke Horacio (castle upstairs)", "Shop keeper (general store)", "Hans (castle courtyard)", "Bob (axes shop at 3230,3203)"],
    adjacentAreas: [
      { name: "Varrock", direction: "north", walkCoords: { x: 3222, z: 3400 }, description: "City with banks, Grand Exchange, anvils, rune shop. Has Varrock West Bank (3185, 3436)." },
      { name: "Al Kharid", direction: "east", walkCoords: { x: 3293, z: 3174 }, description: "Desert town with bank, furnace, tanner. Requires 10gp toll at gate (3268, 3228) - use OPEN_DOOR then NAVIGATE_DIALOG to pay." },
      { name: "Draynor Village", direction: "west", walkCoords: { x: 3093, z: 3244 }, description: "Fishing village with bank, willows. DANGER: Dark Wizards at ~(3220, 3220) between here and Draynor - stay north of z=3240 when walking west!" },
      { name: "Lumbridge Swamp", direction: "south", walkCoords: { x: 3202, z: 3170 }, description: "Swamp with fly fishing spots (level 20+ only, NO small net). Swamp mine is UNRELIABLE - use SE Varrock mine instead." },
    ],
  },
  {
    name: "Lumbridge Swamp",
    bounds: { minX: 3138, maxX: 3260, minZ: 3130, maxZ: 3190 },
    description: "Swampy area south of Lumbridge. WARNING: Mining site here is UNRELIABLE (interactions fail silently) - use SE Varrock mine (3285, 3365) instead. Fishing is fly fishing only (level 20+ required).",
    features: ["mining site (UNRELIABLE - avoid)", "fly fishing spots (level 20+ only, NOT for beginners)", "swamp tar", "giant rats"],
    trainableSkills: ["Fishing (fly fishing level 20+ only)", "Combat (giant rats)"],
    keyNpcs: ["Father Urhney (Restless Ghost quest)"],
    adjacentAreas: [
      { name: "Lumbridge", direction: "north", walkCoords: { x: 3222, z: 3218 }, description: "Starting town" },
      { name: "Al Kharid", direction: "east", walkCoords: { x: 3293, z: 3174 }, description: "Desert town with furnace, bank" },
    ],
  },
  {
    name: "Varrock",
    bounds: { minX: 3170, maxX: 3290, minZ: 3370, maxZ: 3510 },
    description: "Major city north of Lumbridge. Best shops, Grand Exchange, multiple training options.",
    features: ["Grand Exchange (NW)", "West bank", "East bank", "sword shop", "rune shop", "staff shop", "general store", "dark wizards (south entrance)", "Varrock Palace"],
    trainableSkills: ["Woodcutting (oaks, yews)", "Mining (SE mine)", "Smithing (anvils west)", "Combat (dark wizards south, guards)", "Magic (rune shop for supplies)"],
    keyNpcs: ["Aubury (rune shop - Rune Mysteries quest)", "Zaff (staff shop)", "Thessalia (clothes shop)", "Romeo (Romeo & Juliet quest)", "Museum curator"],
    adjacentAreas: [
      { name: "Lumbridge", direction: "south", walkCoords: { x: 3222, z: 3260 }, description: "Starting town" },
      { name: "Edgeville", direction: "northwest", walkCoords: { x: 3093, z: 3493 }, description: "Near wilderness, furnace, yew trees" },
      { name: "Barbarian Village", direction: "west", walkCoords: { x: 3082, z: 3420 }, description: "Fishing (trout/salmon), spinning wheel" },
    ],
  },
  {
    name: "Al Kharid",
    bounds: { minX: 3270, maxX: 3340, minZ: 3140, maxZ: 3320 },
    description: "Desert town east of Lumbridge. Excellent for crafting, smithing, fishing, and thieving. Bank at (3269, 3167).",
    features: ["bank (3269, 3167)", "furnace (3274, 3186) - use USE_ITEM_ON_OBJECT for smelting ore", "tanner Ellis (3274, 3192) - tan hides via TALK_TO_NPC + NAVIGATE_DIALOG", "Dommik's crafting shop (3321, 3179)", "scimitar shop Zeke (3287, 3186)", "fishing spots (3267, 3148) - shrimp/net fishing", "Karim kebab seller (3273, 3180) - 1gp kebabs via TALK_TO_NPC dialog (NOT a shop!)", "Men/Warriors for pickpocketing/combat (3293, 3175)"],
    trainableSkills: ["Mining (south mine)", "Smithing (furnace + anvil)", "Crafting (tanner + furnace)", "Fishing (shrimp at 3267, 3148)", "Cooking (range at 3273, 3180)", "Combat (Al Kharid warriors lvl 9 - can hit hard in multicombat)", "Thieving (pickpocket Men near palace)"],
    keyNpcs: ["Ellis (tanner at 3274, 3192)", "Karim (kebab seller at 3273, 3180 - dialog, not shop)", "Zeke (scimitar shop)", "Gem trader", "Shantay (desert pass south)"],
    adjacentAreas: [
      { name: "Lumbridge", direction: "west", walkCoords: { x: 3268, z: 3226 }, description: "Starting town (through gate, 10gp toll - use OPEN_DOOR then NAVIGATE_DIALOG to select 'Yes')" },
      { name: "Lumbridge Swamp", direction: "southwest", walkCoords: { x: 3202, z: 3170 }, description: "Swamp area (avoid mining here)" },
    ],
  },
  {
    name: "Draynor Village",
    bounds: { minX: 3070, maxX: 3130, minZ: 3225, maxZ: 3280 },
    description: "Small fishing village west of Lumbridge. Best early fishing and willows.",
    features: ["bank", "willow trees (by river)", "fishing spots", "market stalls", "Draynor Manor (north)"],
    trainableSkills: ["Fishing (net/bait)", "Woodcutting (willows for 30+)", "Firemaking", "Thieving (market stalls)"],
    keyNpcs: ["Wise Old Man", "Morgan (Vampire Slayer quest)", "Ned (rope maker)"],
    adjacentAreas: [
      { name: "Lumbridge", direction: "east", walkCoords: { x: 3222, z: 3244 }, description: "Starting town" },
      { name: "Port Sarim", direction: "south", walkCoords: { x: 3047, z: 3236 }, description: "Port with fishing shop, charter ships" },
      { name: "Barbarian Village", direction: "north", walkCoords: { x: 3082, z: 3420 }, description: "Trout/salmon fishing" },
    ],
  },
  {
    name: "Barbarian Village",
    bounds: { minX: 3070, maxX: 3110, minZ: 3390, maxZ: 3440 },
    description: "Village between Varrock and Falador. Great for fly fishing.",
    features: ["fishing spots (trout, salmon)", "spinning wheel", "helmet shop", "barbarians for combat"],
    trainableSkills: ["Fishing (fly fishing - trout/salmon)", "Crafting (spinning wheel)", "Combat (barbarians)"],
    keyNpcs: ["Peksa (helmet shop)"],
    adjacentAreas: [
      { name: "Varrock", direction: "east", walkCoords: { x: 3210, z: 3424 }, description: "Major city" },
      { name: "Edgeville", direction: "north", walkCoords: { x: 3093, z: 3493 }, description: "Near wilderness" },
      { name: "Falador", direction: "southwest", walkCoords: { x: 2965, z: 3380 }, description: "Major city with mining guild" },
    ],
  },
  {
    name: "Edgeville",
    bounds: { minX: 3075, maxX: 3115, minZ: 3465, maxZ: 3510 },
    description: "Town on the edge of wilderness. Has furnace and yew trees.",
    features: ["bank", "furnace", "yew trees", "wilderness border (north - dangerous!)"],
    trainableSkills: ["Smithing (furnace + bank nearby)", "Woodcutting (yew trees)", "Firemaking"],
    keyNpcs: ["Oziach (Dragon Slayer quest)", "Vannaka (Slayer master)"],
    adjacentAreas: [
      { name: "Varrock", direction: "southeast", walkCoords: { x: 3210, z: 3424 }, description: "Major city" },
      { name: "Barbarian Village", direction: "south", walkCoords: { x: 3082, z: 3420 }, description: "Fly fishing" },
    ],
  },
];

/** Find which area contains the given coordinates */
export function findArea(x: number, z: number): GameArea | null {
  for (const area of GAME_AREAS) {
    if (x >= area.bounds.minX && x <= area.bounds.maxX && z >= area.bounds.minZ && z <= area.bounds.maxZ) {
      return area;
    }
  }
  return null;
}

/** Find the nearest area to the given coordinates */
export function findNearestArea(x: number, z: number): { area: GameArea; distance: number } {
  let nearest = GAME_AREAS[0];
  let bestDist = Infinity;
  for (const area of GAME_AREAS) {
    const cx = (area.bounds.minX + area.bounds.maxX) / 2;
    const cz = (area.bounds.minZ + area.bounds.maxZ) / 2;
    const dist = Math.sqrt((x - cx) ** 2 + (z - cz) ** 2);
    if (dist < bestDist) {
      bestDist = dist;
      nearest = area;
    }
  }
  return { area: nearest, distance: Math.round(bestDist) };
}
