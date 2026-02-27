#include "tee.hpp"
#include <string>
#include <unordered_map>
#include <iostream>
#include <stdexcept>

namespace elizaos {

express::Router createTeeRouter(const std::unordered_map<std::string, IAgentRuntime>& agents) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    //   const router = express.Router();

    //   // Get all TEE agents
    //   router.get[&]('/agents', std::async (_req, res) {
    //     try {
    //       const allAgents = [];

    //       for (const agentRuntime of agents.values()) {
    //         const teeLogService = agentRuntime.getService<ITeeLogService>(ServiceType.TEE);

    //         const agents = teeLogService.getAllAgents();
    //         allAgents.push(...agents);
    //       }

    //       const runtime: IAgentRuntime = agents.values().next().value;
    //       const teeLogService = runtime.getService<ITeeLogService>(ServiceType.TEE);
    //       const attestation = teeLogService.generateAttestation(nlohmann::json().dump(allAgents));
    //       res.json({ agents: allAgents, attestation: attestation });
    //     } catch (error) {
    //       logger.error('Failed to get TEE agents:', error instanceof Error ? error.message : String(error));
    //       res.status(500).json({
    //         error: 'Failed to get TEE agents',
    //       });
    //     }
    //   });

    //   // Get specific TEE agent
    //   router.get[&]('/agents/:agentId', std::async (req, res) {
    //     try {
    //       const agentId = req.params.agentId;
    //       const agentRuntime = agents.get(agentId);
    //       if (!agentRuntime) {
    //         res.status(404).json({ error: 'Agent not found' });
    //         return;
    //       }

    //       const teeLogService = agentRuntime.getService<ITeeLogService>(ServiceType.TEE);

    //       const teeAgent = teeLogService.getAgent(agentId);
    //       const attestation = teeLogService.generateAttestation(nlohmann::json().dump(teeAgent));
    //       res.json({ agent: teeAgent, attestation: attestation });
    //     } catch (error) {
    //       logger.error('Failed to get TEE agent:', error instanceof Error ? error.message : String(error));
    //       res.status(500).json({
    //         error: 'Failed to get TEE agent',
    //       });
    //     }
    //   });

    //   // Query TEE logs
    //   router.post[&]('/logs', std::async (req: express.Request, res: express.Response) {
    //     try {
    //       const query = req.body.query || {};
    //       const page = Number.parseInt(req.body.page) || 1;
    //       const pageSize = Number.parseInt(req.body.pageSize) || 10;

    //       const teeLogQuery: TeeLogQuery = {
    //         agentId: query.agentId || '',
    //         roomId: query.roomId || '',
    //         entityId: query.entityId || '',
    //         type: query.type || '',
    //         containsContent: query.containsContent || '',
    //         startTimestamp: query.startTimestamp || std::nullopt,
    //         endTimestamp: query.endTimestamp || std::nullopt,
    //       };
    //       const agentRuntime: IAgentRuntime = agents.values().next().value;
    //       const teeLogService = agentRuntime.getService<ITeeLogService>(ServiceType.TEE);
    //       const pageQuery = teeLogService.getLogs(teeLogQuery, page, pageSize);
    //       const attestation = teeLogService.generateAttestation(nlohmann::json().dump(pageQuery));
    //       res.json({
    //         logs: pageQuery,
    //         attestation: attestation,
    //       });
    //     } catch (error) {
    //       logger.error('Failed to get TEE logs:', error instanceof Error ? error.message : String(error));
    //       res.status(500).json({
    //         error: 'Failed to get TEE logs',
    //       });
    //     }
    //   });

    //   return router;
    //
}

} // namespace elizaos
