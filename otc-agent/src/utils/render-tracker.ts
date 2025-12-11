/**
 * Development-only render tracking utility.
 * Tracks component renders and throws if a component renders too many times
 * in a short time window, indicating a render loop or performance issue.
 */

type RenderRecord = {
  count: number;
  timestamps: number[];
  firstRenderTime: number;
  lastProps?: Record<string, string>;
  lastState?: Record<string, string>;
};

const renderCounts = new Map<string, RenderRecord>();

// Configuration
const CONFIG = {
  /** Maximum renders allowed in the time window before throwing */
  maxRenders: 10,
  /** Maximum renders allowed during initial mount period (higher to allow for StrictMode + library init) */
  maxRendersInitialMount: 20,
  /** Time window in milliseconds to track renders */
  timeWindowMs: 1000,
  /** Initial mount grace period in milliseconds (allow more renders during startup) */
  initialMountGracePeriodMs: 2000,
  /** Enable verbose logging of every render */
  verboseLogging: false,
  /** Components to ignore (some are expected to render frequently) */
  ignoredComponents: new Set<string>([
    // Providers re-render during initialization due to multiple useEffects + StrictMode
    "MultiWalletProvider",
    "SolanaConnectionProvider",
    // TokenGroupLoader renders per token during grid load - expected behavior
    "TokenGroupLoader",
  ]),
};

/**
 * Get a serializable summary of a value for debugging
 */
function summarizeValue(value: unknown): string {
  if (value === null) return "null";
  if (value === undefined) return "undefined";
  if (typeof value === "function") return `fn:${value.name || "anonymous"}`;
  if (typeof value === "object") {
    if (Array.isArray(value)) return `array[${value.length}]`;
    const keys = Object.keys(value);
    if (keys.length === 0) return "{}";
    if (keys.length <= 3) return `{${keys.join(",")}}`;
    return `{${keys.slice(0, 3).join(",")}...+${keys.length - 3}}`;
  }
  if (typeof value === "string") {
    return value.length > 20 ? `"${value.slice(0, 20)}..."` : `"${value}"`;
  }
  return String(value);
}

/**
 * Get a props summary for debugging
 */
function getPropsSnapshot(
  props: Record<string, unknown>,
): Record<string, string> {
  const snapshot: Record<string, string> = {};
  for (const [key, value] of Object.entries(props)) {
    if (key === "children") {
      snapshot[key] = typeof value === "object" ? "ReactNode" : String(value);
    } else {
      snapshot[key] = summarizeValue(value);
    }
  }
  return snapshot;
}

/**
 * Find what changed between two snapshots
 */
function findChanges(
  prev: Record<string, string> | undefined,
  curr: Record<string, string>,
): string[] {
  if (!prev) return ["(first render)"];
  const changes: string[] = [];

  const allKeys = new Set([...Object.keys(prev), ...Object.keys(curr)]);
  for (const key of allKeys) {
    if (prev[key] !== curr[key]) {
      changes.push(`${key}: ${prev[key]} → ${curr[key]}`);
    }
  }

  return changes.length > 0 ? changes : ["(no prop changes detected)"];
}

/**
 * Track a component render. Call this at the top of your component.
 * In development, throws an error if the component renders excessively.
 *
 * @param componentName - Unique identifier for the component
 * @param props - Component props (optional, for debugging)
 * @param state - Component state snapshot (optional, for debugging)
 *
 * @example
 * ```tsx
 * function MyComponent(props: MyProps) {
 *   useRenderTracker("MyComponent", props);
 *   // ... rest of component
 * }
 * ```
 */
export function trackRender(
  componentName: string,
  props?: Record<string, unknown>,
  state?: Record<string, unknown>,
): void {
  // Only run in development
  if (process.env.NODE_ENV !== "development") return;

  // Skip ignored components
  if (CONFIG.ignoredComponents.has(componentName)) return;

  const now = Date.now();
  let record = renderCounts.get(componentName);

  if (!record) {
    record = { count: 0, timestamps: [], firstRenderTime: now };
    renderCounts.set(componentName, record);
  }

  // Clean up old timestamps outside the window
  record.timestamps = record.timestamps.filter(
    (ts) => now - ts < CONFIG.timeWindowMs,
  );

  // Add current timestamp
  record.timestamps.push(now);
  record.count++;

  // Get current snapshots
  const propsSnapshot = props ? getPropsSnapshot(props) : undefined;
  const stateSnapshot = state ? getPropsSnapshot(state) : undefined;

  // Check for excessive renders
  const recentRenders = record.timestamps.length;

  // Determine if we're in initial mount grace period
  const timeSinceFirstRender = now - record.firstRenderTime;
  const isInitialMount =
    timeSinceFirstRender < CONFIG.initialMountGracePeriodMs;
  const maxAllowed = isInitialMount
    ? CONFIG.maxRendersInitialMount
    : CONFIG.maxRenders;

  if (CONFIG.verboseLogging) {
    const propsChanges = propsSnapshot
      ? findChanges(record.lastProps, propsSnapshot)
      : [];
    const stateChanges = stateSnapshot
      ? findChanges(record.lastState, stateSnapshot)
      : [];
    console.log(
      `[RenderTracker] ${componentName} render #${record.count} (${recentRenders} in ${CONFIG.timeWindowMs}ms)`,
      { propsChanges, stateChanges, isInitialMount },
    );
  }

  if (recentRenders > maxAllowed) {
    const propsChanges = propsSnapshot
      ? findChanges(record.lastProps, propsSnapshot)
      : [];
    const stateChanges = stateSnapshot
      ? findChanges(record.lastState, stateSnapshot)
      : [];

    const error = new Error(
      `[RenderTracker] EXCESSIVE RENDERS DETECTED: ${componentName} rendered ${recentRenders} times in ${CONFIG.timeWindowMs}ms.\n\n` +
        `This indicates a render loop or severe performance issue.\n\n` +
        `Recent prop changes: ${propsChanges.join(", ")}\n` +
        `Recent state changes: ${stateChanges.join(", ")}\n\n` +
        `Common causes:\n` +
        `  1. Object/array created in render passed as prop (use useMemo)\n` +
        `  2. Callback created in render passed as prop (use useCallback)\n` +
        `  3. State update in useEffect without proper deps\n` +
        `  4. Context value changing on every render\n` +
        `  5. Missing dependency in useEffect causing infinite loop`,
    );

    // Log detailed info before throwing
    console.error(error.message);
    console.error("Component:", componentName);
    console.error("Props snapshot:", propsSnapshot);
    console.error("State snapshot:", stateSnapshot);
    console.error(
      "Render timestamps:",
      record.timestamps.map((ts) => new Date(ts).toISOString()),
    );
    console.error("Time since first render:", timeSinceFirstRender, "ms");
    console.error("Is initial mount period:", isInitialMount);

    throw error;
  }

  // Update snapshots for next comparison
  record.lastProps = propsSnapshot;
  record.lastState = stateSnapshot;
}

/**
 * React hook version of trackRender for easier integration.
 *
 * @example
 * ```tsx
 * function MyComponent(props: MyProps) {
 *   useRenderTracker("MyComponent", props);
 *   const [state, setState] = useState({});
 *   // ... rest of component
 * }
 * ```
 */
export function useRenderTracker(
  componentName: string,
  props?: Record<string, unknown>,
  state?: Record<string, unknown>,
): void {
  trackRender(componentName, props, state);
}
