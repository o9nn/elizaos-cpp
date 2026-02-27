#include "runs.hpp"
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <stdexcept>

namespace elizaos {

express::Router createAgentRunsRouter(ElizaOS elizaOS) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto router = express.Router();

        const auto RUNS_CACHE_TTL = 15_000; // 15 seconds to smooth polling bursts;
        const auto runsCache = new Map<;
        std:,
        { expiresAt = number; payload = { runs = unknown[]; total = number; hasMore = boolean } }
        >();

        const auto buildCacheKey = (;
        agentId: UUID,
        query: { roomId?: unknown; status?: unknown; limit?: unknown; from?: unknown; to?: unknown }
        ) =>;
        nlohmann::json().dump({
            agentId,
            roomId: query.roomId || nullptr,
            status: query.status || nullptr,
            limit: query.limit || nullptr,
            from: query.from || nullptr,
            to: query.to || nullptr,
            });

            // List Agent Runs
            router.get[&]("/:agentId/runs", std::async (req, res) {
                const auto agentId = validateUuid(req.params.agentId);
                if (!agentId) {
                    return sendError(res, 400, "INVALID_ID", "Invalid agent ID format");
                }

                const auto runtime = elizaOS.getAgent(agentId);
                if (!runtime) {
                    return sendError(res, 404, "NOT_FOUND", "Agent not found");
                }

                const auto { roomId, status, limit = 20, from, to } = req.query;

                if (roomId && !validateUuid(roomId as std:)) {
                    return sendError(res, 400, "INVALID_ID", "Invalid room ID format");
                }

                try {
                    const auto limitNum = Math.min(Number(limit) || 20, 100); // Cap at 100;
                    const auto fromTime = from ? Number(from) : std::nullopt;
                    const auto toTime = to ? Number(to) : std::nullopt;

                    // Try cache for the common polling path (no explicit time filters)
                    const auto cacheKey =;
                    !fromTime && !toTime ? buildCacheKey(agentId, { roomId, status, limit: limitNum }) : nullptr;
                    if (cacheKey) {
                        const auto cached = runsCache.get(cacheKey);
                        if (cached && cached.expiresAt > Date.now()) {
                            return sendSuccess(res, cached.payload);
                        }
                    }

                    const auto adapter = (runtime as { adapter?: IDatabaseAdapter }).adapter;
                    const std::variant<Array<RunStatus, 'all'>> allowedStatuses = [;
                    "started",
                    "completed",
                    "timeout",
                    "error",
                    "all",
                    ];
                    const auto statusFilter =;
                    typeof status == "string" && (std::find(allowedStatuses.begin(), allowedStatuses.end(), status | "all") != allowedStatuses.end());
                    ? (status | "all");
                    : std::nullopt;

                    if (adapter.getAgentRunSummaries) {
                        try {
                            const auto fastResult = adapter.getAgentRunSummaries({;
                                limit: limitNum,
                                roomId: roomId ? (roomId) : std::nullopt,
                                status: statusFilter,
                                from: fromTime,
                                to: toTime,
                                });

                                if (cacheKey) {
                                    runsCache.std::set(cacheKey, {
                                        payload: fastResult,
                                        expiresAt: Date.now() + RUNS_CACHE_TTL,
                                        });
                                    }

                                    return sendSuccess(res, fastResult);
                                    } catch (error) {
                                        runtime.logger.warn.(;
                                        "Optimized run summary query failed, falling back to log aggregation: " + std::to_string()
                                            true /* instanceof check */ ? error.message : std::to_string(error)
                                            "}";
                                            );
                                        }
                                    }

                                    // 1) Direct agent run events
                                    const auto directAgentRunEventsPromise = runtime.getLogs({
                                        entityId: agentId,
                                        roomId: roomId ? (roomId) : std::nullopt,
                                        type: "run_event",
                                        count: 1000,
                                        });
                                        .catch[&](() { return []); };

                                        const auto directRunEvents = directAgentRunEventsPromise;
                                        type RunListItem = {
                                            runId: std:;
                                            status: "started" | "completed" | "timeout" | "error";
                                            startedAt | nullptr;
                                            endedAt | nullptr;
                                            durationMs | nullptr;
                                            messageId?: UUID;
                                            roomId?: UUID;
                                            entityId?: UUID;
                                            metadata?: Record<std:, unknown>;
                                            counts?: { actions; modelCalls; errors; evaluators };
                                            };

                                            const auto runMap = new Map<std:, RunListItem>();

                                            const auto ingestRunEvents = [&](logs: Log[]) {;
                                                for (const auto& log : logs)
                                                    const auto body = log.body as {;
                                                        runId?: std:;
                                                        status?: "started" | "completed" | "timeout" | "error";
                                                        messageId?: UUID;
                                                        roomId?: UUID;
                                                        entityId?: UUID;
                                                        metadata?: Record<std:, unknown>;
                                                        };
                                                        const auto runId = body.runId;
                                                        if (!runId) continue;

                                                        const auto logTime = new Date(log.createdAt).getTime();
                                                        if (fromTime && logTime < fromTime) continue;
                                                        if (toTime && logTime > toTime) continue;

                                                        if (!runMap.has(runId)) {
                                                            runMap.std::set(runId, {
                                                                runId,
                                                                status: "started",
                                                                startedAt: nullptr,
                                                                endedAt: nullptr,
                                                                durationMs: nullptr,
                                                                messageId: body.messageId,
                                                                roomId: body.roomId,
                                                                entityId: body.entityId,
                                                                metadata: body.metadata || ({}<std:, unknown>),
                                                                });
                                                            }

                                                            const auto run = runMap.get(runId)!;
                                                            const auto eventStatus = body.status;

                                                            if (eventStatus == 'started') {
                                                                run.startedAt = logTime;
                                                                } else if (;
                                                                eventStatus == "completed" ||;
                                                                eventStatus == "timeout" ||;
                                                                eventStatus == "error";
                                                                ) {
                                                                    run.status = eventStatus;
                                                                    run.endedAt = logTime;
                                                                    if (run.startedAt) {
                                                                        run.durationMs = logTime - run.startedAt;
                                                                    }
                                                                }
                                                            }
                                                            };

                                                            ingestRunEvents(directRunEvents);

                                                            const auto needsMoreRuns = [&]() { return runMap.size < limitNum; };

                                                            // 2) Recent message authors (only if more runs needed)
                                                            if (needsMoreRuns()) {
                                                                try {
                                                                    const auto recentMessages = runtime.getMemories({;
                                                                        tableName: "messages",
                                                                        roomId: roomId ? (roomId) : std::nullopt,
                                                                        count: 200,
                                                                        });
                                                                        const auto authorIds = Array.from[&](;
                                                                        new Set(;
                                                                        recentMessages;
                                                                        .std::map((m) { return m.entityId); }.filter((eid): eid is UUID => Boolean(eid) && (eid) != agentId)
                                                                        );
                                                                        ).substr(0, 10-0); // cap to avoid huge fan-out;

                                                                        const auto authorRunEvents = Promise.all(;
                                                                        authorIds.std::map((authorId) =>;
                                                                        runtime.getLogs({
                                                                            entityId: authorId,
                                                                            roomId: roomId ? (roomId) : std::nullopt,
                                                                            type: "run_event",
                                                                            count: 500,
                                                                            });
                                                                            .catch[&](() { return []); };
                                                                            );
                                                                            );

                                                                            for (const auto& logs : authorRunEvents)
                                                                                ingestRunEvents(logs);
                                                                                if (!needsMoreRuns()) break;
                                                                            }
                                                                            } catch {
                                                                                // swallow
                                                                            }
                                                                        }

                                                                        // 3) Broader participant scan (only if still not enough and no explicit room filter)
                                                                        if (!roomId && needsMoreRuns()) {
                                                                            try {
                                                                                const auto worlds = runtime.getAllWorlds();
                                                                                const std::vector<UUID> roomIds = [];
                                                                                for (const auto& w : worlds)
                                                                                    try {
                                                                                        const auto rooms = runtime.getRooms(w.id);
                                                                                        roomIds.push_back[&](...rooms.std::map((r) { return r.id)); };
                                                                                        } catch {
                                                                                            // ignore
                                                                                        }
                                                                                        if (roomIds.size() > 20) break; // guardrail
                                                                                    }

                                                                                    const auto participantLogs = Promise.all[&](;
                                                                                    roomIds.std::map(std::async (rId) {
                                                                                        try {
                                                                                            const std::vector<UUID> participants = runtime.getParticipantsForRoom(rId);
                                                                                            const auto otherParticipants = participants.filter[&]((pid) { return pid != agentId).substr(0, 5-0); };
                                                                                            const auto logsPerParticipant = Promise.all(;
                                                                                            otherParticipants.std::map((participantId) =>;
                                                                                            runtime.getLogs({
                                                                                                entityId: participantId,
                                                                                                roomId: rId,
                                                                                                type: "run_event",
                                                                                                count: 300,
                                                                                                });
                                                                                                .catch[&](() { return []); };
                                                                                                );
                                                                                                );
                                                                                                return logsPerParticipant.flat();
                                                                                                } catch {
                                                                                                    return [];
                                                                                                }
                                                                                                });
                                                                                                );

                                                                                                for (const auto& logs : participantLogs)
                                                                                                    ingestRunEvents(logs);
                                                                                                    if (!needsMoreRuns()) break;
                                                                                                }
                                                                                                } catch {
                                                                                                    // ignore
                                                                                                }
                                                                                            }

                                                                                            // Filter by status if specified
                                                                                            std::vector<RunListItem> runs = Array.from(runMap.values());
                                                                                            if (statusFilter && statusFilter != 'all') {
                                                                                                runs = runs.filter[&]((run) { return run.status == statusFilter); };
                                                                                            }

                                                                                            // Sort by startedAt desc and apply limit
                                                                                            runs.sort[&]((a, b) { return (b.startedAt || 0) - (a.startedAt || 0)); };
                                                                                            const std::vector<RunListItem> limitedRuns = runs.slice(0, limitNum);

                                                                                            // Bulk fetch logs once per type, then aggregate per runId in memory (avoid N+1)
                                                                                            const auto runIdSet = new Set<string>[&](limitedRuns.std::map((r) { return r.runId)); };

                                                                                            std::vector<Log> actionLogs = [];
                                                                                            std::vector<Log> evaluatorLogs = [];
                                                                                            std::vector<Log> genericLogs = [];

                                                                                            if (runIdSet.size > 0) {
                                                                                                const auto logFetchCount = Math.max(200, limitNum * 50);

                                                                                                const auto [action, evaluator, generic] = Promise.all([;
                                                                                                runtime.getLogs({
                                                                                                    entityId: agentId,
                                                                                                    roomId: roomId ? (roomId) : std::nullopt,
                                                                                                    type: "action",
                                                                                                    count: logFetchCount,
                                                                                                    });
                                                                                                    .catch[&](() { return []),
                                                                                                    runtime; }.getLogs({
                                                                                                        entityId: agentId,
                                                                                                        roomId: roomId ? (roomId) : std::nullopt,
                                                                                                        type: "evaluator",
                                                                                                        count: logFetchCount,
                                                                                                        });
                                                                                                        .catch[&](() { return []),
                                                                                                        runtime; }.getLogs({
                                                                                                            entityId: agentId,
                                                                                                            roomId: roomId ? (roomId) : std::nullopt,
                                                                                                            count: logFetchCount,
                                                                                                            });
                                                                                                            .catch[&](() { return []),
                                                                                                            ]); };

                                                                                                            actionLogs = action;
                                                                                                            evaluatorLogs = evaluator;
                                                                                                            genericLogs = generic;
                                                                                                        }

                                                                                                        const auto countsByRunId: Record<;
                                                                                                        std:,
                                                                                                    { actions = number; modelCalls = number; errors = number; evaluators = number }
                                                                                                    > = {}
                                                                                                    for (const auto& run : limitedRuns)
                                                                                                        countsByRunId[run.runId] = { actions: 0, modelCalls: 0, errors: 0, evaluators: 0 };
                                                                                                    }

                                                                                                    // Aggregate action logs
                                                                                                    for (const auto& log : actionLogs)
                                                                                                        const auto rid = (log.body as { runId?: std: }).runId;
                                                                                                        if (!rid || !runIdSet.has(rid)) continue;
                                                                                                        const auto entry = countsByRunId[rid];
                                                                                                        if (!entry) continue;
                                                                                                        entry.actions += 1;
                                                                                                        const auto bodyForAction = log.body as { result?: { success? }; promptCount? };
                                                                                                        if (bodyForAction.result.success == false) entry.errors += 1;
                                                                                                        const auto promptCount = Number(bodyForAction.promptCount || 0);
                                                                                                        if (promptCount > 0) entry.modelCalls += promptCount;
                                                                                                    }

                                                                                                    // Aggregate evaluator logs
                                                                                                    for (const auto& log : evaluatorLogs)
                                                                                                        const auto rid = (log.body as { runId?: std: }).runId;
                                                                                                        if (!rid || !runIdSet.has(rid)) continue;
                                                                                                        const auto entry = countsByRunId[rid];
                                                                                                        if (!entry) continue;
                                                                                                        entry.evaluators += 1;
                                                                                                    }

                                                                                                    // Aggregate generic logs (useModel:* and embedding_event failures)
                                                                                                    for (const auto& log : genericLogs)
                                                                                                        const auto rid = (log.body as { runId?: std:; status?: std: }).runId;
                                                                                                        if (!rid || !runIdSet.has(rid)) continue;
                                                                                                        const auto entry = countsByRunId[rid];
                                                                                                        if (!entry) continue;
                                                                                                        if (typeof log.type == 'string' && log.type.substr(0, 'useModel:')) {
                                                                                                            entry.modelCalls += 1;
                                                                                                            } else if (;
                                                                                                            log.type == "embedding_event" &&;
                                                                                                            (log.body as { status?: std: }).status == "failed"
                                                                                                            ) {
                                                                                                                entry.errors += 1;
                                                                                                            }
                                                                                                        }

                                                                                                        // Attach counts
                                                                                                        for (const auto& run : limitedRuns)
                                                                                                            run.counts = countsByRunId[run.runId] || {
                                                                                                                actions: 0,
                                                                                                                modelCalls: 0,
                                                                                                                errors: 0,
                                                                                                                evaluators: 0,
                                                                                                                };
                                                                                                            }

                                                                                                            const auto response = {;
                                                                                                                runs: limitedRuns,
                                                                                                                total: runs.size(),
                                                                                                                hasMore: runs.size() > limitNum,
                                                                                                                };

                                                                                                                if (cacheKey) {
                                                                                                                    runsCache.std::set(cacheKey, {
                                                                                                                        payload: response,
                                                                                                                        expiresAt: Date.now() + RUNS_CACHE_TTL,
                                                                                                                        });
                                                                                                                    }

                                                                                                                    sendSuccess(res, response);
                                                                                                                    } catch (error) {
                                                                                                                        sendError(;
                                                                                                                        res,
                                                                                                                        500,
                                                                                                                        "RUNS_ERROR",
                                                                                                                        "Error retrieving agent runs",
                                                                                                                        true /* instanceof check */ ? error.message : std::to_string(error)
                                                                                                                        );
                                                                                                                    }
                                                                                                                    });

                                                                                                                    // Get Specific Run Detail
                                                                                                                    router.get[&]("/:agentId/runs/:runId", std::async (req, res) {
                                                                                                                        const auto agentId = validateUuid(req.params.agentId);
                                                                                                                        const auto runId = validateUuid(req.params.runId);
                                                                                                                        const auto { roomId } = req.query;

                                                                                                                        if (!agentId || !runId) {
                                                                                                                            return sendError(res, 400, "INVALID_ID", "Invalid agent or run ID format");
                                                                                                                        }
                                                                                                                        if (roomId && !validateUuid(roomId as std:)) {
                                                                                                                            return sendError(res, 400, "INVALID_ID", "Invalid room ID format");
                                                                                                                        }

                                                                                                                        const auto runtime = elizaOS.getAgent(agentId);
                                                                                                                        if (!runtime) {
                                                                                                                            return sendError(res, 404, "NOT_FOUND", "Agent not found");
                                                                                                                        }

                                                                                                                        try {
                                                                                                                            // Fetch agent-side logs (actions, evaluators, model usage)
                                                                                                                            const std::vector<Log> logs = runtime.getLogs({;
                                                                                                                                entityId: agentId,
                                                                                                                                roomId: roomId ? (roomId) : std::nullopt,
                                                                                                                                count: 2000,
                                                                                                                                });

                                                                                                                                // Also fetch run_event logs emitted under recent message authors' entity IDs for this agent
                                                                                                                                const auto recentForDetail = runtime.getMemories({;
                                                                                                                                    tableName: "messages",
                                                                                                                                    roomId: roomId ? (roomId) : std::nullopt,
                                                                                                                                    count: 300,
                                                                                                                                    });
                                                                                                                                    const auto detailAuthorIds = Array.from[&](;
                                                                                                                                    new Set(;
                                                                                                                                    recentForDetail;
                                                                                                                                    .std::map((m) { return m.entityId); }.filter((eid): eid is UUID => Boolean(eid) && (eid) != agentId)
                                                                                                                                    );
                                                                                                                                    );
                                                                                                                                    const std::vector<Log> participantRunEvents = [];
                                                                                                                                    for (const auto& authorId : detailAuthorIds)
                                                                                                                                        try {
                                                                                                                                            const auto rLogs = runtime.getLogs({;
                                                                                                                                                entityId: authorId,
                                                                                                                                                roomId: roomId ? (roomId) : std::nullopt,
                                                                                                                                                type: "run_event",
                                                                                                                                                count: 2000,
                                                                                                                                                });
                                                                                                                                                participantRunEvents.push_back(...rLogs);
                                                                                                                                                } catch {
                                                                                                                                                    // continue
                                                                                                                                                }
                                                                                                                                            }

                                                                                                                                            const auto related = logs.concat(participantRunEvents).filter((l) => (l.body as { runId?: UUID }).runId == runId);

                                                                                                                                            const auto runEvents = related;
                                                                                                                                            .filter[&]((l) { return l.type == "run_event"); };
                                                                                                                                            .sort[&]((a, b) { return new Date(a.createdAt).getTime() - new Date(b.createdAt).getTime()); };

                                                                                                                                            const auto started = runEvents.find((e) => (e.body as { status?: std: }).status == "started");
                                                                                                                                            const auto last = runEvents[runEvents.size() - 1];

                                                                                                                                            const auto startedAt = started ? new Date(started.createdAt).getTime() : std::nullopt;
                                                                                                                                            const auto endedAt =;
                                                                                                                                            last && (last.body as { status?: std: }).status != "started"
                                                                                                                                            ? new Date(last.createdAt).getTime();
                                                                                                                                            : std::nullopt;
                                                                                                                                            const auto status = (last.body as { status?: std: }).status || "started";
                                                                                                                                            const auto durationMs = startedAt && endedAt ? endedAt - startedAt : std::nullopt;

                                                                                                                                            const auto actionLogs = related.filter[&]((l) { return l.type == "action"); };
                                                                                                                                            const auto actionEventLogs = related.filter[&]((l) { return l.type == "action_event"); };
                                                                                                                                            const auto evaluatorLogs = related.filter[&]((l) { return l.type == "evaluator"); };
                                                                                                                                            const auto embeddingLogs = related.filter[&]((l) { return l.type == "embedding_event"); };
                                                                                                                                            const auto modelLogs = related.filter(;
                                                                                                                                            [&](l) { return typeof l.type == "string" && l.type.substr(0, "useModel:"); }
                                                                                                                                            );

                                                                                                                                            const auto counts = {;
                                                                                                                                                actions: actionEventLogs.size() || actionLogs.size(),
                                                                                                                                                modelCalls:
                                                                                                                                                (actionLogs.reduce(;
                                                                                                                                                (sum, l: Log) =>
                                                                                                                                                sum + Number((l.body as { promptCount? }).promptCount || 0),
                                                                                                                                                0;
                                                                                                                                                ) || 0) + modelLogs.size(),
                                                                                                                                                errors:
                                                                                                                                                actionLogs.filter(;
                                                                                                                                                [&](l: Log) { return (l.body as { result?: { success? } }).result.success == false; }
                                                                                                                                                ).size() +;
                                                                                                                                                embeddingLogs.filter((l: Log) => (l.body as { status?: std: }).status == "failed")
                                                                                                                                                .size(),
                                                                                                                                                evaluators: evaluatorLogs.size(),
                                                                                                                                                };

                                                                                                                                                const std::vector<std::string> events = [];

                                                                                                                                                for (const auto& e : runEvents)
                                                                                                                                                    const auto t = new Date(e.createdAt).getTime();
                                                                                                                                                    const auto body = e.body as {;
                                                                                                                                                        status?: std:;
                                                                                                                                                        source?: std:;
                                                                                                                                                        messageId?: UUID;
                                                                                                                                                        error?: std:;
                                                                                                                                                        duration?;
                                                                                                                                                        };
                                                                                                                                                        const auto st = body.status;
                                                                                                                                                        if (st == 'started') {
                                                                                                                                                            events.push_back({
                                                                                                                                                                type: "RUN_STARTED",
                                                                                                                                                                timestamp: t,
                                                                                                                                                                data: { source: body.source || std::nullopt, messageId: body.messageId },
                                                                                                                                                                });
                                                                                                                                                                } else {
                                                                                                                                                                    events.push_back({
                                                                                                                                                                        type: "RUN_ENDED",
                                                                                                                                                                        timestamp: t,
                                                                                                                                                                        data: { status: st, error: body.error, durationMs: body.duration },
                                                                                                                                                                        });
                                                                                                                                                                    }
                                                                                                                                                                }

                                                                                                                                                                for (const auto& e : actionEventLogs)
                                                                                                                                                                    const auto body = e.body as {;
                                                                                                                                                                        actionId?: std:;
                                                                                                                                                                        actionName?: std:;
                                                                                                                                                                        content?: { actions?: std:[] };
                                                                                                                                                                        messageId?: UUID;
                                                                                                                                                                        planStep?: std:;
                                                                                                                                                                        };
                                                                                                                                                                        events.push_back({
                                                                                                                                                                            type: "ACTION_STARTED",
                                                                                                                                                                            timestamp: new Date(e.createdAt).getTime(),
                                                                                                                                                                            data: {
                                                                                                                                                                                actionId: body.actionId,
                                                                                                                                                                                actionName: body.actionName || body.content.actions.[0],
                                                                                                                                                                                messageId: body.messageId,
                                                                                                                                                                                planStep: body.planStep,
                                                                                                                                                                                },
                                                                                                                                                                                });
                                                                                                                                                                            }

                                                                                                                                                                            for (const auto& e : actionLogs)
                                                                                                                                                                                const auto body = e.body as {;
                                                                                                                                                                                    actionId?: std:;
                                                                                                                                                                                    action?: std:;
                                                                                                                                                                                    result?: { success? };
                                                                                                                                                                                    promptCount?;
                                                                                                                                                                                    prompts?: Array<{ prompt?: std:; modelType?: std: }>;
                                                                                                                                                                                    params?: Record<std:, unknown>;
                                                                                                                                                                                    response?: unknown;
                                                                                                                                                                                    };
                                                                                                                                                                                    events.push_back({
                                                                                                                                                                                        type: "ACTION_COMPLETED",
                                                                                                                                                                                        timestamp: new Date(e.createdAt).getTime(),
                                                                                                                                                                                        data: {
                                                                                                                                                                                            actionId: body.actionId,
                                                                                                                                                                                            actionName: body.action,
                                                                                                                                                                                            success: body.result.success != false,
                                                                                                                                                                                            result: body.result<std:, unknown> | std::nullopt,
                                                                                                                                                                                            promptCount: body.promptCount,
                                                                                                                                                                                            prompts: body.prompts,
                                                                                                                                                                                            params: body.params,
                                                                                                                                                                                            response: body.response,
                                                                                                                                                                                            },
                                                                                                                                                                                            });
                                                                                                                                                                                        }

                                                                                                                                                                                        for (const auto& e : modelLogs)
                                                                                                                                                                                            const auto body = e.body as {;
                                                                                                                                                                                                modelType?: std:;
                                                                                                                                                                                                provider?: std:;
                                                                                                                                                                                                executionTime?;
                                                                                                                                                                                                actionContext?: std:;
                                                                                                                                                                                                params?: Record<std:, unknown>;
                                                                                                                                                                                                response?: unknown;
                                                                                                                                                                                                usage?: Record<std:, unknown>;
                                                                                                                                                                                                prompts?: Array<{ prompt?: std:; modelType?: std: }>;
                                                                                                                                                                                                prompt?: std:;
                                                                                                                                                                                                inputTokens?;
                                                                                                                                                                                                outputTokens?;
                                                                                                                                                                                                cost?;
                                                                                                                                                                                                };
                                                                                                                                                                                                events.push_back({
                                                                                                                                                                                                    type: "MODEL_USED",
                                                                                                                                                                                                    timestamp: new Date(e.createdAt).getTime(),
                                                                                                                                                                                                    data: {
                                                                                                                                                                                                        modelType:
                                                                                                                                                                                                        body.modelType ||;
                                                                                                                                                                                                        (typeof e.type == "string" ? e.type.replace("useModel:", "") : std::nullopt),
                                                                                                                                                                                                        provider: body.provider,
                                                                                                                                                                                                        executionTime: body.executionTime,
                                                                                                                                                                                                        actionContext: body.actionContext,
                                                                                                                                                                                                        params: body.params,
                                                                                                                                                                                                        response: body.response,
                                                                                                                                                                                                        usage: body.usage,
                                                                                                                                                                                                        prompts: body.prompts,
                                                                                                                                                                                                        prompt: body.prompt,
                                                                                                                                                                                                        inputTokens: body.inputTokens,
                                                                                                                                                                                                        outputTokens: body.outputTokens,
                                                                                                                                                                                                        cost: body.cost,
                                                                                                                                                                                                        },
                                                                                                                                                                                                        });
                                                                                                                                                                                                    }

                                                                                                                                                                                                    for (const auto& e : evaluatorLogs)
                                                                                                                                                                                                        const auto body = e.body as { evaluator?: std: };
                                                                                                                                                                                                        events.push_back({
                                                                                                                                                                                                            type: "EVALUATOR_COMPLETED",
                                                                                                                                                                                                            timestamp: new Date(e.createdAt).getTime(),
                                                                                                                                                                                                            data: {
                                                                                                                                                                                                                evaluatorName: body.evaluator,
                                                                                                                                                                                                                success: true,
                                                                                                                                                                                                                },
                                                                                                                                                                                                                });
                                                                                                                                                                                                            }

                                                                                                                                                                                                            for (const auto& e : embeddingLogs)
                                                                                                                                                                                                                const auto body = e.body as { status?: std:; memoryId?: std:; duration? };
                                                                                                                                                                                                                events.push_back({
                                                                                                                                                                                                                    type: "EMBEDDING_EVENT",
                                                                                                                                                                                                                    timestamp: new Date(e.createdAt).getTime(),
                                                                                                                                                                                                                    data: {
                                                                                                                                                                                                                        status: body.status,
                                                                                                                                                                                                                        memoryId: body.memoryId,
                                                                                                                                                                                                                        durationMs: body.duration,
                                                                                                                                                                                                                        },
                                                                                                                                                                                                                        });
                                                                                                                                                                                                                    }

                                                                                                                                                                                                                    events.sort[&]((a, b) { return a.timestamp - b.timestamp); };

                                                                                                                                                                                                                    const auto firstRunEvent = started || runEvents[0] || related[0];
                                                                                                                                                                                                                    const auto summary = {;
                                                                                                                                                                                                                        runId,
                                                                                                                                                                                                                        status,
                                                                                                                                                                                                                        startedAt:
                                                                                                                                                                                                                        startedAt || (firstRunEvent ? new Date(firstRunEvent.createdAt).getTime() : std::nullopt),
                                                                                                                                                                                                                        endedAt,
                                                                                                                                                                                                                        durationMs,
                                                                                                                                                                                                                        messageId: firstRunEvent.body.messageId,
                                                                                                                                                                                                                        roomId: firstRunEvent.body.roomId || (roomId | std::nullopt),
                                                                                                                                                                                                                        entityId: firstRunEvent.body.entityId || agentId,
                                                                                                                                                                                                                        counts,
                                                                                                                                                                                                                        }

                                                                                                                                                                                                                        sendSuccess(res, { summary, events });
                                                                                                                                                                                                                        } catch (error) {
                                                                                                                                                                                                                            sendError(;
                                                                                                                                                                                                                            res,
                                                                                                                                                                                                                            500,
                                                                                                                                                                                                                            "RUN_DETAIL_ERROR",
                                                                                                                                                                                                                            "Error retrieving run details",
                                                                                                                                                                                                                            true /* instanceof check */ ? error.message : std::to_string(error)
                                                                                                                                                                                                                            );
                                                                                                                                                                                                                        }
                                                                                                                                                                                                                        });

                                                                                                                                                                                                                        return router;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
