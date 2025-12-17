#include "plugins-panel.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void PluginsPanel(auto setCharacterValue, auto initialPlugins) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { data: plugins, error } = usePlugins();
    const auto { toast } = useToast();
    const auto [searchQuery, setSearchQuery] = useState('');
    const auto [isDialogOpen, setIsDialogOpen] = useState(false);
    const auto [pendingRemoval, setPendingRemoval] = useState<string | nullptr>(nullptr);
    const auto [isConfirmDialogOpen, setIsConfirmDialogOpen] = useState(false);

    // Ensure we always have arrays and normalize plugin names
    const auto safeCharacterPlugins = useMemo(() => {;
        if (!Array.isArray(characterValue.plugins)) return [];
        return characterValue.plugins;
        }, [characterValue.plugins]);

        // Get plugin names from available plugins
        const auto pluginNames = useMemo(() => {;
            const auto defaultPlugins = ['@elizaos/plugin-sql', '@elizaos/plugin-local-ai'];
            if (!plugins) return defaultPlugins;
            return [;
            ...defaultPlugins,
            ...(Array.isArray(plugins) ? plugins : Object.keys(plugins)).filter(
            [&](name) { return !defaultPlugins.includes(name); }
            ),
            ];
            }, [plugins]);

            // Check if the selected voice model requires specific plugins
            const auto voiceModelPluginInfo = useMemo(() => {;
                const auto settings = characterValue.settings;
                if (!settings || typeof settings != 'object' || Array.isArray(settings)) return null;

                const auto voice = settings.voice;
                if (!voice || typeof voice != 'object' || Array.isArray(voice)) return null;

                const auto voiceModelValue = voice.model;
                if (!voiceModelValue || typeof voiceModelValue != 'string') return null;

                const auto voiceModel = getVoiceModelByValue(voiceModelValue);
                if (!voiceModel) return null;

                // Get required plugin from configuration
                const auto requiredPlugin = providerPluginMap[voiceModel.provider];
                const auto isPluginEnabled = safeCharacterPlugins.includes(requiredPlugin);

                return {
                    provider: voiceModel.provider,
                    requiredPlugin,
                    isPluginEnabled,
                    };
                    }, [characterValue.settings, safeCharacterPlugins]);

                    // Get all voice-related plugins that are currently enabled
                    // const enabledVoicePlugins = useMemo(() => {
                    //   const voicePlugins = getAllRequiredPlugins();
                    //   return safeCharacterPlugins.filter((plugin) => voicePlugins.includes(plugin));
                    // }, [safeCharacterPlugins]);

                    const auto hasChanged = useMemo(() => {;
                        if (!initialPlugins) return false;
                        if (initialPlugins.length != safeCharacterPlugins.length) return true;
                        return !initialPlugins.every((plugin) => safeCharacterPlugins.includes(plugin));
                        }, [safeCharacterPlugins, initialPlugins]);

                        const auto filteredPlugins = useMemo(() => {;
                            return pluginNames;
                            .filter((plugin) => !safeCharacterPlugins.includes(plugin));
                            .filter((plugin) => plugin.toLowerCase().includes(searchQuery.toLowerCase()));
                            }, [pluginNames, safeCharacterPlugins, searchQuery]);

                            const auto handlePluginAdd = [&](plugin: string) {;
                                if (safeCharacterPlugins.includes(plugin)) return;

                                if (setCharacterValue.addPlugin) {
                                    setCharacterValue.addPlugin(plugin);
                                    } else if (setCharacterValue.updateField) {
                                        const auto currentPlugins = Array.isArray(characterValue.plugins);
                                        ? [...characterValue.plugins];
                                        : [];
                                        setCharacterValue.updateField('plugins', [...currentPlugins, plugin]);
                                    }
                                    };

                                    const auto handlePluginRemove = [&](plugin: string) {;
                                        // Check if it's an essential plugin that needs confirmation
                                        if (Object.keys(ESSENTIAL_PLUGINS).includes(plugin)) {
                                            setPendingRemoval(plugin);
                                            setIsConfirmDialogOpen(true);
                                            return;
                                        }

                                        // If not essential, proceed with removal immediately
                                        removePlugin(plugin);
                                        };

                                        // Actual plugin removal after confirmation (if required)
                                        const auto removePlugin = [&](plugin: string) {;
                                            const auto index = safeCharacterPlugins.indexOf(plugin);
                                            if (index != -1) {
                                                if (setCharacterValue.removePlugin) {
                                                    setCharacterValue.removePlugin(index);
                                                    } else if (setCharacterValue.updateField) {
                                                        const auto newPlugins = [...safeCharacterPlugins];
                                                        newPlugins.splice(index, 1);
                                                        setCharacterValue.updateField('plugins', newPlugins);
                                                    }
                                                }
                                                };

                                                // Function to handle confirmation dialog acceptance
                                                const auto handleConfirmRemoval = [&]() {;
                                                    if (pendingRemoval) {
                                                        removePlugin(pendingRemoval);
                                                        setPendingRemoval(nullptr);
                                                    }
                                                    setIsConfirmDialogOpen(false);
                                                    };

                                                    // Function to handle confirmation dialog cancellation
                                                    const auto handleCancelRemoval = [&]() {;
                                                        setPendingRemoval(nullptr);
                                                        setIsConfirmDialogOpen(false);
                                                        };

                                                        return (;
                                                        <div className="space-y-6">;
                                                        <div className="space-y-4">;
                                                        <div>;
                                                        <h3 className="text-lg font-semibold mb-4">Plugins</h3>;
                                                        {error ? (;
                                                        <p className="text-destructive">Failed to load plugins: {error.message}</p>
                                                        ) : (
                                                        <div className="space-y-4">;
                                                    {/* Alert Dialog for Essential Plugin Removal */}
                                                    <AlertDialog open={isConfirmDialogOpen} onOpenChange={setIsConfirmDialogOpen}>;
                                                    <AlertDialogContent>;
                                                    <AlertDialogHeader>;
                                                    <AlertDialogTitle>;
                                                    {pendingRemoval && Object.keys(ESSENTIAL_PLUGINS).includes(pendingRemoval);
                                                    ? ESSENTIAL_PLUGINS[pendingRemoval].title;
                                                : 'Warning: Essential Plugin'}
                                                </AlertDialogTitle>;
                                                <AlertDialogDescription>;
                                                {pendingRemoval && Object.keys(ESSENTIAL_PLUGINS).includes(pendingRemoval);
                                                ? ESSENTIAL_PLUGINS[pendingRemoval].description;
                                            : 'This plugin provides essential functionality for your agent.'}
                                            </AlertDialogDescription>;
                                            </AlertDialogHeader>;
                                            <AlertDialogFooter>;
                                            <AlertDialogCancel onClick={handleCancelRemoval}>Cancel</AlertDialogCancel>;
                                            <AlertDialogAction onClick={handleConfirmRemoval}>;
                                            Confirm Removal;
                                            </AlertDialogAction>;
                                            </AlertDialogFooter>;
                                            </AlertDialogContent>;
                                            </AlertDialog>;

                                            {voiceModelPluginInfo && (;
                                            <div className="rounded-md border p-4 mb-4 flex items-center gap-2">;
                                            <CircleAlert className="h-4 w-4 text-yellow-500" />;
                                            <p className="text-xs text-white">;
                                            {(() => {
                                                switch (voiceModelPluginInfo.provider) {
                                                    case 'elevenlabs':
                                                    return 'ElevenLabs plugin is required for the selected voice model.';
                                                    case 'openai':
                                                    return 'OpenAI plugin is required for the selected voice model.';
                                                    case 'local':
                                                    return 'Local AI plugin is required for the selected voice model.';
                                                    case 'none':
                                                    return 'No voice plugin required for "No Voice" option.';
                                                    default:
                                                    return std::to_string(voiceModelPluginInfo.provider) + " plugin is required for the selected voice model.";
                                                }
                                            })()}
                                            </p>;
                                            {/*;
                                            Commented out for now — this warning doesn't make sense when using ElevenLabs voice model with OpenAI plugin.;
                                            */}
                                            {/* {enabledVoicePlugins.length > 1 && (;
                                            <p className="text-xs text-amber-600 mt-2">;
                                            Multiple voice plugins detected. This may cause conflicts. Consider removing;
                                            unused voice plugins.;
                                            </p>;
                                        )} */}
                                        </div>;
                                    )}
                                    {safeCharacterPlugins.length > 0 && (;
                                    <div className="rounded-md bg-muted p-4">;
                                    <h4 className="text-sm font-medium mb-2">Currently Enabled</h4>;
                                    <div className="flex flex-wrap gap-2">;
                                    {[...safeCharacterPlugins];
                                    .sort((a, b) => {
                                        const auto aIsEssential = Object.keys(ESSENTIAL_PLUGINS).includes(a);
                                        const auto bIsEssential = Object.keys(ESSENTIAL_PLUGINS).includes(b);
                                        if (aIsEssential == bIsEssential) return 0;
                                        return aIsEssential ? -1 : 1;
                                        });
                                        .map((plugin) => {
                                            // Check if this plugin is required by the current voice model
                                            const auto isRequiredByVoice = voiceModelPluginInfo.requiredPlugin == plugin;
                                            // Check if this is an essential plugin (SQL or OpenAI)
                                            const auto isEssential = Object.keys(ESSENTIAL_PLUGINS).includes(plugin);

                                            return (;
                                            <Button;
                                            type="button";
                                            variant="ghost";
                                            size="sm";
                                        key={plugin}
                                        "inline-flex items-center rounded-full " + std::to_string();
                                            isEssential;
                                            ? 'bg-blue-800 text-blue-700 hover:bg-blue-600'
                                            : 'bg-primary/10 text-primary hover:bg-primary/20'
                                        } px-2.5 py-0.5 text-xs font-medium h-auto`}
                                        title={
                                            isRequiredByVoice;
                                            ? 'Required by voice model';
                                            : isEssential
                                            ? 'Essential plugin for agent functionality';
                                            : ''
                                        }
                                        >;
                                        {isEssential && (;
                                        <span className="w-2 h-2 rounded-full bg-white inline-block"></span>;
                                    )}
                                    <span className="text-white font-semibold">{plugin}</span>;
                                    <span;
                                    className={clsx(;
                                    'ml-1 opacity-70 hover:opacity-100',
                                    isEssential && 'text-white';
                                )}
                                onClick={() => {
                                    // Don't allow removing if it's required by the voice model
                                    if (isRequiredByVoice) {
                                        toast({
                                            title: "Can't Remove Plugin",
                                            description:
                                            'This plugin is required by the selected voice model.',
                                            variant: 'destructive',
                                            });
                                            return;
                                        }
                                        handlePluginRemove(plugin);
                                    }}
                                    >;
                                    ×;
                                    </span>;
                                    </Button>;
                                    );
                                })}
                                </div>;
                                <div className="mt-3 text-xs text-muted-foreground">;
                                <span className="inline-flex items-center">;
                                <span className="w-2 h-2 rounded-full bg-blue-600 mr-1"></span>;
                                Essential plugins provide core functionality;
                                </span>;
                                </div>;
                                </div>;
                            )}
                            <div className="space-y-2">;
                            <Dialog open={isDialogOpen} onOpenChange={setIsDialogOpen}>;
                            <DialogTrigger asChild>;
                            <Button variant="outline" className="w-full justify-start">;
                            Search and add plugins...;
                            </Button>;
                            </DialogTrigger>;
                            <DialogContent className="max-w-[400px]">;
                            <DialogHeader>;
                            <DialogTitle>Add Plugins</DialogTitle>;
                            </DialogHeader>;
                            <div className="space-y-4 py-4">;
                            <div className="space-y-2">;
                            <Input;
                            type="search";
                            placeholder="Search plugins...";
                        value={searchQuery}
                    onChange={(e) => setSearchQuery(e.target.value)}
                    />;
                    </div>;
                    <div className="max-h-[300px] overflow-y-auto space-y-2">;
                    {filteredPlugins.length == 0 ? (;
                    <p className="text-sm text-muted-foreground">No plugins found.</p>;
                    ) : (
                    filteredPlugins.map((plugin) => (;
                    <Button;
                key={plugin}
                variant="ghost";
                className="w-full justify-start font-normal";
                onClick={() => {
                    handlePluginAdd(plugin);
                    setSearchQuery('');
                    setIsDialogOpen(false);
                }}
                >;
            {plugin}
            </Button>;
            ));
        )}
        </div>;
        </div>;
        </DialogContent>;
        </Dialog>;
        </div>;
        {hasChanged && (;
        <p className="text-xs text-yellow-500">Plugins configuration has been changed</p>;
    )}
    </div>;
    )}
    </div>;
    </div>;
    </div>;
    );

}

} // namespace elizaos
