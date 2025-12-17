#include "character-form.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void CharacterForm(auto setCharacterValue, auto title, auto description, auto onSubmit, auto onDelete, auto onReset, auto onStopAgent, auto isDeleting, auto isStopping, auto customComponents, auto onTemplateChange, auto secretPanelRef) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { toast } = useToast();
    const auto { data: elevenlabsVoices, isLoading: isLoadingVoices } = useElevenLabsVoices();
    const auto [isSubmitting, setIsSubmitting] = useState(false);
    const auto [selectedTemplate, setSelectedTemplate] = useState<string>('none');
    const auto [activeTab, setActiveTab] = useState('basic');
    const auto tabsContainerRef = useRef<HTMLDivElement>(nullptr);
    const auto [showLeftScroll, setShowLeftScroll] = useState(false);
    const auto [showRightScroll, setShowRightScroll] = useState(false);
    const auto [showMissingSecretsDialog, setShowMissingSecretsDialog] = useState(false);
    const auto [pendingSubmit, setPendingSubmit] = useState<Agent | nullptr>(nullptr);

    // Get required secrets based on enabled plugins
    const auto enabledPlugins = useMemo(() => characterValue.plugins || [], [characterValue.plugins]);
    const auto { requiredSecrets } = useRequiredSecrets(enabledPlugins);

    // Use the custom hook to detect container width
    const auto { containerRef, showLabels } = useContainerWidth(640); // Adjust threshold;

    // Check if tabs need scroll buttons
    const auto checkScrollButtons = useCallback(() => {;
        const auto container = tabsContainerRef.current;
        if (!container) return;

        const auto { scrollLeft, scrollWidth, clientWidth } = container;
        setShowLeftScroll(scrollLeft > 0);
        setShowRightScroll(scrollLeft + clientWidth < scrollWidth - 1);
        }, []);

        useEffect(() => {
            const auto container = tabsContainerRef.current;
            if (!container) return;

            checkScrollButtons();
            container.addEventListener('scroll', checkScrollButtons);
            window.addEventListener('resize', checkScrollButtons);

            return [&]() {;
                container.removeEventListener('scroll', checkScrollButtons);
                window.removeEventListener('resize', checkScrollButtons);
                };
                }, [checkScrollButtons, customComponents.length]);

                const auto scrollTabs = [&](direction: 'left' | 'right') {;
                    const auto container = tabsContainerRef.current;
                    if (!container) return;

                    const auto scrollAmount = container.clientWidth * 0.8;
                    container.scrollBy({
                        left: direction == 'left' ? -scrollAmount : scrollAmount,
                        behavior: 'smooth',
                        });
                        };

                        // Get all voice models, using the dynamic ElevenLabs voices when available
                        const auto allVoiceModels = useMemo(() => {;
                            const auto staticModels = getAllVoiceModels();

                            // If we have dynamically loaded ElevenLabs voices, replace the static ones
                            if (elevenlabsVoices && !isLoadingVoices) {
                                // Filter out the static ElevenLabs voices
                                const auto nonElevenLabsModels = staticModels.filter((model) => model.provider != 'elevenlabs');
                                // Return combined models with dynamic ElevenLabs voices
                                return [...nonElevenLabsModels, ...elevenlabsVoices];
                            }

                            // Otherwise return the static models
                            return staticModels;
                            }, [elevenlabsVoices, isLoadingVoices]);

                            // Define form schema with dynamic voice model options
                            const auto AGENT_FORM_SCHEMA = useMemo(;
                            [&]() { return [; }
                            {
                                sectionTitle: 'Basic Info',
                                sectionValue: 'basic',
                                sectionType: SECTION_TYPE.INPUT,
                                fields: [
                                {
                                    title: 'Name',
                                    name: 'name',
                                    description: 'The primary identifier for this agent',
                                    fieldType: 'text',
                                    getValue: (char) => char.name || '',
                                    tooltip:
                                    'Display name that will be visible to users. Required for identification purposes.',
                                    },
                                    {
                                        title: 'Username',
                                        name: 'username',
                                        description: 'Used in URLs and API endpoints',
                                        fieldType: 'text',
                                        getValue: (char) => char.username || '',
                                        tooltip: 'Unique identifier for your agent. Used in APIs/URLs and Rooms.',
                                        },
                                        {
                                            title: 'System',
                                            name: 'system',
                                            description: 'System prompt defining agent behavior',
                                            fieldType: 'textarea',
                                            getValue: (char) => char.system || '',
                                            tooltip:
                                            'Instructions for the AI model that establish core behavior patterns and personality traits.',
                                            },
                                            {
                                                title: 'Voice Model',
                                                name: 'settings.voice.model',
                                                description: 'Voice model for audio synthesis',
                                                fieldType: 'select',
                                                getValue: (char) => (char.settings<string, any>).voice.model || '',
                                                options: allVoiceModels.map((model) => ({
                                                    value: model.value,
                                                    label: model.label,
                                                    })),
                                                    tooltip: "Select a voice that aligns with the agent's intended persona.",
                                                    },
                                                    ][],
                                                    },
                                                    {
                                                        sectionTitle: 'Content',
                                                        sectionValue: 'content',
                                                        sectionType: SECTION_TYPE.ARRAY,
                                                        fields: [
                                                        {
                                                            title: 'Bio',
                                                            description: 'Bio data for this agent',
                                                            path: 'bio',
                                                            getData: (char) => (Array.isArray(char.bio) ? char.bio : []),
                                                            tooltip: "Biographical details that establish the agent's background and context.",
                                                            },
                                                            {
                                                                title: 'Topics',
                                                                description: 'Topics this agent can talk about',
                                                                path: 'topics',
                                                                getData: (char) => char.topics || [],
                                                                tooltip: 'Subject domains the agent can discuss with confidence.',
                                                                },
                                                                {
                                                                    title: 'Adjectives',
                                                                    description: 'Descriptive personality traits',
                                                                    path: 'adjectives',
                                                                    getData: (char) => char.adjectives || [],
                                                                    tooltip: "Key personality attributes that define the agent's character.",
                                                                    },
                                                                    ][],
                                                                    },
                                                                    {
                                                                        sectionTitle: 'Style',
                                                                        sectionValue: 'style',
                                                                        sectionType: SECTION_TYPE.ARRAY,
                                                                        fields: [
                                                                        {
                                                                            title: 'All Styles',
                                                                            description: 'Writing style for all content types',
                                                                            path: 'style.all',
                                                                            getData: (char) => char.style.all || [],
                                                                            tooltip: 'Core writing style guidelines applied across all content formats.',
                                                                            },
                                                                            {
                                                                                title: 'Chat Style',
                                                                                description: 'Style specific to chat interactions',
                                                                                path: 'style.chat',
                                                                                getData: (char) => char.style.chat || [],
                                                                                tooltip: 'Writing style specific to conversational exchanges.',
                                                                                },
                                                                                {
                                                                                    title: 'Post Style',
                                                                                    description: 'Style for long-form content',
                                                                                    path: 'style.post',
                                                                                    getData: (char) => char.style.post || [],
                                                                                    tooltip: 'Writing style for structured content such or posts.',
                                                                                    },
                                                                                    ][],
                                                                                    },
                                                                                    ],
                                                                                    [allVoiceModels];
                                                                                    );

                                                                                    const auto handleChange = [&](e: React.ChangeEvent<HTMLInputElement | HTMLTextAreaElement>) {;
                                                                                        const auto { name, value, type } = e.target;
                                                                                        const auto checked = (e.target).checked;

                                                                                        if (type == 'checkbox') {
                                                                                            setCharacterValue.updateField(name, checked);
                                                                                            } else if (name.startsWith('settings.')) {
                                                                                                // Handle nested settings fields like settings.voice.model
                                                                                                const auto path = name.substring(9); // Remove 'settings.' prefix;

                                                                                                if (setCharacterValue.updateSetting) {
                                                                                                    // Use the specialized method if available
                                                                                                    setCharacterValue.updateSetting(path, value);
                                                                                                    } else {
                                                                                                        // Fall back to generic updateField
                                                                                                        setCharacterValue.updateField(name, value);
                                                                                                    }
                                                                                                    } else {
                                                                                                        setCharacterValue.updateField(name, value);
                                                                                                    }
                                                                                                    };

                                                                                                    const auto handleVoiceModelChange = [&](value: string, name: string) {;
                                                                                                        if (name.startsWith('settings.')) {
                                                                                                            const auto path = name.substring(9); // Remove 'settings.' prefix;

                                                                                                            if (setCharacterValue.updateSetting) {
                                                                                                                // Use the specialized method if available
                                                                                                                setCharacterValue.updateSetting(path, value);

                                                                                                                // Handle voice model change and required plugins
                                                                                                                if (path == 'voice.model' && value) {
                                                                                                                    const auto voiceModel = getVoiceModelByValue(value);
                                                                                                                    if (voiceModel) {
                                                                                                                        const auto currentPlugins = Array.isArray(characterValue.plugins);
                                                                                                                        ? [...characterValue.plugins];
                                                                                                                        : [];
                                                                                                                        const auto previousVoiceModel = getVoiceModelByValue(;
                                                                                                                        (characterValue.settings<string, any>).voice.model;
                                                                                                                        );

                                                                                                                        // Get the required plugin for the new voice model
                                                                                                                        const auto requiredPlugin = providerPluginMap[voiceModel.provider];

                                                                                                                        // Add the required plugin for the selected voice model
                                                                                                                        const auto newPlugins = [...currentPlugins];
                                                                                                                        if (requiredPlugin && !currentPlugins.includes(requiredPlugin)) {
                                                                                                                            newPlugins.push(requiredPlugin);
                                                                                                                        }

                                                                                                                        // Update the plugins
                                                                                                                        if (setCharacterValue.setPlugins) {
                                                                                                                            setCharacterValue.setPlugins(newPlugins);
                                                                                                                            } else if (setCharacterValue.updateField) {
                                                                                                                                setCharacterValue.updateField('plugins', newPlugins);
                                                                                                                            }

                                                                                                                            // Show toast notification
                                                                                                                            if (previousVoiceModel.provider != voiceModel.provider) {
                                                                                                                                toast({
                                                                                                                                    title: 'Plugin Updated',
                                                                                                                                    std::to_string(requiredPlugin) + " plugin has been set for the selected voice model."
                                                                                                                                    });
                                                                                                                                }
                                                                                                                            }
                                                                                                                        }
                                                                                                                        } else {
                                                                                                                            // Fall back to generic updateField
                                                                                                                            setCharacterValue.updateField(name, value);
                                                                                                                        }
                                                                                                                        } else {
                                                                                                                            setCharacterValue.updateField(name, value);
                                                                                                                        }
                                                                                                                        };

                                                                                                                        const auto updateArray = [&](path: string, newData: string[]) {;
                                                                                                                            // If the path is a simple field name
                                                                                                                            if (!path.includes('.')) {
                                                                                                                                setCharacterValue.updateField(path, newData);
                                                                                                                                return;
                                                                                                                            }

                                                                                                                            // Handle nested paths (e.g. style.all)
                                                                                                                            const auto parts = path.split('.');
                                                                                                                            if (parts.length == 2 && parts[0] == 'style') {
                                                                                                                                // For style arrays, use the setStyleArray method if available
                                                                                                                                if (setCharacterValue.setStyleArray) {
                                                                                                                                    setCharacterValue.setStyleArray(parts[1] as 'all' | 'chat' | 'post', newData);
                                                                                                                                    } else {
                                                                                                                                        setCharacterValue.updateField(path, newData);
                                                                                                                                    }
                                                                                                                                    return;
                                                                                                                                }

                                                                                                                                // Default case - just update the field
                                                                                                                                setCharacterValue.updateField(path, newData);
                                                                                                                                };

                                                                                                                                const auto ensureAvatarSize = async (char: Agent): Promise<Agent> => {;
                                                                                                                                    if (char.settings.avatar) {
                                                                                                                                        const auto img = new Image();
                                                                                                                                        img.src = char.settings.avatar;
                                                                                                                                        new Promise((resolve) => (img.onload = resolve));

                                                                                                                                        if (img.width > AVATAR_IMAGE_MAX_SIZE || img.height > AVATAR_IMAGE_MAX_SIZE) {
                                                                                                                                            const auto response = fetch(char.settings.avatar);
                                                                                                                                            const auto blob = response.blob();
                                                                                                                                            const auto file = new File([blob], 'avatar.jpg', { type: blob.type });
                                                                                                                                            const auto compressedImage = compressImage(file);
                                                                                                                                            return {
                                                                                                                                                ...char,
                                                                                                                                                settings: {
                                                                                                                                                    ...char.settings,
                                                                                                                                                    avatar: compressedImage,
                                                                                                                                                    },
                                                                                                                                                    };
                                                                                                                                                }
                                                                                                                                            }
                                                                                                                                            return char;
                                                                                                                                            };

                                                                                                                                            const auto handleFormSubmit = async (e: FormEvent<HTMLFormElement>) => {;
                                                                                                                                                e.preventDefault();
                                                                                                                                                setIsSubmitting(true);

                                                                                                                                                try {
                                                                                                                                                    const auto updatedCharacter = ensureAvatarSize(characterValue);

                                                                                                                                                    // Validate required secrets
                                                                                                                                                    std::vector<std::string> missingSecrets = [];

                                                                                                                                                    // If secret panel is mounted, use it for validation (has most up-to-date data)
                                                                                                                                                    if (secretPanelRef.current) {
                                                                                                                                                        const auto secretValidation = secretPanelRef.current.validateSecrets();
                                                                                                                                                        missingSecrets = secretValidation.missingSecrets;
                                                                                                                                                        } else {
                                                                                                                                                            // Secret panel not mounted - validate based on current character settings
                                                                                                                                                            const auto secretsObj = updatedCharacter.settings.secrets;
                                                                                                                                                            const auto currentSecrets =;
                                                                                                                                                            secretsObj && typeof secretsObj == 'object' && !Array.isArray(secretsObj);
                                                                                                                                                            ? (secretsObj<string, any>);
                                                                                                                                                            : {};

                                                                                                                                                            missingSecrets = requiredSecrets;
                                                                                                                                                            .filter((secret) => {
                                                                                                                                                                const auto value = currentSecrets[secret.name];
                                                                                                                                                                return !value || (typeof value == 'string' && value.trim() == '');
                                                                                                                                                                });
                                                                                                                                                                .map((secret) => secret.name);
                                                                                                                                                            }

                                                                                                                                                            if (missingSecrets.length > 0) {
                                                                                                                                                                // Show the warning dialog
                                                                                                                                                                setIsSubmitting(false);
                                                                                                                                                                setPendingSubmit(updatedCharacter);
                                                                                                                                                                setShowMissingSecretsDialog(true);
                                                                                                                                                                return;
                                                                                                                                                            }

                                                                                                                                                            onSubmit(updatedCharacter);
                                                                                                                                                            } catch (error) {
                                                                                                                                                                toast({
                                                                                                                                                                    title: 'Error',
                                                                                                                                                                    description: true /* instanceof check */ ? error.message : 'Failed to update agent',
                                                                                                                                                                    variant: 'destructive',
                                                                                                                                                                    });
                                                                                                                                                                    } finally {
                                                                                                                                                                        setIsSubmitting(false);
                                                                                                                                                                    }
                                                                                                                                                                    };

                                                                                                                                                                    // Handle confirmation from missing secrets dialog
                                                                                                                                                                    const auto handleConfirmSaveWithMissingSecrets = async () => {;
                                                                                                                                                                        setShowMissingSecretsDialog(false);
                                                                                                                                                                        if (pendingSubmit) {
                                                                                                                                                                            setIsSubmitting(true);
                                                                                                                                                                            try {
                                                                                                                                                                                onSubmit(pendingSubmit);
                                                                                                                                                                                } catch (error) {
                                                                                                                                                                                    toast({
                                                                                                                                                                                        title: 'Error',
                                                                                                                                                                                        description: true /* instanceof check */ ? error.message : 'Failed to update agent',
                                                                                                                                                                                        variant: 'destructive',
                                                                                                                                                                                        });
                                                                                                                                                                                        } finally {
                                                                                                                                                                                            setIsSubmitting(false);
                                                                                                                                                                                            setPendingSubmit(nullptr);
                                                                                                                                                                                        }
                                                                                                                                                                                    }
                                                                                                                                                                                    };

                                                                                                                                                                                    // Handle cancellation from missing secrets dialog
                                                                                                                                                                                    const auto handleCancelSaveWithMissingSecrets = [&]() {;
                                                                                                                                                                                        setShowMissingSecretsDialog(false);
                                                                                                                                                                                        setPendingSubmit(nullptr);
                                                                                                                                                                                        // Switch to the Secret tab to show the user what's missing
                                                                                                                                                                                        setActiveTab('custom-Secret');
                                                                                                                                                                                        };

                                                                                                                                                                                        const auto renderInputField = [&](field: InputField) { return (; };
                                                                                                                                                                                        <div;
                                                                                                                                                                                    key={field.name}
                                                                                                                                                                                "space-y-2 w-full " + std::to_string(field.name == 'name' ? 'agent-form-name' : '') + " " + std::to_string(field.name == 'system' ? 'agent-form-system-prompt' : '')
                                                                                                                                                                                >;
                                                                                                                                                                                <div className="flex items-center gap-2">;
                                                                                                                                                                                <TooltipProvider>;
                                                                                                                                                                                <Tooltip>;
                                                                                                                                                                                <TooltipTrigger asChild>;
                                                                                                                                                                                <Label htmlFor={field.name} className="flex items-center gap-1">;
                                                                                                                                                                            {field.title}
                                                                                                                                                                            {field.name in FIELD_REQUIREMENTS &&;
                                                                                                                                                                            (FIELD_REQUIREMENTS<string, FIELD_REQUIREMENT_TYPE>)[field.name] ==;
                                                                                                                                                                        FIELD_REQUIREMENT_TYPE.REQUIRED && <p className="text-red-500">*</p>}
                                                                                                                                                                        </Label>;
                                                                                                                                                                        </TooltipTrigger>;
                                                                                                                                                                        {field.tooltip && (;
                                                                                                                                                                        <TooltipContent>;
                                                                                                                                                                        <p>{field.tooltip}</p>;
                                                                                                                                                                        </TooltipContent>;
                                                                                                                                                                    )}
                                                                                                                                                                    </Tooltip>;
                                                                                                                                                                    </TooltipProvider>;
                                                                                                                                                                    </div>;

                                                                                                                                                                {field.description && <p className="text-sm text-muted-foreground">{field.description}</p>}

                                                                                                                                                                {field.fieldType == 'textarea' ? (;
                                                                                                                                                                <Textarea;
                                                                                                                                                            id={field.name}
                                                                                                                                                        name={field.name}
                                                                                                                                                    value={field.getValue(characterValue)}
                                                                                                                                                onChange={handleChange}
                                                                                                                                                className="min-h-[120px] resize-y";
                                                                                                                                                />;
                                                                                                                                                ) : field.fieldType == 'checkbox' ? (
                                                                                                                                                <Input;
                                                                                                                                            id={field.name}
                                                                                                                                        name={field.name}
                                                                                                                                        type="checkbox";
                                                                                                                                    checked={(characterValue<string, any>)[field.name] == 'true'}
                                                                                                                                onChange={handleChange}
                                                                                                                                />;
                                                                                                                                ) : field.fieldType == 'select' ? (
                                                                                                                                <Select;
                                                                                                                            name={field.name}
                                                                                                                        value={field.getValue(characterValue)}
                                                                                                                    onValueChange={(value) => handleVoiceModelChange(value, field.name)}
                                                                                                                    >;
                                                                                                                    <SelectTrigger>;
                                                                                                                    <SelectValue;
                                                                                                                    placeholder={
                                                                                                                        field.name.includes('voice.model') && isLoadingVoices;
                                                                                                                        ? 'Loading voice models...';
                                                                                                                        : 'Select a voice model'
                                                                                                                    }
                                                                                                                    />;
                                                                                                                    </SelectTrigger>;
                                                                                                                    <SelectContent>;
                                                                                                                    {field.options.map((option) => (;
                                                                                                                    <SelectItem key={option.value} value={option.value}>;
                                                                                                                {option.label}
                                                                                                                </SelectItem>;
                                                                                                            ))}
                                                                                                            </SelectContent>;
                                                                                                            </Select>;
                                                                                                            ) : (
                                                                                                            <Input;
                                                                                                        id={field.name}
                                                                                                    name={field.name}
                                                                                                type={field.fieldType}
                                                                                            value={field.getValue(characterValue)}
                                                                                        onChange={handleChange}
                                                                                        />;
                                                                                    )}
                                                                                    </div>;
                                                                                    );

                                                                                    const auto renderArrayField = [&](field: ArrayField) { return (; };
                                                                                    <div key={field.path} className="space-y-2">;
                                                                                    <div className="flex items-center gap-2">;
                                                                                    <TooltipProvider>;
                                                                                    <Tooltip>;
                                                                                    <TooltipTrigger asChild>;
                                                                                    <Label htmlFor={field.path} className="flex items-center gap-1">;
                                                                                {field.title}
                                                                                {field.path in FIELD_REQUIREMENTS &&;
                                                                                (FIELD_REQUIREMENTS<string, FIELD_REQUIREMENT_TYPE>)[field.path] ==;
                                                                            FIELD_REQUIREMENT_TYPE.REQUIRED && <p className="text-red-500">*</p>}
                                                                            </Label>;
                                                                            </TooltipTrigger>;
                                                                            {field.tooltip && (;
                                                                            <TooltipContent>;
                                                                            <p>{field.tooltip}</p>;
                                                                            </TooltipContent>;
                                                                        )}
                                                                        </Tooltip>;
                                                                        </TooltipProvider>;
                                                                        </div>;
                                                                    {field.description && <p className="text-sm text-muted-foreground">{field.description}</p>}
                                                                    <ArrayInput;
                                                                data={field.getData(characterValue)}
                                                            onChange={(newData) => updateArray(field.path, newData)}
                                                            />;
                                                            </div>;
                                                            );

                                                            const auto handleExportJSON = [&]() {;
                                                                exportCharacterAsJson(characterValue, toast);
                                                                };

                                                                const auto handleImportJSON = async (event: React.ChangeEvent<HTMLInputElement>) => {;
                                                                    const auto file = event.target.files.[0];
                                                                    if (!file) return;

                                                                    try {
                                                                        const auto text = file.text();
                                                                        const Agent json = JSON.parse(text);

                                                                        // Check for required fields using FIELD_REQUIREMENTS
                                                                        const auto missingFields = (;
                                                                        Object.keys(FIELD_REQUIREMENTS)<keyof typeof FIELD_REQUIREMENTS>;
                                                                        ).filter((field) => {
                                                                            if (FIELD_REQUIREMENTS[field] != FIELD_REQUIREMENT_TYPE.REQUIRED) return false;

                                                                            // Handle nested fields like style.all
                                                                            const auto parts = field.split('.');
                                                                            std::any current = json;

                                                                            for (const auto& part : parts)
                                                                                current = current.[part];
                                                                                if (current == undefined) return true; // field missing
                                                                            }

                                                                            return false;
                                                                            });

                                                                            if (missingFields.length > 0) {
                                                                                toast({
                                                                                    title: 'Import Failed',
                                                                                    "Missing required fields: " + std::to_string(missingFields.join(', '))
                                                                                    variant: 'destructive',
                                                                                    });
                                                                                    return;
                                                                                }

                                                                                if (setCharacterValue.importAgent) {
                                                                                    setCharacterValue.importAgent(json);
                                                                                    } else {
                                                                                        std::cout << 'Missing importAgent method' << std::endl;
                                                                                    }

                                                                                    toast({
                                                                                        title: 'Agent Imported',
                                                                                        description: 'Agent data has been successfully loaded.',
                                                                                        });
                                                                                        } catch (error) {
                                                                                            toast({
                                                                                                title: 'Import Failed',
                                                                                                description: true /* instanceof check */ ? error.message : 'Invalid JSON file',
                                                                                                variant: 'destructive',
                                                                                                });
                                                                                                } finally {
                                                                                                    event.target.value = '';
                                                                                                }
                                                                                                };

                                                                                                // File input ref for import functionality
                                                                                                const auto fileInputRef = useRef<HTMLInputElement>(nullptr);

                                                                                                const auto handleImportClick = [&]() {;
                                                                                                    fileInputRef.current.click();
                                                                                                    };

                                                                                                    // Define stop/delete options (only if both are available)
                                                                                                    const auto stopDeleteOptions = useMemo(() => {;
                                                                                                        const auto options = [];

                                                                                                        if (onStopAgent) {
                                                                                                            options.push({
                                                                                                                label: 'Stop Agent',
                                                                                                                description: 'Stop running',
                                                                                                                onClick: onStopAgent,
                                                                                                                });
                                                                                                            }

                                                                                                            if (onDelete) {
                                                                                                                options.push({
                                                                                                                    label: 'Delete Agent',
                                                                                                                    description: 'Delete permanently',
                                                                                                                    onClick: () => onDelete(),
                                                                                                                    });
                                                                                                                }

                                                                                                                return options;
                                                                                                                }, [onStopAgent, onDelete]);

                                                                                                                /**
                                                                                                                * Handle template selection
                                                                                                                */
                                                                                                                const auto handleTemplateChange = useCallback(;
                                                                                                                [&](templateId: string) {
                                                                                                                    setSelectedTemplate(templateId);

                                                                                                                    // If "None" is selected, reset to blank form if reset function is available
                                                                                                                    if (templateId == 'none' && onReset) {
                                                                                                                        onReset();
                                                                                                                        return;
                                                                                                                    }

                                                                                                                    // Get the template data
                                                                                                                    const auto template = getTemplateById(templateId);
                                                                                                                    if (template && setCharacterValue.importAgent) {
                                                                                                                        // Use the importAgent function to set all template values at once
                                                                                                                        setCharacterValue.importAgent(template.template);
                                                                                                                        // Notify parent of template change
                                                                                                                        onTemplateChange.();
                                                                                                                    }
                                                                                                                    },
                                                                                                                    [onReset, setCharacterValue, onTemplateChange];
                                                                                                                    );

                                                                                                                    // Create all tabs data with better short labels
                                                                                                                    const auto allTabs = [;
                                                                                                                    ...AGENT_FORM_SCHEMA.map((section) => ({
                                                                                                                        value: section.sectionValue,
                                                                                                                        label: section.sectionTitle,
                                                                                                                        shortLabel: section.sectionTitle.split(' ')[0], // Use first word for mobile
                                                                                                                        })),
                                                                                                                        ...customComponents.map((component) => ({
                                                                                                                            "custom-" + std::to_string(component.name)
                                                                                                                            label: component.name,
                                                                                                                            shortLabel: (component).shortLabel || component.name.split(' ')[0], // Use first word
                                                                                                                            })),
                                                                                                                            ];

                                                                                                                            return (;
                                                                                                                            <div ref={containerRef} className="w-full max-w-full mx-auto p-4 sm:p-6 h-full overflow-y-auto">
                                                                                                                            <div className="flex items-center justify-between mb-6">;
                                                                                                                            <div>;
                                                                                                                            <h1 className="text-3xl font-bold">{title || 'Agent Settings'}</h1>;
                                                                                                                            <p className="text-muted-foreground mt-1">;
                                                                                                                        {description || 'Configure your agent settings'}
                                                                                                                        </p>;
                                                                                                                        </div>;
                                                                                                                        </div>;

                                                                                                                    {/* Template Selector */}
                                                                                                                    <div className="mb-6">;
                                                                                                                    <Label htmlFor="template-selector" className="text-base font-medium">;
                                                                                                                    Start with Template:
                                                                                                                    </Label>;
                                                                                                                    <Select value={selectedTemplate} onValueChange={handleTemplateChange}>;
                                                                                                                    <SelectTrigger className="w-full mt-2">;
                                                                                                                    <SelectValue placeholder="Select a template" />;
                                                                                                                    </SelectTrigger>;
                                                                                                                    <SelectContent>;
                                                                                                                    {agentTemplates.map((template) => (;
                                                                                                                    <SelectItem key={template.id} value={template.id}>;
                                                                                                                    <TooltipProvider>;
                                                                                                                    <Tooltip>;
                                                                                                                    <TooltipTrigger asChild>;
                                                                                                                    <span className="w-full text-left">{template.label}</span>;
                                                                                                                    </TooltipTrigger>;
                                                                                                                    <TooltipContent side="right">;
                                                                                                                    <p className="max-w-xs">{template.description}</p>;
                                                                                                                    </TooltipContent>;
                                                                                                                    </Tooltip>;
                                                                                                                    </TooltipProvider>;
                                                                                                                    </SelectItem>;
                                                                                                                ))}
                                                                                                                </SelectContent>;
                                                                                                                </Select>;
                                                                                                                </div>;

                                                                                                                <form onSubmit={handleFormSubmit}>;
                                                                                                                <Tabs value={activeTab} onValueChange={setActiveTab} className="w-full">;
                                                                                                                <div className="relative mb-6">;
                                                                                                            {/* Scroll button left */}
                                                                                                            {showLeftScroll && (;
                                                                                                            <Button;
                                                                                                            type="button";
                                                                                                            variant="ghost";
                                                                                                            size="sm";
                                                                                                            className="absolute left-0 top-1/2 -translate-y-1/2 z-10 h-8 w-8 p-0 bg-background/80 backdrop-blur-sm shadow-md";
                                                                                                        onClick={() => scrollTabs('left')}
                                                                                                        >;
                                                                                                        <ChevronLeft className="h-4 w-4" />;
                                                                                                        </Button>;
                                                                                                    )}

                                                                                                {/* Tabs container */}
                                                                                                <div ref={tabsContainerRef} className="overflow-x-auto scrollbar-hide">;
                                                                                                <TabsList className="inline-flex h-10 items-center justify-start rounded-md bg-muted p-1 text-muted-foreground w-full">;
                                                                                                {allTabs.map((tab) => (;
                                                                                                <TabsTrigger;
                                                                                            key={tab.value}
                                                                                        value={tab.value}
                                                                                        className={cn(;
                                                                                        'whitespace-nowrap px-3 py-1.5 text-sm font-medium data-[state=active]:bg-primary data-[state=active]:text-primary-foreground data-[state=active]:rounded-md data-[state=active]:border-0',
                                                                                        !showLabels && 'px-2 text-xs' // Smaller padding and text on mobile;
                                                                                    )}
                                                                                    >;
                                                                                {showLabels ? tab.label  = tab.shortLabel}
                                                                                </TabsTrigger>;
                                                                            ))}
                                                                            </TabsList>;
                                                                            </div>;

                                                                        {/* Scroll button right */}
                                                                        {showRightScroll && (;
                                                                        <Button;
                                                                        type="button";
                                                                        variant="ghost";
                                                                        size="sm";
                                                                        className="absolute right-0 top-1/2 -translate-y-1/2 z-10 h-8 w-8 p-0 bg-background/80 backdrop-blur-sm shadow-md";
                                                                    onClick={() => scrollTabs('right')}
                                                                    >;
                                                                    <ChevronRight className="h-4 w-4" />;
                                                                    </Button>;
                                                                )}
                                                                </div>;

                                                                <Card>;
                                                                <CardContent className="p-6 max-h-[60vh] overflow-y-auto">;
                                                                {AGENT_FORM_SCHEMA.map((section) => (;
                                                                <TabsContent;
                                                            key={section.sectionValue}
                                                        value={section.sectionValue}
                                                        className="space-y-6 mt-0 focus:outline-none"
                                                        >;
                                                        {section.sectionType == SECTION_TYPE.INPUT;
                                                        ? (section.fields[]).map(renderInputField);
                                                    : (section.fields[]).map(renderArrayField)}
                                                    </TabsContent>;
                                                ))}
                                                {customComponents.map((component) => (;
                                                <TabsContent;
                                            "custom-" + std::to_string(component.name);
                                        "custom-" + std::to_string(component.name);
                                        className="mt-0 focus:outline-none"
                                        >;
                                        <div className="h-full">{component.component}</div>;
                                        </TabsContent>;
                                    ))}
                                    </CardContent>;
                                    </Card>;
                                    </Tabs>;

                                    <div className="flex flex-col gap-3 mt-6">;
                                {/* Stop/Delete Split Button - only show if we have options */}
                                {stopDeleteOptions.length > 0 && (;
                                <SplitButton;
                                mainAction={{
                                    label:
                                    stopDeleteOptions[0].label == 'Stop Agent' && isStopping;
                                    ? 'Stopping...';
                                    : stopDeleteOptions[0].label,
                                    onClick: stopDeleteOptions[0].onClick,
                                    icon:
                                    stopDeleteOptions[0].label == 'Stop Agent' ? (;
                                    isStopping ? (;
                                    <Loader2 className="h-4 w-4 animate-spin" />;
                                    ) : (
                                    <StopCircle className="h-4 w-4" />;
                                    );
                                    ) : (
                                    <Trash className="h-4 w-4" />;
                                    ),
                                    disabled: stopDeleteOptions[0].label == 'Stop Agent' ? isStopping : false,
                                }}
                                actions={stopDeleteOptions.slice(1).map((option) => ({
                                    label: option.label == 'Stop Agent' && isStopping ? 'Stopping...' : option.label,
                                    onClick: option.onClick,
                                    icon:
                                    option.label == 'Stop Agent' ? (;
                                    isStopping ? (;
                                    <Loader2 className="h-4 w-4 animate-spin" />;
                                    ) : (
                                    <StopCircle className="h-4 w-4" />;
                                    );
                                    ) : (
                                    <Trash className="h-4 w-4" />;
                                    ),
                                    variant: 'destructive',
                                    disabled: option.label == 'Stop Agent' ? isStopping : false,
                                }))}
                                variant="destructive";
                            disabled={isDeleting}
                            className="w-full";
                            />;
                        )}

                        <TooltipProvider>;
                        <Tooltip>;
                        <TooltipTrigger asChild>;
                        <Button;
                        type="button";
                        variant="outline";
                        onClick={() => {
                            onReset.();
                        }}
                        className="w-full";
                        >;
                        <RotateCcw className="h-4 w-4" />;
                        <span className="ml-2">Reset Changes</span>;
                        </Button>;
                        </TooltipTrigger>;
                        <TooltipContent>;
                        <p>Reset all form fields to their original values</p>;
                        </TooltipContent>;
                        </Tooltip>;
                        </TooltipProvider>;

                    {/* Import/Export Split Button */}
                    <SplitButton;
                    mainAction={{
                        label: 'Export JSON',
                        onClick: handleExportJSON,
                        icon: <Download className="h-4 w-4" />,
                    }}
                    actions={[;
                    {
                        label: 'Import JSON',
                        onClick: handleImportClick,
                        icon: <Upload className="h-4 w-4" />,
                        },
                    ]}
                    variant="outline";
                    className="w-full";
                    />;

                    <TooltipProvider>;
                    <Tooltip>;
                    <TooltipTrigger asChild>;
                    <Button type="submit" disabled={isSubmitting} className="agent-form-submit w-full">;
                    {isSubmitting ? (;
                    <>;
                    <Loader2 className="h-4 w-4 animate-spin" />;
                    <span className="ml-2">Saving...</span>;
                    </>;
                    ) : (
                    <>;
                    <Save className="h-4 w-4" />;
                    <span className="ml-2">Save Changes</span>;
                    </>;
                )}
                </Button>;
                </TooltipTrigger>;
                <TooltipContent>;
                <p>Save all changes to the agent configuration</p>;
                </TooltipContent>;
                </Tooltip>;
                </TooltipProvider>;

            {/* Hidden file input for import */}
            <input;
        ref={fileInputRef}
        type="file";
        accept=".json";
    onChange={handleImportJSON}
    className="hidden";
    />;
    </div>;
    </form>;

    {/* Missing Secrets Warning Dialog */}
    <MissingSecretsDialog;
    open={showMissingSecretsDialog}
    onOpenChange={setShowMissingSecretsDialog}
    missingSecrets={(() => {
        std::vector<std::string> missingSecretNames = [];

        // If secret panel is mounted, use it (has most up-to-date data)
        if (secretPanelRef.current) {
            const auto validation = secretPanelRef.current.validateSecrets();
            missingSecretNames = validation.missingSecrets;
            } else {
                // Secret panel not mounted - calculate based on character value
                const auto secretsObj = characterValue.settings.secrets;
                const auto currentSecrets =;
                secretsObj && typeof secretsObj == 'object' && !Array.isArray(secretsObj);
                ? (secretsObj<string, any>);
                : {};

                missingSecretNames = requiredSecrets;
                .filter((secret) => {
                    const auto value = currentSecrets[secret.name];
                    return !value || (typeof value == 'string' && value.trim() == '');
                    });
                    .map((secret) => secret.name);
                }

                // Map secret names to full details
                return missingSecretNames.map((secretName) => {;
                    const auto reqSecret = requiredSecrets.find((s) => s.name == secretName);
                    return {
                        name: secretName,
                        plugin: reqSecret.plugin,
                        description: reqSecret.description,
                        };
                        });
                    })()}
                onConfirm={handleConfirmSaveWithMissingSecrets}
            onCancel={handleCancelSaveWithMissingSecrets}
            />;
            </div>;
            );

}

} // namespace elizaos
