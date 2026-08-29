import { describe, expect, it } from "vitest";
import { websocketFrameText } from "./cogverseSocket";

describe("websocketFrameText", () => {
  it("converts Buffer-backed upstream JSON into browser-readable text", () => {
    const event = { id: 7, event_type: "resident.thought", payload: { thought: "coherent" } };
    expect(JSON.parse(websocketFrameText(Buffer.from(JSON.stringify(event))))).toEqual(event);
  });

  it("preserves fragmented buffer frames as one UTF-8 JSON document", () => {
    const fragments = [Buffer.from('{"type":"bridge.'), Buffer.from('status","status":"connected"}')];
    expect(websocketFrameText(fragments)).toBe('{"type":"bridge.status","status":"connected"}');
  });
});
