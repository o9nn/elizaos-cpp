// RunRecorder - Records agent conversations to runs folder
// Creates a folder per conversation with JSONL events, screenshots, and HTML transcript

import { mkdirSync, writeFileSync, appendFileSync, existsSync, readdirSync, statSync } from 'fs';
import { join } from 'path';

export interface RunEvent {
    timestamp: number;
    type: 'system' | 'thinking' | 'action' | 'code' | 'result' | 'error' | 'user_message' | 'state' | 'screenshot' | 'console';
    content: string;
    // Optional fields for specific event types
    screenshot?: string;  // base64 data URL for screenshot events
    state?: object;       // world state snapshot for state events
    // Action event fields
    method?: string;      // method name for action events
    args?: unknown[];     // arguments for action events
    result?: unknown;     // result for action events
    durationMs?: number;  // duration for action events
    delta?: string;       // state delta summary for action events
}

export interface RunMetadata {
    runId: string;
    username: string;
    goal: string;
    startTime: number;
    endTime?: number;
    eventCount: number;
    screenshotCount: number;
    outcome?: 'success' | 'failure' | 'timeout' | 'stall' | 'error';
    outcomeMessage?: string;
}

export interface RunRecorderConfig {
    runsDir?: string;
    screenshotIntervalMs?: number;  // Default: 5000 (5s), set to 0 to disable
}

export class RunRecorder {
    private runsDir: string;
    private runDir: string | null = null;
    private metadata: RunMetadata | null = null;
    private eventCount = 0;
    private screenshotCount = 0;
    private screenshotInterval: ReturnType<typeof setInterval> | null = null;
    private requestScreenshot: (() => void) | null = null;
    private screenshotIntervalMs: number;

    constructor(config: RunRecorderConfig | string = {}) {
        // Support legacy string argument for backwards compatibility
        if (typeof config === 'string') {
            this.runsDir = config;
            this.screenshotIntervalMs = 5000;
        } else {
            this.runsDir = config.runsDir ?? join(__dirname, '..', 'runs');
            this.screenshotIntervalMs = config.screenshotIntervalMs ?? 5000;
        }
        // Ensure runs directory exists
        if (!existsSync(this.runsDir)) {
            mkdirSync(this.runsDir, { recursive: true });
        }
    }

    /**
     * Start recording a new run
     */
    startRun(username: string, goal: string, requestScreenshotFn?: () => void): string {
        const timestamp = new Date().toISOString().replace(/[:.]/g, '-').slice(0, 19);
        const goalSlug = this.slugify(goal).slice(0, 40);
        const runId = `${timestamp}-${username}-${goalSlug}`;

        this.runDir = join(this.runsDir, runId);
        mkdirSync(this.runDir, { recursive: true });
        mkdirSync(join(this.runDir, 'screenshots'), { recursive: true });

        this.metadata = {
            runId,
            username,
            goal,
            startTime: Date.now(),
            eventCount: 0,
            screenshotCount: 0
        };
        this.eventCount = 0;
        this.screenshotCount = 0;
        this.requestScreenshot = requestScreenshotFn || null;

        // Write initial metadata
        this.writeMetadata();

        // Log start event
        this.logEvent({
            timestamp: Date.now(),
            type: 'system',
            content: `Run started: ${goal}`
        });

        // Start periodic screenshot capture
        if (this.requestScreenshot && this.screenshotIntervalMs > 0) {
            this.screenshotInterval = setInterval(() => {
                this.requestScreenshot?.();
            }, this.screenshotIntervalMs);
        }

        console.log(`[RunRecorder] Started recording: ${runId}`);
        return runId;
    }

