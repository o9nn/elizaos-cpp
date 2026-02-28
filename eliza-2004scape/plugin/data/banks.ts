/** All major bank locations in the early-mid game areas */

export interface BankLocation {
  name: string;
  x: number;
  z: number;
  area: string;
  notes: string;
}

// NOTE: There is NO bank in Lumbridge in 2004scape!
export const BANK_LOCATIONS: BankLocation[] = [
  { name: "Al Kharid Bank", x: 3269, z: 3167, area: "Al Kharid", notes: "Near furnace and tanner. Requires 10gp toll or Prince Ali Rescue quest to access from Lumbridge." },
  { name: "Draynor Village Bank", x: 3092, z: 3243, area: "Draynor Village", notes: "Near willows and fishing. WARNING: Dark wizards between Lumbridge and Draynor at ~(3220, 3220) - stay north of z=3240 when walking." },
  { name: "Varrock West Bank", x: 3185, z: 3436, area: "Varrock", notes: "Near anvils and Grand Exchange. Confirmed working for all banking operations." },
  { name: "Varrock East Bank", x: 3253, z: 3420, area: "Varrock", notes: "Near rune shop" },
  { name: "Edgeville Bank", x: 3094, z: 3491, area: "Edgeville", notes: "Near furnace and yew trees" },
  { name: "Falador West Bank", x: 2946, z: 3368, area: "Falador", notes: "Major city bank" },
  { name: "Falador East Bank", x: 3013, z: 3355, area: "Falador", notes: "Near mining guild" },
];

/** Find nearest bank to given coordinates */
export function findNearestBank(x: number, z: number): { bank: BankLocation; distance: number } {
  let nearest = BANK_LOCATIONS[0];
  let bestDist = Infinity;
  for (const bank of BANK_LOCATIONS) {
    const dist = Math.sqrt((x - bank.x) ** 2 + (z - bank.z) ** 2);
    if (dist < bestDist) {
      bestDist = dist;
      nearest = bank;
    }
  }
  return { bank: nearest, distance: Math.round(bestDist) };
}

/** Find all banks within a given distance */
export function findBanksNearby(x: number, z: number, maxDistance: number = 100): { bank: BankLocation; distance: number }[] {
  return BANK_LOCATIONS
    .map(bank => ({ bank, distance: Math.round(Math.sqrt((x - bank.x) ** 2 + (z - bank.z) ** 2)) }))
    .filter(b => b.distance <= maxDistance)
    .sort((a, b) => a.distance - b.distance);
}
