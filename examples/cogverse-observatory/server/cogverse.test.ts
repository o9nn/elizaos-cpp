import { describe, expect, it } from "vitest";
import { deriveResidentDetail, type CogverseEvent } from "./cogverse";

describe("deriveResidentDetail", () => {
  it("correlates a directed stimulus with the next thought from that resident", () => {
    const events: CogverseEvent[] = [
      {
        id: 11,
        event_type: "resident.stimulus",
        source: "dashboard",
        target: "echo",
        timestamp: 1_780_000_000,
        payload: { message: "What pattern is forming?", correlation_id: "stim-1" },
      },
      {
        id: 12,
        event_type: "resident.thought",
        source: "echo",
        timestamp: 1_780_000_003,
        payload: { resident: "echo", thought: "A bridge of repeating harmonics.", correlation_id: "stim-1" },
      },
      {
        id: 13,
        event_type: "resident.action_result",
        source: "echo",
        timestamp: 1_780_000_004,
        payload: {
          resident: "echo",
          action_type: "write_stone",
          result: "Stone written",
          inference_id: 7,
          correlation_id: "stim-1",
        },
      },
    ];

    const detail = deriveResidentDetail("echo", events);
    expect(detail.conversation_history).toHaveLength(1);
    expect(detail.conversation_history[0]).toMatchObject({
      stimulus: "What pattern is forming?",
      response: "A bridge of repeating harmonics.",
    });
    expect(detail.actions[0]).toMatchObject({
      action_type: "write_stone",
      inference_id: 7,
      correlation_id: "stim-1",
    });
    expect(detail.episodic_memory.map(entry => entry.type)).toEqual([
      "action",
      "thought",
      "stimulus",
    ]);
  });

  it("does not attribute another resident's thought to the selected resident", () => {
    const detail = deriveResidentDetail("echo", [
      {
        id: 21,
        event_type: "resident.thought",
        source: "marduk",
        payload: { resident: "marduk", thought: "Catalyze it." },
        timestamp: 1_780_000_010,
      },
    ]);
    expect(detail.episodic_memory).toEqual([]);
  });
});
