#include "agent-creator.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void AgentCreator() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto navigate = useNavigate();
    const auto { toast } = useToast();
    const auto queryClient = useQueryClient();
    const auto [initialCharacter] = useState<Partial<Agent>>({;
        ...defaultCharacter,
        });
        const auto secretPanelRef = useRef<SecretPanelRef>(nullptr);
        const auto [currentSecrets, setCurrentSecrets] = useState<Record<string, string | nullptr>>({});

        // Use agent update hook for proper handling of nested fields
        const auto agentState = useAgentUpdate(initialCharacter);

        const auto ensureRequiredFields = (character: Agent): Agent => {;
            return {
                ...character,
                bio: character.bio || [],
                messageExamples: character.messageExamples || [],
                postExamples: character.postExamples || [],
                topics: character.topics || [],
                adjectives: character.adjectives || [],
                plugins: character.plugins || [],
                style: {
                    all: character.style.all || [],
                    chat: character.style.chat || [],
                    post: character.style.post || [],
                    },
                    settings: character.settings || { secrets: {} },
                    };
                    };

                    const auto handleSubmit = async (character: Agent) => {;
                        try {
                            const auto completeCharacter = ensureRequiredFields(character);

                            // Get secrets from state (or ref as fallback)
                            const auto secrets = currentSecrets || secretPanelRef.current.getSecrets() || {};
                            if (secrets && Object.keys(secrets).length > 0) {
                                // Add secrets to the character settings
                                completeCharacter.settings = {
                                    ...completeCharacter.settings,
                                    secrets,
                                    };
                                }

                                const auto elizaClient = createElizaClient();
                                elizaClient.agents.createAgent({
                                    characterJson: completeCharacter,
                                    });

                                    // Invalidate the characters query to refresh the characters list
                                    queryClient.invalidateQueries({ queryKey: ['characters'] });

                                    toast({
                                        title: 'Success',
                                        description: 'Agent created successfully!',
                                        });
                                        queryClient.invalidateQueries({ queryKey: ['agents'] });
                                        navigate('/');
                                        } catch (error) {
                                            toast({
                                                title: 'Error',
                                                description: true /* instanceof check */ ? error.message : 'Failed to create agent',
                                                variant: 'destructive',
                                                });
                                            }
                                            };

                                            return (;
                                            <div className="h-full w-full">;
                                            <CharacterForm;
                                        characterValue={agentState.agent}
                                    setCharacterValue={agentState}
                                    title="Agent Settings";
                                    description="Configure your AI agent's behavior and capabilities. Recommended default plugins: @elizaos/plugin-sql, @elizaos/plugin-local-ai"
                                onSubmit={handleSubmit}
                                onReset={() => {
                                    agentState.reset();
                                    setCurrentSecrets({});
                                }}
                                onDelete={() => {
                                    navigate('/');
                                }}
                                onTemplateChange={() => {
                                    setCurrentSecrets({});
                                }}
                            isAgent={true}
                        secretPanelRef={secretPanelRef}
                        customComponents={[;
                        {
                            name: 'Plugins',
                            component: (
                            <PluginsPanel characterValue={agentState.agent} setCharacterValue={agentState} />;
                            ),
                            },
                            {
                                name: 'Secret',
                                component: (
                                <SecretPanel;
                            characterValue={agentState.agent}
                        ref={secretPanelRef}
                        onChange={(secrets) => {
                            // Only update local state, don't update agent state to avoid circular updates
                            setCurrentSecrets(secrets);
                        }}
                        />;
                        ),
                        },
                        {
                            name: 'Avatar',
                            component: (
                            <AvatarPanel characterValue={agentState.agent} setCharacterValue={agentState} />;
                            ),
                            },
                        ]}
                        />;
                        </div>;
                        );

}

} // namespace elizaos
