/** Skill training recommendations by level range */

export interface TrainingSpot {
  levelRange: [number, number]; // [min, max]
  method: string;
  location: string;
  coords: { x: number; z: number };
  requirements: string;
  notes: string;
}

export interface SkillTraining {
  skill: string;
  spots: TrainingSpot[];
}

export const SKILL_TRAINING: SkillTraining[] = [
  {
    skill: "Woodcutting",
    spots: [
      { levelRange: [1, 15], method: "Normal trees", location: "Lumbridge", coords: { x: 3222, z: 3244 }, requirements: "Any axe", notes: "Trees everywhere in Lumbridge" },
      { levelRange: [15, 30], method: "Oak trees", location: "Lumbridge/Varrock", coords: { x: 3204, z: 3242 }, requirements: "Any axe", notes: "Oaks near Lumbridge church or Varrock west bank" },
      { levelRange: [30, 45], method: "Willow trees", location: "Draynor Village", coords: { x: 3089, z: 3233 }, requirements: "Any axe", notes: "Willows by river, bank nearby" },
      { levelRange: [45, 60], method: "Maple trees", location: "Seers' Village", coords: { x: 2725, z: 3491 }, requirements: "Any axe", notes: "Maples near bank" },
      { levelRange: [60, 99], method: "Yew trees", location: "Edgeville/Varrock", coords: { x: 3087, z: 3481 }, requirements: "Any axe", notes: "Yews behind Edgeville bank or Varrock castle" },
    ],
  },
  {
    skill: "Mining",
    spots: [
      { levelRange: [1, 15], method: "Copper & Tin", location: "SE Varrock Mine", coords: { x: 3285, z: 3365 }, requirements: "Any pickaxe", notes: "Copper/tin rocks, works reliably. AVOID Lumbridge Swamp mine (interactions fail silently). Smelt into bronze bars at Al Kharid furnace." },
      { levelRange: [15, 30], method: "Iron ore", location: "Al Kharid Mine", coords: { x: 3300, z: 3310 }, requirements: "Any pickaxe", notes: "Iron rocks south of Al Kharid" },
      { levelRange: [30, 45], method: "Iron/Coal", location: "Varrock SE Mine", coords: { x: 3285, z: 3365 }, requirements: "Any pickaxe", notes: "Mix of ores near Varrock" },
      { levelRange: [45, 99], method: "Coal/Mithril", location: "Mining Guild (Falador)", coords: { x: 3038, z: 9737 }, requirements: "60 Mining", notes: "Best rates, requires 60 Mining" },
    ],
  },
  {
    skill: "Fishing",
    spots: [
      { levelRange: [1, 20], method: "Small net fishing (Shrimp)", location: "Draynor Village", coords: { x: 3087, z: 3230 }, requirements: "Small fishing net", notes: "USE DRAYNOR for level 1 fishing. Shrimp/anchovies. WARNING: Lumbridge area has NO small net spots (only fly fishing at 20+). Dark wizards to the north - stay south if low combat." },
      { levelRange: [20, 40], method: "Fly fishing (Trout)", location: "Barbarian Village", coords: { x: 3103, z: 3434 }, requirements: "Fly fishing rod + feathers", notes: "Trout at 20, Salmon at 30" },
      { levelRange: [40, 62], method: "Lobster", location: "Catherby", coords: { x: 2837, z: 3431 }, requirements: "Lobster pot", notes: "Lobsters near bank" },
      { levelRange: [62, 99], method: "Swordfish/Monkfish", location: "Fishing Guild", coords: { x: 2604, z: 3414 }, requirements: "68 Fishing for guild", notes: "Best fishing spots" },
    ],
  },
  {
    skill: "Cooking",
    spots: [
      { levelRange: [1, 30], method: "Cook shrimp/meat on fire", location: "Lumbridge Castle range", coords: { x: 3211, z: 3216 }, requirements: "Raw food", notes: "Lumbridge range never burns food for Cook's Assistant" },
      { levelRange: [30, 99], method: "Cook fish at range", location: "Cooking Guild / Rogues' Den", coords: { x: 3044, z: 3445 }, requirements: "Raw food + 32 Cooking for guild", notes: "Range near bank for efficiency" },
    ],
  },
  {
    skill: "Firemaking",
    spots: [
      { levelRange: [1, 15], method: "Burn normal logs", location: "Anywhere open", coords: { x: 3222, z: 3244 }, requirements: "Tinderbox + Logs", notes: "Use BURN_LOGS anywhere with open space" },
      { levelRange: [15, 30], method: "Burn oak logs", location: "Near bank", coords: { x: 3222, z: 3244 }, requirements: "Tinderbox + Oak logs", notes: "Chop oaks, burn them" },
      { levelRange: [30, 99], method: "Burn willow logs", location: "Draynor Village", coords: { x: 3089, z: 3233 }, requirements: "Tinderbox + Willow logs", notes: "Cut willows, burn, repeat" },
    ],
  },
  {
    skill: "Smithing",
    spots: [
      { levelRange: [1, 15], method: "Smelt bronze bars + smith daggers", location: "Al Kharid furnace", coords: { x: 3275, z: 3186 }, requirements: "Hammer + copper/tin ore", notes: "Mine at swamp, smelt at Al Kharid, smith at Varrock anvils" },
      { levelRange: [15, 30], method: "Smith iron items", location: "Varrock anvils", coords: { x: 3188, z: 3426 }, requirements: "Hammer + iron bars", notes: "Anvils west of Varrock bank" },
      { levelRange: [30, 99], method: "Smith steel/mithril", location: "Varrock anvils", coords: { x: 3188, z: 3426 }, requirements: "Hammer + bars", notes: "Bank nearby for bar supplies" },
    ],
  },
  {
    skill: "Crafting",
    spots: [
      { levelRange: [1, 20], method: "Leather crafting", location: "Al Kharid / Lumbridge", coords: { x: 3275, z: 3192 }, requirements: "Needle + Thread + Leather", notes: "Kill cows for hides, tan at Al Kharid Ellis, craft" },
      { levelRange: [20, 40], method: "Gem cutting / Silver jewelry", location: "Al Kharid", coords: { x: 3275, z: 3186 }, requirements: "Chisel + uncut gems", notes: "Buy gems, cut them" },
    ],
  },
  {
    skill: "Combat (Attack/Strength/Defence)",
    spots: [
      { levelRange: [1, 10], method: "Chickens", location: "Lumbridge north", coords: { x: 3237, z: 3295 }, requirements: "Any weapon", notes: "Safe, feathers for fletching. Use SET_COMBAT_STYLE to choose skill" },
      { levelRange: [10, 20], method: "Cows", location: "Lumbridge NE", coords: { x: 3253, z: 3270 }, requirements: "Any weapon", notes: "Cowhides for crafting, good XP" },
      { levelRange: [20, 40], method: "Goblins / Al Kharid warriors", location: "Lumbridge east / Al Kharid", coords: { x: 3259, z: 3228 }, requirements: "Decent weapon", notes: "Goblins are easy, warriors give more XP" },
      { levelRange: [40, 60], method: "Hill Giants", location: "Edgeville Dungeon", coords: { x: 3116, z: 9852 }, requirements: "Good weapon + food", notes: "Big bones for Prayer, limpwurt roots" },
      { levelRange: [60, 99], method: "Moss Giants / Lesser Demons", location: "Various dungeons", coords: { x: 2913, z: 9810 }, requirements: "Strong gear + food", notes: "Advanced combat training" },
    ],
  },
  {
    skill: "Fletching",
    spots: [
      { levelRange: [1, 25], method: "Arrow shafts + Shortbows", location: "Near any bank", coords: { x: 3093, z: 3244 }, requirements: "Knife + Logs", notes: "Cut logs into arrow shafts or bows" },
      { levelRange: [25, 50], method: "Oak shortbows/longbows", location: "Near any bank", coords: { x: 3093, z: 3244 }, requirements: "Knife + Oak logs", notes: "Bank nearby for log supplies" },
    ],
  },
  {
    skill: "Prayer",
    spots: [
      { levelRange: [1, 99], method: "Bury bones", location: "Anywhere (near kills)", coords: { x: 3253, z: 3270 }, requirements: "Bones", notes: "Kill NPCs, pick up bones, use USE_ITEM to bury. Big bones from giants give 3x XP" },
    ],
  },
  {
    skill: "Thieving",
    spots: [
      { levelRange: [1, 10], method: "Pickpocket Men/Women", location: "Lumbridge Castle", coords: { x: 3222, z: 3218 }, requirements: "None (no tools needed!)", notes: "Use PICKPOCKET_NPC on Man/Woman. 3GP + 8XP per success. ~5 second stun on failure. Requires no equipment - great for fresh accounts." },
      { levelRange: [10, 25], method: "Pickpocket Farmers", location: "Near Draynor", coords: { x: 3167, z: 3283 }, requirements: "None", notes: "Farmers give 9GP per pickpocket. Higher success rate at higher levels." },
      { levelRange: [25, 40], method: "Pickpocket Warriors/Guards", location: "Al Kharid / Falador", coords: { x: 3293, z: 3175 }, requirements: "Food recommended", notes: "Al Kharid has kebabs (1gp from Karim via TALK_TO_NPC dialog) to sustain HP from stun damage." },
      { levelRange: [40, 99], method: "Pickpocket Guards/Knights", location: "Falador / Ardougne", coords: { x: 2960, z: 3339 }, requirements: "Food", notes: "Higher level targets for better GP" },
    ],
  },
  {
    skill: "Ranged",
    spots: [
      { levelRange: [1, 20], method: "Shortbow on Chickens", location: "Lumbridge Chickens", coords: { x: 3237, z: 3295 }, requirements: "Shortbow + Bronze arrows", notes: "Equip bow and arrows with EQUIP_ITEM first. Pick up arrows after kills to conserve ammo. Gate at entrance may need OPEN_DOOR." },
      { levelRange: [20, 40], method: "Oak shortbow on Cows", location: "Lumbridge Cows", coords: { x: 3253, z: 3290 }, requirements: "Oak shortbow + Iron arrows", notes: "Better XP per hour. Collect cowhides for profit." },
      { levelRange: [40, 99], method: "Maple/Yew bow on stronger targets", location: "Various", coords: { x: 3253, z: 3290 }, requirements: "Good bow + arrows", notes: "Upgrade weapons as levels increase" },
    ],
  },
  {
    skill: "Magic",
    spots: [
      { levelRange: [1, 13], method: "Wind Strike on Chickens", location: "Lumbridge Chickens", coords: { x: 3237, z: 3295 }, requirements: "Air runes + Mind runes", notes: "Use CAST_SPELL on Chicken. 1 Air + 1 Mind rune per cast. Gate at entrance may need OPEN_DOOR." },
      { levelRange: [13, 23], method: "Fire Strike on Cows/Goblins", location: "Lumbridge", coords: { x: 3253, z: 3290 }, requirements: "Air runes + Mind runes + Fire runes", notes: "Higher damage spells as level increases" },
      { levelRange: [23, 99], method: "Bolt spells on stronger targets", location: "Various", coords: { x: 3253, z: 3290 }, requirements: "Various runes", notes: "Upgrade spells as magic level increases" },
    ],
  },
];

/** Get training recommendation for a skill at a given level */
export function getTrainingRecommendation(skill: string, level: number): TrainingSpot | null {
  const skillData = SKILL_TRAINING.find(s => s.skill.toLowerCase().includes(skill.toLowerCase()));
  if (!skillData) return null;
  // Find the spot that matches the current level
  for (const spot of skillData.spots) {
    if (level >= spot.levelRange[0] && level <= spot.levelRange[1]) {
      return spot;
    }
  }
  // Return last spot if overleveled
  return skillData.spots[skillData.spots.length - 1] || null;
}
