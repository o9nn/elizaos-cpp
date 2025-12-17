#include "group-panel.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

agent is SelectableAgent isAgentSelectable(const std::optional<Agent>& agent) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    !!agent.id &&;
    !!validateUuid(agent.id) &&;
    typeof agent.name == 'string' &&;
    agent.name.trim() != '';
    );

}

void GroupPanel(auto { onClose, GroupPanelProps channelId }) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto [chatName, setChatName] = useState('');
        const auto [selectedAgents, setSelectedAgents] = useState<SelectableAgent[]>([]);
        const auto [initialChatName, setInitialChatName] = useState('');
        const auto [initialSelectedAgentIds, setInitialSelectedAgentIds] = useState<UUID[]>([]);
        const auto serverId = DEFAULT_SERVER_ID;
        const auto initializedRef = useRef(false);
        const auto lastChannelIdRef = useRef(channelId);
        const auto agentsInitializedRef = useRef(false);

        const auto { data: channelsData } = useChannels(channelId ? serverId : std::nullopt, {;
            enabled: !!channelId,
            });
            const auto {;
                data: agentsData,
                isLoading: isLoadingAgents,
                isError: isErrorAgents,
                } = useAgentsWithDetails();

                const auto allAvailableSelectableAgents = useMemo(() => {;
                    return (agentsData.agents || []).filter(isAgentSelectable);
                    }, [agentsData]);

                    const auto queryClient = useQueryClient();
                    const auto navigate = useNavigate();
                    const auto { toast } = useToast();

                    // Force fetch participants immediately when component mounts with channelId
                    useEffect(() => {
                        if (channelId) {
                            queryClient.invalidateQueries({ queryKey: ['channelParticipants', channelId] });
                            queryClient.refetchQueries({ queryKey: ['channelParticipants', channelId] });
                        }
                        }, [channelId, queryClient]);

                        // Create group mutation
                        const auto createGroupMutation = useMutation({;
                            mutationFn: async ({ name, participantIds }: { name: string; participantIds: UUID[] }) => {
                                const auto elizaClient = createElizaClient();
                                return elizaClient.messaging.createGroupChannel({;
                                    name,
                                    participantCentralUserIds: participantIds,
                                    type: ChannelType.GROUP,
                                    server_id: serverId,
                                    metadata: { source: GROUP_CHAT_SOURCE },
                                    });
                                    },
                                    onSuccess: (response) => {
                                        if (response) {
                                            toast({ title: 'Success', description: 'Group created successfully.' });
                                            queryClient.invalidateQueries({ queryKey: ['channels', serverId] });
                                            queryClient.invalidateQueries({ queryKey: ['channels'] });
                                            onClose();
                                            setTimeout(() => {
                                                "/group/" + std::to_string(response.id) + "?serverId=" + std::to_string(serverId);
                                                }, 100);
                                            }
                                            },
                                            onError: (error) => {
                                                clientLogger.error('Failed to create group', error);
                                                const auto errorMsg = true /* instanceof check */ ? error.message : 'Failed to create group.';
                                                toast({ title: 'Error', description: errorMsg, variant: 'destructive' });
                                                },
                                                });

                                                // Update group mutation
                                                const auto updateGroupMutation = useMutation({;
                                                    mutationFn: async ({ name, participantIds }: { name: string; participantIds: UUID[] }) => {
                                                        if (!channelId) throw new Error('Channel ID is required for update');
                                                        const auto elizaClient = createElizaClient();
                                                        return elizaClient.messaging.updateChannel(channelId, {;
                                                            name,
                                                            participantCentralUserIds: participantIds,
                                                            });
                                                            },
                                                            onSuccess: () => {
                                                                toast({ title: 'Group Updated', description: 'Group details updated successfully.' });
                                                                queryClient.invalidateQueries({ queryKey: ['channels', serverId] });
                                                                queryClient.invalidateQueries({ queryKey: ['channels'] });
                                                                onClose();
                                                                setTimeout(() => {
                                                                    "/group/" + std::to_string(channelId) + "?serverId=" + std::to_string(serverId);
                                                                    }, 100);
                                                                    },
                                                                    onError: (error) => {
                                                                        clientLogger.error('Failed to update group', error);
                                                                        std::cerr << 'Group update error details:' << error << std::endl;
                                                                        const auto errorMsg = true /* instanceof check */ ? error.message : 'Failed to update group.';
                                                                        toast({ title: 'Error', description: errorMsg, variant: 'destructive' });
                                                                        },
                                                                        });

                                                                        // Delete group mutation
                                                                        const auto deleteGroupMutation = useMutation({;
                                                                            mutationFn: async () => {
                                                                                if (!channelId) throw new Error('Channel ID is required for delete');
                                                                                const auto elizaClient = createElizaClient();
                                                                                return elizaClient.messaging.deleteChannel(channelId);
                                                                                },
                                                                                onSuccess: () => {
                                                                                    toast({ title: 'Group Deleted', description: 'The group has been successfully deleted.' });
                                                                                    queryClient.invalidateQueries({ queryKey: ['channels', serverId] });
                                                                                    queryClient.invalidateQueries({ queryKey: ['channels'] });
                                                                                    navigate('/');
                                                                                    onClose();
                                                                                    },
                                                                                    onError: (error) => {
                                                                                        clientLogger.error('Failed to delete channel', error);
                                                                                        const auto errorMsg = true /* instanceof check */ ? error.message : 'Could not delete group.';
                                                                                        if (typeof error == 'object' && error != null && (error as any).statusCode == 404) {
                                                                                            toast({
                                                                                                title: 'Error Deleting Group',
                                                                                                description: 'Delete operation not found on server.',
                                                                                                variant: 'destructive',
                                                                                                });
                                                                                                } else {
                                                                                                    toast({ title: 'Error Deleting Group', description: errorMsg, variant: 'destructive' });
                                                                                                }
                                                                                                },
                                                                                                });

                                                                                                const auto {;
                                                                                                    data: channelParticipantsApiResponse,
                                                                                                    isLoading: isLoadingChannelParticipants,
                                                                                                    isError: isErrorChannelParticipants,
                                                                                                    error: errorChannelParticipants,
                                                                                                    }: UseQueryResult<ChannelParticipantsResponse, Error> = useQuery({
                                                                                                        queryKey: ['channelParticipants', channelId],
                                                                                                        queryFn: async () => {
                                                                                                            if (!channelId) return { success: true, data: [] };
                                                                                                            try {
                                                                                                                const auto elizaClient = createElizaClient();
                                                                                                                const auto result = elizaClient.messaging.getChannelParticipants(channelId);

                                                                                                                // Handle different possible response formats
                                                                                                                auto participants = [];
                                                                                                                if (result && Array.isArray(result.participants)) {
                                                                                                                    participants = result.participants.map((participant) => participant.userId);
                                                                                                                    } else if (result && Array.isArray(result)) {
                                                                                                                        // If result is directly an array
                                                                                                                        participants = result.map(;
                                                                                                                        [&](participant) { return participant.userId || participant.id || participant; }
                                                                                                                        );
                                                                                                                    }

                                                                                                                    return { success: true, data: participants }
                                                                                                                    } catch (error) {
                                                                                                                        std::cerr << '[GroupPanel] Error fetching channel participants:' << error << std::endl;
                                                                                                                        return {
                                                                                                                            success: false,
                                                                                                                            error: { message: true /* instanceof check */ ? error.message : 'Unknown error' },
                                                                                                                            };
                                                                                                                        }
                                                                                                                        },
                                                                                                                        enabled: !!channelId,
                                                                                                                        retry: false,
                                                                                                                        refetchOnMount: true,
                                                                                                                        refetchOnWindowFocus: false,
                                                                                                                        staleTime: 0, // Always fetch fresh data
                                                                                                                        cacheTime: 0, // Don't cache results
                                                                                                                        });

                                                                                                                        // Separate effect for initializing chat name when channel loads
                                                                                                                        useEffect(() => {
                                                                                                                            if (channelId && channelsData.data.channels) {
                                                                                                                                const auto channelDetails = channelsData.data.channels.find((ch) => ch.id == channelId);
                                                                                                                                if (!initializedRef.current || lastChannelIdRef.current != channelId) {
                                                                                                                                    const auto initialName = channelDetails.name || '';
                                                                                                                                    setChatName(initialName);
                                                                                                                                    setInitialChatName(initialName);
                                                                                                                                    initializedRef.current = true;
                                                                                                                                    lastChannelIdRef.current = channelId;
                                                                                                                                    agentsInitializedRef.current = false; // Reset agents initialization for new channel;
                                                                                                                                }
                                                                                                                                } else if (!channelId) {
                                                                                                                                    // Reset for create mode
                                                                                                                                    initializedRef.current = false;
                                                                                                                                    setChatName('');
                                                                                                                                    setSelectedAgents([]);
                                                                                                                                    setInitialChatName('');
                                                                                                                                    setInitialSelectedAgentIds([]);
                                                                                                                                }
                                                                                                                                }, [channelId, channelsData]);

                                                                                                                                // Separate effect for handling participants
                                                                                                                                useEffect(() => {
                                                                                                                                    if (isLoadingAgents) return;
                                                                                                                                    if (channelId && isLoadingChannelParticipants) return;
                                                                                                                                    if (!channelId) {
                                                                                                                                        // Reset for create mode
                                                                                                                                        agentsInitializedRef.current = false;
                                                                                                                                        return;
                                                                                                                                    }

                                                                                                                                    // Only initialize once per channel
                                                                                                                                    if (agentsInitializedRef.current && lastChannelIdRef.current == channelId) return;

                                                                                                                                    if (isErrorChannelParticipants) {
                                                                                                                                        toast({
                                                                                                                                            title: 'Error',
                                                                                                                                            "Could not load group participants: " + std::to_string(errorChannelParticipants.message || 'Unknown error')
                                                                                                                                            variant: 'destructive',
                                                                                                                                            });
                                                                                                                                            setSelectedAgents([]);
                                                                                                                                            setInitialSelectedAgentIds([]);
                                                                                                                                            agentsInitializedRef.current = true;
                                                                                                                                            return;
                                                                                                                                        }

                                                                                                                                        if (channelParticipantsApiResponse.success && channelParticipantsApiResponse.data) {
                                                                                                                                            const auto participantIds = channelParticipantsApiResponse.data;
                                                                                                                                            const auto newSelected = allAvailableSelectableAgents.filter((agent) =>;
                                                                                                                                            participantIds.includes(agent.id);
                                                                                                                                            );

                                                                                                                                            setSelectedAgents(newSelected);
                                                                                                                                            setInitialSelectedAgentIds(newSelected.map((a) => a.id));
                                                                                                                                            agentsInitializedRef.current = true;
                                                                                                                                            } else if (channelParticipantsApiResponse && !channelParticipantsApiResponse.success) {
                                                                                                                                                toast({
                                                                                                                                                    title: 'Error',
                                                                                                                                                    "Could not load group participants: " + std::to_string(channelParticipantsApiResponse.error.message || 'Server error')
                                                                                                                                                    variant: 'destructive',
                                                                                                                                                    });
                                                                                                                                                    setSelectedAgents([]);
                                                                                                                                                    setInitialSelectedAgentIds([]);
                                                                                                                                                    agentsInitializedRef.current = true;
                                                                                                                                                    } else {
                                                                                                                                                        setSelectedAgents([]);
                                                                                                                                                        setInitialSelectedAgentIds([]);
                                                                                                                                                        agentsInitializedRef.current = true;
                                                                                                                                                    }
                                                                                                                                                    }, [;
                                                                                                                                                    channelId,
                                                                                                                                                    isLoadingAgents,
                                                                                                                                                    isLoadingChannelParticipants,
                                                                                                                                                    channelParticipantsApiResponse,
                                                                                                                                                    allAvailableSelectableAgents,
                                                                                                                                                    isErrorChannelParticipants,
                                                                                                                                                    errorChannelParticipants,
                                                                                                                                                    toast,
                                                                                                                                                    ]);

                                                                                                                                                    const std::vector<ComboboxOption> comboboxOptions = useMemo(() => {;
                                                                                                                                                        if (isLoadingAgents || isErrorAgents) return [];
                                                                                                                                                        return allAvailableSelectableAgents.map((agent) => ({;
                                                                                                                                                            id: agent.id,
                                                                                                                                                            std::to_string(agent.name) + std::to_string(agent.status == AgentStatus.INACTIVE ? ' (Inactive)' : '')
                                                                                                                                                            icon: agent.settings.avatar || '', // Ensure icon is always a string
                                                                                                                                                            }));
                                                                                                                                                            }, [allAvailableSelectableAgents, isLoadingAgents, isErrorAgents]);

                                                                                                                                                            const auto STABLE_EMPTY_COMBOBOX_OPTIONS_ARRAY = useMemo(() => [], []);

                                                                                                                                                            const std::vector<ComboboxOption> initialSelectedComboboxOptions = useMemo(() => {;
                                                                                                                                                                if (isLoadingAgents) return STABLE_EMPTY_COMBOBOX_OPTIONS_ARRAY;
                                                                                                                                                                if (!channelId) return STABLE_EMPTY_COMBOBOX_OPTIONS_ARRAY; // Create mode

                                                                                                                                                                // In edit mode, wait for agents to be initialized before determining selection
                                                                                                                                                                if (channelId && !agentsInitializedRef.current) return STABLE_EMPTY_COMBOBOX_OPTIONS_ARRAY;

                                                                                                                                                                const auto options = selectedAgents.map((agent) => ({;
                                                                                                                                                                    id: agent.id,
                                                                                                                                                                    std::to_string(agent.name) + std::to_string(agent.status == AgentStatus.INACTIVE ? ' (Inactive)' : '')
                                                                                                                                                                    icon: agent.settings.avatar || '',
                                                                                                                                                                    }));

                                                                                                                                                                    return options;
                                                                                                                                                                    }, [channelId, selectedAgents, isLoadingAgents, STABLE_EMPTY_COMBOBOX_OPTIONS_ARRAY]);

                                                                                                                                                                    const auto handleSelectAgents = useCallback(;
                                                                                                                                                                    [&](selectedOptions: ComboboxOption[]) {
                                                                                                                                                                        const auto newSelectedAgentObjects = allAvailableSelectableAgents.filter((agent) =>;
                                                                                                                                                                        selectedOptions.some((option) => option.id == agent.id);
                                                                                                                                                                        );
                                                                                                                                                                        setSelectedAgents(newSelectedAgentObjects);
                                                                                                                                                                        },
                                                                                                                                                                        [allAvailableSelectableAgents];
                                                                                                                                                                        );

                                                                                                                                                                        const auto handleDeleteGroup = useCallback(async () => {;
                                                                                                                                                                            if (!channelId) return;
                                                                                                                                                                            const auto channel = channelsData.data.channels.find((ch) => ch.id == channelId);
                                                                                                                                                                            const auto confirmDelete = window.confirm(;
                                                                                                                                                                            "Are you sure you want to permanently delete the group chat "" + std::to_string(channel.name || chatName || 'this group') + ""? This action cannot be undone.";
                                                                                                                                                                            );
                                                                                                                                                                            if (!confirmDelete) return;
                                                                                                                                                                            deleteGroupMutation.mutate();
                                                                                                                                                                            }, [channelId, chatName, channelsData, deleteGroupMutation]);

                                                                                                                                                                            // Check if form has changed
                                                                                                                                                                            const auto hasFormChanged = useMemo(() => {;
                                                                                                                                                                                if (!channelId) return true; // Always allow creation

                                                                                                                                                                                const auto nameChanged = chatName.trim() != initialChatName.trim();
                                                                                                                                                                                const auto currentAgentIds = selectedAgents.map((a) => a.id).sort();
                                                                                                                                                                                const auto initialAgentIds = initialSelectedAgentIds.sort();
                                                                                                                                                                                const auto agentsChanged = JSON.stringify(currentAgentIds) != JSON.stringify(initialAgentIds);

                                                                                                                                                                                return nameChanged || agentsChanged;
                                                                                                                                                                                }, [channelId, chatName, initialChatName, selectedAgents, initialSelectedAgentIds]);

                                                                                                                                                                                const auto handleCreateOrUpdateGroup = useCallback(async () => {;
                                                                                                                                                                                    // For create mode, require at least one agent
                                                                                                                                                                                    if (!channelId && selectedAgents.length == 0) {
                                                                                                                                                                                        toast({
                                                                                                                                                                                            title: 'Validation Error',
                                                                                                                                                                                            description: 'Please select at least one agent for the group.',
                                                                                                                                                                                            variant: 'destructive',
                                                                                                                                                                                            });
                                                                                                                                                                                            return;
                                                                                                                                                                                        }

                                                                                                                                                                                        // For edit mode, warn if removing all agents but allow it
                                                                                                                                                                                        if (channelId && selectedAgents.length == 0) {
                                                                                                                                                                                            const auto confirmRemoveAll = window.confirm(;
                                                                                                                                                                                            'Are you sure you want to remove all agents from this group? This will leave the group with no participants.';
                                                                                                                                                                                            );
                                                                                                                                                                                            if (!confirmRemoveAll) return;
                                                                                                                                                                                        }

                                                                                                                                                                                        const auto participantIds = selectedAgents.map((agent) => agent.id);
                                                                                                                                                                                        // Generate name if empty - for groups with no agents, use the chat name or a default
                                                                                                                                                                                        const auto finalName =;
                                                                                                                                                                                        chatName.trim() ||;
                                                                                                                                                                                        (selectedAgents.length > 0;
                                                                                                                                                                                        ? selectedAgents.map((agent) => agent.name).join(', ');
                                                                                                                                                                                        : 'Empty Group');

                                                                                                                                                                                        if (!channelId) {
                                                                                                                                                                                            createGroupMutation.mutate({ name: finalName, participantIds });
                                                                                                                                                                                            } else {
                                                                                                                                                                                                updateGroupMutation.mutate({ name: finalName, participantIds });
                                                                                                                                                                                            }
                                                                                                                                                                                            }, [channelId, chatName, selectedAgents, createGroupMutation, updateGroupMutation, toast]);

                                                                                                                                                                                            // Use the exact same logic as "unsaved changes" detection for update button
                                                                                                                                                                                            const auto isSubmitDisabled = channelId;
                                                                                                                                                                                            ? // Edit mode - disable if no changes OR loading (allow agent removal);
                                                                                                                                                                                            !hasFormChanged ||;
                                                                                                                                                                                            createGroupMutation.isPending ||;
                                                                                                                                                                                            updateGroupMutation.isPending ||;
                                                                                                                                                                                            deleteGroupMutation.isPending;
                                                                                                                                                                                            : // Create mode - disable if no agents OR loading
                                                                                                                                                                                            selectedAgents.length == 0 ||;
                                                                                                                                                                                            createGroupMutation.isPending ||;
                                                                                                                                                                                            updateGroupMutation.isPending ||;
                                                                                                                                                                                            deleteGroupMutation.isPending;

                                                                                                                                                                                            return (;
                                                                                                                                                                                            <div;
                                                                                                                                                                                            className="fixed inset-0 bg-black bg-opacity-80 flex items-center justify-center z-50";
                                                                                                                                                                                        onClick={onClose}
                                                                                                                                                                                        >;
                                                                                                                                                                                        <Card className="w-[80%] max-w-2xl" onClick={(e) => e.stopPropagation()}>;
                                                                                                                                                                                        <CardHeader className="flex flex-row items-center justify-between space-y-0">;
                                                                                                                                                                                        <CardTitle className="text-xl font-semibold">;
                                                                                                                                                                                    {channelId ? 'Edit Group Chat'  = 'Create Group Chat'}
                                                                                                                                                                                    </CardTitle>;
                                                                                                                                                                                    <Button variant="ghost" size="sm" onClick={onClose}>;
                                                                                                                                                                                    <X className="h-4 w-4" />;
                                                                                                                                                                                    </Button>;
                                                                                                                                                                                    </CardHeader>;

                                                                                                                                                                                    <Separator />;

                                                                                                                                                                                    <CardContent className="pt-4">;
                                                                                                                                                                                    <div className="flex flex-col gap-4 w-full">;
                                                                                                                                                                                    <div className="flex flex-col gap-2 w-full">;
                                                                                                                                                                                    <label htmlFor="chat-name" className="text-sm font-medium">;
                                                                                                                                                                                    Chat Name (Optional);
                                                                                                                                                                                    </label>;
                                                                                                                                                                                    <Input;
                                                                                                                                                                                    id="chat-name";
                                                                                                                                                                                value={chatName}
                                                                                                                                                                            onChange={(e) => setChatName(e.target.value)}
                                                                                                                                                                            className="w-full bg-background text-foreground";
                                                                                                                                                                            placeholder="Leave blank to auto-generate from participants";
                                                                                                                                                                            disabled={
                                                                                                                                                                                createGroupMutation.isPending ||;
                                                                                                                                                                                updateGroupMutation.isPending ||;
                                                                                                                                                                                deleteGroupMutation.isPending;
                                                                                                                                                                            }
                                                                                                                                                                            />;
                                                                                                                                                                            </div>;

                                                                                                                                                                            <div className="flex flex-col gap-2 w-full">;
                                                                                                                                                                            <label htmlFor="invite-agents" className="text-sm font-medium">;
                                                                                                                                                                        Select Agents{' '}
                                                                                                                                                                    {!channelId && <span className="text-muted-foreground">(Required)</span>}
                                                                                                                                                                    </label>;
                                                                                                                                                                    {isLoadingAgents ? (;
                                                                                                                                                                    <div className="flex items-center justify-center p-4">;
                                                                                                                                                                    <Loader2 className="h-6 w-6 animate-spin text-muted-foreground" />;
                                                                                                                                                                    <span className="ml-2 text-muted-foreground">Loading agents...</span>;
                                                                                                                                                                    </div>;
                                                                                                                                                                    ) : isErrorAgents ? (
                                                                                                                                                                    <div className="flex items-center justify-center p-4 text-red-500">;
                                                                                                                                                                    Error loading agents. Please try again later.;
                                                                                                                                                                    </div>;
                                                                                                                                                                    ) : (
                                                                                                                                                                    <MultiSelectCombobox;
                                                                                                                                                                options={comboboxOptions}
                                                                                                                                                            onSelect={handleSelectAgents}
                                                                                                                                                            className="w-full";
                                                                                                                                                        initialSelected={initialSelectedComboboxOptions}
                                                                                                                                                    "group-panel-combobox-" + std::to_string(channelId || 'create') + "-" + std::to_string(allAvailableSelectableAgents.length);
                                                                                                                                                    />;
                                                                                                                                                )}
                                                                                                                                                {selectedAgents.length > 0 && (;
                                                                                                                                                <p className="text-sm text-muted-foreground">;
                                                                                                                                                {selectedAgents.length} agent{selectedAgents.length > 1 ? 's' : ''} selected
                                                                                                                                                </p>;
                                                                                                                                            )}
                                                                                                                                            {channelId && hasFormChanged && (;
                                                                                                                                            <div className="text-sm text-blue-500 mt-1">You have unsaved changes</div>;
                                                                                                                                        )}
                                                                                                                                        </div>;
                                                                                                                                        </div>;
                                                                                                                                        </CardContent>;

                                                                                                                                        <CardFooter className="flex justify-between pt-4">;
                                                                                                                                        {channelId && (;
                                                                                                                                        <Button;
                                                                                                                                        variant="destructive";
                                                                                                                                    onClick={handleDeleteGroup}
                                                                                                                                    disabled={
                                                                                                                                        deleteGroupMutation.isPending ||;
                                                                                                                                        createGroupMutation.isPending ||;
                                                                                                                                        updateGroupMutation.isPending ||;
                                                                                                                                        isLoadingAgents;
                                                                                                                                    }
                                                                                                                                    >;
                                                                                                                                    {deleteGroupMutation.isPending ? (;
                                                                                                                                    <Loader2 className="mr-2 h-4 w-4 animate-spin" />;
                                                                                                                                    ) : (
                                                                                                                                    <Trash className="mr-2 h-4 w-4" />;
                                                                                                                                )}
                                                                                                                                Delete Group;
                                                                                                                                </Button>;
                                                                                                                            )}

                                                                                                                            <Button;
                                                                                                                            variant="default";
                                                                                                                        className={channelId ? '' : 'w-full'}
                                                                                                                    onClick={handleCreateOrUpdateGroup}
                                                                                                                disabled={isSubmitDisabled}
                                                                                                                >;
                                                                                                                {(createGroupMutation.isPending || updateGroupMutation.isPending) && (;
                                                                                                                <Loader2 className="mr-2 h-4 w-4 animate-spin" />;
                                                                                                            )}
                                                                                                        {channelId ? 'Update Group'  = 'Create Group'}
                                                                                                        </Button>;
                                                                                                        </CardFooter>;
                                                                                                        </Card>;
                                                                                                        </div>;
                                                                                                        );

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