    /**
     * Stop recording
     */
    stopRun(): void {
        if (!this.runDir || !this.metadata) return;

        // Stop screenshot interval
        if (this.screenshotInterval) {
            clearInterval(this.screenshotInterval);
            this.screenshotInterval = null;
        }

        // Log stop event
        this.logEvent({
            timestamp: Date.now(),
            type: 'system',
            content: 'Run stopped'
        });

        // Update metadata
        this.metadata.endTime = Date.now();
        this.metadata.eventCount = this.eventCount;
        this.metadata.screenshotCount = this.screenshotCount;
        this.writeMetadata();

        console.log(`[RunRecorder] Stopped recording: ${this.metadata.runId}`);
        console.log(`[RunRecorder]   Events: ${this.eventCount}, Screenshots: ${this.screenshotCount}`);

        this.runDir = null;
        this.metadata = null;
    }

    /**
     * Log an event to the JSONL file
     */
    logEvent(event: RunEvent): void {
        if (!this.runDir) return;

        const jsonlPath = join(this.runDir, 'events.jsonl');
        appendFileSync(jsonlPath, JSON.stringify(event) + '\n');
        this.eventCount++;
    }

    /**
     * Save a screenshot
     */
    saveScreenshot(dataUrl: string, label?: string): string | null {
        if (!this.runDir) return null;

        const idx = String(this.screenshotCount).padStart(4, '0');
        const filename = label ? `${idx}-${this.slugify(label)}.png` : `${idx}.png`;
        const filepath = join(this.runDir, 'screenshots', filename);

        // Extract base64 data from data URL
        const base64Data = dataUrl.replace(/^data:image\/png;base64,/, '');
        writeFileSync(filepath, Buffer.from(base64Data, 'base64'));

        // Log screenshot event
        this.logEvent({
            timestamp: Date.now(),
            type: 'screenshot',
            content: filename
        });

        this.screenshotCount++;
        return filename;
    }

    /**
     * Log world state snapshot
     */
    logState(state: object): void {
        this.logEvent({
            timestamp: Date.now(),
            type: 'state',
            content: 'State snapshot',
            state
        });
    }

    /**
     * Log a console message
     */
    logConsole(message: string, level: 'log' | 'warn' | 'error' = 'log'): void {
        this.logEvent({
            timestamp: Date.now(),
            type: 'console',
            content: `[${level}] ${message}`
        });
    }

    /**
     * Log a BotActions method call
     */
    logAction(method: string, args: unknown[], result: unknown, durationMs: number, delta?: string): void {
        this.logEvent({
            timestamp: Date.now(),
            type: 'action',
            content: `${method}(${args.map(a => JSON.stringify(a)).join(', ')})`,
            method,
            args,
            result,
            durationMs,
            delta
        });
    }

    /**
     * Set the run outcome (called before stopRun)
     */
    setOutcome(outcome: RunMetadata['outcome'], message?: string): void {
        if (this.metadata) {
            this.metadata.outcome = outcome;
            this.metadata.outcomeMessage = message;
        }
    }

    /**
     * Check if currently recording
     */
    isRecording(): boolean {
        return this.runDir !== null;
    }

    /**
     * Get current run directory
     */
    getRunDir(): string | null {
        return this.runDir;
    }

    private slugify(text: string): string {
        return text
            .toLowerCase()
            .replace(/[^a-z0-9]+/g, '-')
            .replace(/^-|-$/g, '')
            .slice(0, 50);
    }

    private writeMetadata(): void {
        if (!this.runDir || !this.metadata) return;
        const metaPath = join(this.runDir, 'metadata.json');
        writeFileSync(metaPath, JSON.stringify(this.metadata, null, 2));
    }

    /**
     * List all runs in the runs directory
     */
    listRuns(): RunMetadata[] {
        const runs: RunMetadata[] = [];
        if (!existsSync(this.runsDir)) return runs;

        const dirs = readdirSync(this.runsDir);
        for (const dir of dirs) {
            const metaPath = join(this.runsDir, dir, 'metadata.json');
            if (existsSync(metaPath)) {
                try {
                    const meta = JSON.parse(require('fs').readFileSync(metaPath, 'utf-8'));
                    runs.push(meta);
                } catch {}
            }
        }

        // Sort by start time descending (newest first)
        runs.sort((a, b) => b.startTime - a.startTime);
        return runs;
    }
}
