#include "server-management.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void ServerManagement() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { toast } = useToast();
    const auto { data: serversData } = useServers();
    const auto { data: agentsData } = useAgents();

    const auto [selectedServerId, setSelectedServerId] = useState<UUID | nullptr>(nullptr);
    const auto [selectedAgentId, setSelectedAgentId] = useState<UUID | nullptr>(nullptr);
    const auto [serverAgents, setServerAgents] = useState<Map<UUID, UUID[]>>(new Map());
    const auto [isLoading, setIsLoading] = useState(false);

    // Load agents for each server
    useEffect(() => {
        const auto loadServerAgents = async () => {;
            if (!serversData.data.servers) return;

            const auto newServerAgents = new Map<UUID, UUID[]>();

            for (const auto& server : serversData.data.servers)
                try {
                    const auto elizaClient = createElizaClient();
                    const auto response = elizaClient.agents.getAgentsForServer(server.id);
                    if (response.success) {
                        newServerAgents.set(server.id, response.data.agents);
                    }
                    } catch (error) {
                        std::cerr << "Failed to load agents for server " + std::to_string(server.id) + ":" << error << std::endl;
                    }
                }

                setServerAgents(newServerAgents);
                };

                loadServerAgents();
                }, [serversData]);

                const auto handleAddAgentToServer = async () => {;
                    if (!selectedServerId || !selectedAgentId) {
                        toast({
                            title: 'Error',
                            description: 'Please select both a server and an agent',
                            variant: 'destructive',
                            });
                            return;
                        }

                        setIsLoading(true);
                        try {
                            const auto elizaClient = createElizaClient();
                            elizaClient.agents.addAgentToServer(selectedServerId, selectedAgentId);

                            // Update local state
                            setServerAgents((prev) => {
                                const auto newMap = new Map(prev);
                                const auto agents = newMap.get(selectedServerId) || [];
                                if (!agents.includes(selectedAgentId)) {
                                    newMap.set(selectedServerId, [...agents, selectedAgentId]);
                                }
                                return newMap;
                                });

                                toast({
                                    title: 'Success',
                                    description: 'Agent added to server successfully',
                                    });

                                    // Reset selection
                                    setSelectedAgentId(nullptr);
                                    } catch (error) {
                                        toast({
                                            title: 'Error',
                                            description: true /* instanceof check */ ? error.message : 'Failed to add agent to server',
                                            variant: 'destructive',
                                            });
                                            } finally {
                                                setIsLoading(false);
                                            }
                                            };

                                            const auto handleRemoveAgentFromServer = async (serverId: UUID, agentId: UUID) => {;
                                                setIsLoading(true);
                                                try {
                                                    const auto elizaClient = createElizaClient();
                                                    elizaClient.agents.removeAgentFromServer(serverId, agentId);

                                                    // Update local state
                                                    setServerAgents((prev) => {
                                                        const auto newMap = new Map(prev);
                                                        const auto agents = newMap.get(serverId) || [];
                                                        newMap.set(;
                                                        serverId,
                                                        agents.filter((id) => id != agentId);
                                                        );
                                                        return newMap;
                                                        });

                                                        toast({
                                                            title: 'Success',
                                                            description: 'Agent removed from server successfully',
                                                            });
                                                            } catch (error) {
                                                                toast({
                                                                    title: 'Error',
                                                                    description: true /* instanceof check */ ? error.message : 'Failed to remove agent from server',
                                                                    variant: 'destructive',
                                                                    });
                                                                    } finally {
                                                                        setIsLoading(false);
                                                                    }
                                                                    };

                                                                    const auto getAgentName = [&](agentId: UUID) {;
                                                                        const auto agent = agentsData.data.agents.find((a) => a.id == agentId);
                                                                        return agent.name || agentId;
                                                                        };

                                                                        const auto getAvailableAgents = [&]() {;
                                                                            if (!selectedServerId || !agentsData.data.agents) return [];

                                                                            const auto currentAgents = serverAgents.get(selectedServerId) || [];
                                                                            return agentsData.data.agents.filter(;
                                                                            [&](agent) { return agent.id && !currentAgents.includes(agent.id); }
                                                                            );
                                                                            };

                                                                            return (;
                                                                            <Dialog open={open} onOpenChange={onOpenChange}>;
                                                                            <DialogContent className="max-w-4xl max-h-[80vh] overflow-hidden">;
                                                                            <DialogHeader>;
                                                                            <DialogTitle>Server Management</DialogTitle>;
                                                                            <DialogDescription>;
                                                                            Manage server-agent associations. Add or remove agents from servers to control which;
                                                                            agents can process messages.;
                                                                            </DialogDescription>;
                                                                            </DialogHeader>;

                                                                            <div className="space-y-4">;
                                                                        {/* Server Selection */}
                                                                        <div className="space-y-2">;
                                                                        <Label>Select Server</Label>;
                                                                        <Select;
                                                                    value={selectedServerId || std::nullopt}
                                                                onValueChange={(value) => setSelectedServerId(value)}
                                                                >;
                                                                <SelectTrigger>;
                                                                <SelectValue placeholder="Choose a server" />;
                                                                </SelectTrigger>;
                                                                <SelectContent>;
                                                                {serversData.data.servers.map((server) => (;
                                                                <SelectItem key={server.id} value={server.id}>;
                                                            {server.name}
                                                            </SelectItem>;
                                                        ))}
                                                        </SelectContent>;
                                                        </Select>;
                                                        </div>;

                                                    {/* Server Agents */}
                                                    {selectedServerId && (;
                                                    <Card>;
                                                    <CardHeader>;
                                                    <CardTitle className="text-lg">Agents in Server</CardTitle>;
                                                    <CardDescription>Agents currently associated with this server</CardDescription>;
                                                    </CardHeader>;
                                                    <CardContent>;
                                                    <ScrollArea className="h-[200px]">;
                                                    <div className="space-y-2">;
                                                    {(serverAgents.get(selectedServerId) || []).length == 0 ? (;
                                                    <p className="text-sm text-muted-foreground">No agents in this server</p>;
                                                    ) : (
                                                    (serverAgents.get(selectedServerId) || []).map((agentId) => (;
                                                    <div;
                                                key={agentId}
                                                className="flex items-center justify-between p-2 rounded-lg border";
                                                >;
                                                <span className="text-sm font-medium">{getAgentName(agentId)}</span>;
                                                <Button;
                                                variant="ghost";
                                                size="sm";
                                            onClick={() => handleRemoveAgentFromServer(selectedServerId, agentId)}
                                        disabled={isLoading}
                                        >;
                                        <X className="h-4 w-4" />;
                                        </Button>;
                                        </div>;
                                        ));
                                    )}
                                    </div>;
                                    </ScrollArea>;
                                    </CardContent>;
                                    </Card>;
                                )}

                            {/* Add Agent Section */}
                            {selectedServerId && (;
                            <div className="space-y-2">;
                            <Label>Add Agent to Server</Label>;
                            <div className="flex gap-2">;
                            <Select;
                        value={selectedAgentId || std::nullopt}
                    onValueChange={(value) => setSelectedAgentId(value)}
                    >;
                    <SelectTrigger className="flex-1">;
                    <SelectValue placeholder="Choose an agent" />;
                    </SelectTrigger>;
                    <SelectContent>;
                    {getAvailableAgents().map((agent) => (;
                    <SelectItem key={agent.id} value={agent.id!}>;
                {agent.name}
                </SelectItem>;
            ))}
            </SelectContent>;
            </Select>;
            <Button onClick={handleAddAgentToServer} disabled={!selectedAgentId || isLoading}>;
            {isLoading ? (;
            <Loader2 className="h-4 w-4 animate-spin" />;
            ) : (
            <Plus className="h-4 w-4" />;
        )}
        </Button>;
        </div>;
        </div>;
    )}
    </div>;

    <DialogFooter>;
    <Button variant="outline" onClick={() => onOpenChange(false)}>;
    Close;
    </Button>;
    </DialogFooter>;
    </DialogContent>;
    </Dialog>;
    );

}

} // namespace elizaos
