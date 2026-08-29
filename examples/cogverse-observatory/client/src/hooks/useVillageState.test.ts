import { describe, expect, it } from "vitest";
import { matchesStimulusResponse, type BusEvent, type StimulusTracker } from "./useVillageState";

const tracker: StimulusTracker = {
  correlationId: "stimulus-42",
  target: "echo",
  message: "What do you hear?",
  status: "waiting",
  sentAt: 1_000,
};

describe("matchesStimulusResponse", () => {
  it("accepts only the thought carrying the exact correlation identifier", () => {
    const event: BusEvent = {
      id: 1,
      event_type: "resident.thought",
      source: "echo",
      payload: { resident: "echo", thought: "A pulse.", correlation_id: "stimulus-42" },
    };
    expect(matchesStimulusResponse(event, tracker)).toBe(true);
  });

  it("rejects unrelated autonomous thoughts from the same resident", () => {
    const event: BusEvent = {
      id: 2,
      event_type: "resident.thought",
      source: "echo",
      payload: { resident: "echo", thought: "Background cognition." },
    };
    expect(matchesStimulusResponse(event, tracker)).toBe(false);
  });

  it("rejects a matching identifier emitted by another resident", () => {
    const event: BusEvent = {
      id: 3,
      event_type: "resident.thought",
      source: "marduk",
      payload: { resident: "marduk", thought: "Catalysis.", correlation_id: "stimulus-42" },
    };
    expect(matchesStimulusResponse(event, tracker)).toBe(false);
  });
});
