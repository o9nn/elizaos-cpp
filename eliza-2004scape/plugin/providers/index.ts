import type { Provider } from "@elizaos/core";
import botStateProvider, { addEventLogEntry, getEventLog } from "./botStateProvider";
import mapAreaProvider from "./mapAreaProvider";
import worldKnowledgeProvider from "./worldKnowledgeProvider";
import goalsProvider from "./goalsProvider";

export const rsSdkProviders: Provider[] = [
  mapAreaProvider,         // position 40 - area detection first
  worldKnowledgeProvider,  // position 45 - training/bank knowledge
  goalsProvider,           // position 48 - strategic goals
  botStateProvider,        // position 50 - current game state
];

export { botStateProvider, mapAreaProvider, worldKnowledgeProvider, goalsProvider, addEventLogEntry, getEventLog };
