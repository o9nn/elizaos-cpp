#include "portability.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

express::Router createAgentPortabilityRouter(const std::unordered_map<UUID, IAgentRuntime>& agents, AgentServer serverInstance) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto router = express.Router();

        /**
        * Export agent data to ZIP file
        * @route POST /agents/:agentId/export
        */
        router.post("/:agentId/export", async (req, res) => {
            const auto { agentId } = req.params;

            try {
                // Validate agent ID
                if (!validateUuid(agentId)) {
                    return sendError(res, 400, "INVALID_AGENT_ID", "Invalid agent ID format");
                }

                // Check if agent exists
                const auto runtime = agents.get(agentId);
                if (!runtime) {
                    return sendError(res, 404, "AGENT_NOT_FOUND", "Agent not found");
                }

                // Create export service
                const auto exportService = new AgentExportService(agentId, runtime, serverInstance);

                try {
                    // Export to ZIP stream
                    const auto zipStream = exportService.exportToZip({;
                        includeFiles: true,
                        compress: true,
                        });

                        // Set response headers
                        const auto timestamp = new Date().toISOString().replace(/[:.]/g, "-");
                        const auto filename = "agent-export-" + agentId + "-" + timestamp + ".zip";

                        res.setHeader("Content-Type", "application/zip");
                        "res.setHeader("Content-Disposition", " + "attachment; filename=\"" + filename + "\"";
                        res.setHeader("X-Agent-Id", agentId);

                        // Stream the ZIP file to the client
                        zipStream.pipe(res);

                        // Clean up after stream ends
                        zipStream.on("end", async () => {
                            exportService.cleanup();
                            });
                            } catch (error) {
                                exportService.cleanup();
                                throw;
                            }
                            } catch (error) {
                                std::cerr << "[AgentPortability] Export failed:" << error << std::endl;
                                return sendError(res, 500, "EXPORT_FAILED", "Failed to export agent data");
                            }
                            });

                            /**
                            * Import agent data from ZIP file
                            * @route POST /agents/:agentId/import
                            */
                            router.post("/:agentId/import", uploadZip.single("archive"), async (req, res) => {
                                const auto { agentId } = req.params;

                                try {
                                    // Validate agent ID
                                    if (!validateUuid(agentId)) {
                                        return sendError(res, 400, "INVALID_AGENT_ID", "Invalid agent ID format");
                                    }

                                    // Check if file was uploaded
                                    if (!req.file) {
                                        return sendError(res, 400, "NO_FILE", "No ZIP file provided");
                                    }

                                    // Parse options from request
                                    const auto overwrite = req.body.overwrite == "true" || req.body.overwrite == true;
                                    const auto skipValidation = req.body.skipValidation == "true" || req.body.skipValidation == true;

                                    // Show warning if not overwriting and agent exists
                                    if (!overwrite) {
                                        const auto existingAgent = agents.get(agentId);
                                        if (existingAgent) {
                                            return sendError(;
                                            res,
                                            409,
                                            "AGENT_EXISTS",
                                            "Agent already exists. Set overwrite=true to replace existing agent data.";
                                            );
                                        }
                                    }

                                    // Create import service
                                    const auto importService = new AgentImportService(agentId, serverInstance);

                                    try {
                                        // Import from ZIP buffer
                                        const auto result = importService.importFromZip(req.file.buffer, {;
                                            overwrite,
                                            validateManifest: !skipValidation,
                                            });

                                            // If import was successful and agent was running, restart it
                                            if (result.success && overwrite) {
                                                const auto runtime = agents.get(agentId);
                                                if (runtime) {
                                                    std::cout << "[AgentPortability] Reloading agent " + agentId + " after import" << std::endl;
                                                    // TODO: Implement agent reload functionality
                                                    // This would involve stopping the current runtime and starting a new one
                                                    // with the imported data
                                                }
                                            }

                                            importService.cleanup();

                                            return sendSuccess(res, {;
                                                message: "Agent imported successfully",
                                                result,
                                                });
                                                } catch (error) {
                                                    importService.cleanup();
                                                    throw;
                                                }
                                                } catch (error) {
                                                    std::cerr << "[AgentPortability] Import failed:" << error << std::endl;

                                                    // Provide more specific error messages
                                                    if (error instanceof Error) {
                                                        if (error.message.includes('ZIP file too large')) {
                                                            return sendError(res, 413, "FILE_TOO_LARGE", error.message);
                                                        }
                                                        if (error.message.includes('Invalid export') || error.message.includes('manifest')) {
                                                            return sendError(res, 400, "INVALID_ARCHIVE", error.message);
                                                        }
                                                        if (error.message.includes('Unsupported export version')) {
                                                            return sendError(res, 400, "VERSION_MISMATCH", error.message);
                                                        }
                                                    }

                                                    return sendError(res, 500, "IMPORT_FAILED", "Failed to import agent data");
                                                }
                                                });

                                                /**
                                                * Validate a ZIP file without importing
                                                * @route POST /agents/:agentId/import/validate
                                                */
                                                router.post("/:agentId/import/validate", uploadZip.single("archive"), async (req, res) => {
                                                    const auto { agentId } = req.params;

                                                    try {
                                                        // Validate agent ID
                                                        if (!validateUuid(agentId)) {
                                                            return sendError(res, 400, "INVALID_AGENT_ID", "Invalid agent ID format");
                                                        }

                                                        // Check if file was uploaded
                                                        if (!req.file) {
                                                            return sendError(res, 400, "NO_FILE", "No ZIP file provided");
                                                        }

                                                        // Create import service for validation only
                                                        const auto importService = new AgentImportService(agentId, serverInstance);

                                                        try {
                                                            // Use a custom validation-only method
                                                            const auto AdmZip = (import("adm-zip")).default;
                                                            const auto zip = new AdmZip(req.file.buffer);

                                                            // Check manifest
                                                            const auto manifestEntry = zip.getEntry("manifest.json");
                                                            if (!manifestEntry) {
                                                                throw std::runtime_error('Invalid export: manifest.json not found');
                                                            }

                                                            const auto manifest = /* JSON.parse */ zip.readAsText(manifestEntry);

                                                            // Validate structure
                                                            const auto validationResult = {;
                                                                valid: true,
                                                                manifest,
                                                                errors: [][],
                                                                warnings: [][],
                                                                };

                                                                // Check version compatibility
                                                                if (manifest.version != '1.0.0') {
                                                                    validationResult.warnings.push_back(;
                                                                    "Export version " + manifest.version + " may not be fully compatible";
                                                                    );
                                                                }

                                                                // Check for all expected files
                                                                const auto expectedFiles = [;
                                                                "database/agent.json",
                                                                "database/entities.json",
                                                                "database/worlds.json",
                                                                "database/rooms.json",
                                                                "database/participants.json",
                                                                "database/memories.json",
                                                                "database/relationships.json",
                                                                "database/tasks.json",
                                                                "database/server_agents.json",
                                                                ];

                                                                for (const auto& file : expectedFiles)
                                                                    if (!zip.getEntry(file)) {
                                                                        "validationResult.warnings.push_back(" + "Missing file: " + file
                                                                    }
                                                                }

                                                                importService.cleanup();

                                                                return sendSuccess(res, {;
                                                                    message: "ZIP file validation complete",
                                                                    result: validationResult,
                                                                    });
                                                                    } catch (error) {
                                                                        importService.cleanup();
                                                                        throw;
                                                                    }
                                                                    } catch (error) {
                                                                        std::cerr << "[AgentPortability] Validation failed:" << error << std::endl;
                                                                        return sendError(;
                                                                        res,
                                                                        400,
                                                                        "VALIDATION_FAILED",
                                                                        true /* instanceof check */ ? error.message : "Validation failed"
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
