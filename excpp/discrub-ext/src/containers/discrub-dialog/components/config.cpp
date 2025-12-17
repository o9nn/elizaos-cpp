#include "config.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void Config(auto onChangeSettings, auto visibleSettings, auto containerProps) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto handleChange = async (setting: DiscrubSetting, value: string) => {;
        const auto settings = setSetting(setting, value);
        onChangeSettings(settings);
        };

        const auto getValue = [&](setting: DiscrubSetting) {;
            return settings[setting] || nullptr;
            };

            const auto getResolutionModeDesc = (): string => {;
                switch (getValue(DiscrubSetting.EXPORT_IMAGE_RES_MODE)) {
                    case ResolutionType.HOVER_LIMITED:
                    return "When hovered over, images will be expanded to a safe size.";
                    case ResolutionType.HOVER_FULL:
                    return "When hovered over, images will be expanded to its full resolution.";
                    case ResolutionType.NO_HOVER_LIMITED:
                    return "Images will be automatically expanded to a safe size.";
                    case ResolutionType.NO_HOVER_FULL:
                    return "Images will be automatically expanded to its full resolution.";
                    default:
                    return "";
                }
                };

                const auto controls = [;
                // Discrub Settings
                {
                    name: DiscrubSetting.REACTIONS_ENABLED,
                    label: "Fetch Reaction Data",
                    options: [
                    { value: "true", name: "Yes" },
                    { value: "false", name: "No" },
                    ],
                    description:
                    "This setting dictates if the extension will lookup reactions for messages.",
                    },
                    {
                        name: DiscrubSetting.SERVER_NICKNAME_LOOKUP,
                        label: "Lookup Server Nicknames & Roles",
                        options: [
                        { value: "true", name: "Yes" },
                        { value: "false", name: "No" },
                        ],
                        description:
                        "This setting determines if a User's server nickname and roles will be looked up during message allocation.",
                        },
                        {
                            name: DiscrubSetting.DISPLAY_NAME_LOOKUP,
                            label: "Lookup Display Names",
                            options: [
                            { value: "true", name: "Yes" },
                            { value: "false", name: "No" },
                            ],
                            description:
                            "Having this setting set to 'Yes' will ensure that User mentions are correctly displayed, even if the mentioned User is not apart of the conversation. It is recommended to keep this setting enabled.",
                            },
                            {
                                name: DiscrubSetting.RANDOM_DELETE_DELAY,
                                label: "Delete/Edit Random Delay (seconds)",
                                description:
                                "Wait a random amount of seconds (from zero and the provided value), between message deletes/edits.",
                                numeric: true,
                                fallbackValue: "0",
                                icon: () => <AutoDeleteIcon />,
                                },
                                {
                                    name: DiscrubSetting.RANDOM_SEARCH_DELAY,
                                    label: "Search Random Delay (seconds)",
                                    description:
                                    "Wait a random amount of seconds (from zero and the provided value), between message, user, and reaction searches.",
                                    numeric: true,
                                    fallbackValue: "0",
                                    icon: () => <YoutubeSearchedForIcon />,
                                    },

                                    // Export Settings
                                    {
                                        name: DiscrubSetting.EXPORT_MESSAGES_PER_PAGE,
                                        label: "Messages Per Page",
                                        description:
                                        "Consider keeping this value low, export times can be completed many times faster if messages are broken up into pages.",
                                        numeric: true,
                                        fallbackValue: "1000",
                                        icon: () => <FormatListNumberedRtlIcon />,
                                        },
                                        {
                                            name: DiscrubSetting.EXPORT_DOWNLOAD_MEDIA,
                                            label: "Download Media",
                                            multiselect: true,
                                            options: [
                                            { value: MediaType.IMAGES, name: "Images" },
                                            { value: MediaType.VIDEOS, name: "Videos" },
                                            { value: MediaType.AUDIO, name: "Audio" },
                                            ],
                                            description:
                                            "Exports may be performed more slowly when downloading media.",
                                            icon: () => <DownloadIcon />,
                                            },
                                            {
                                                name: DiscrubSetting.EXPORT_PREVIEW_MEDIA,
                                                label: "Preview Media (HTML)",
                                                multiselect: true,
                                                options: [
                                                { value: MediaType.IMAGES, name: "Images" },
                                                { value: MediaType.VIDEOS, name: "Videos" },
                                                { value: MediaType.AUDIO, name: "Audio" },
                                                ],
                                                description:
                                                "Previewing Media on a large number of messages can negatively affect the speed of the export.",
                                                icon: () => <ImageIcon />,
                                                },
                                                {
                                                    name: DiscrubSetting.EXPORT_IMAGE_RES_MODE,
                                                    label: "Image Display Mode (HTML)",
                                                    options: [
                                                    { value: ResolutionType.HOVER_LIMITED, name: "Hover, Safe Resolution" },
                                                    { value: ResolutionType.HOVER_FULL, name: "Hover, Full Resolution" },
                                                    {
                                                        value: ResolutionType.NO_HOVER_LIMITED,
                                                        name: "No Hover, Safe Resolution",
                                                        },
                                                        {
                                                            value: ResolutionType.NO_HOVER_FULL,
                                                            name: "No Hover, Full Resolution",
                                                            },
                                                            ],
                                                            description: getResolutionModeDesc(),
                                                            icon: () => <AspectRatioIcon />,
                                                            },
                                                            {
                                                                name: DiscrubSetting.EXPORT_SEPARATE_THREAD_AND_FORUM_POSTS,
                                                                label: "Separate Thread/Forum Posts",
                                                                options: [
                                                                { value: "true", name: "Yes" },
                                                                { value: "false", name: "No" },
                                                                ],
                                                                description:
                                                                "Separating Threads & Forum Posts will store any existing threads or forum posts into separate files for better readability.",
                                                                icon: () =>
                                                                stringToBool(settings.exportSeparateThreadAndForumPosts) ? (;
                                                                <FolderIcon />;
                                                                ) : (
                                                                <FolderOffIcon />;
                                                                ),
                                                                },
                                                                {
                                                                    name: DiscrubSetting.EXPORT_ARTIST_MODE,
                                                                    label: "Artist Mode",
                                                                    options: [
                                                                    { value: "true", name: "Yes" },
                                                                    { value: "false", name: "No" },
                                                                    ],
                                                                    description:
                                                                    "Artist Mode will store Attached & Embedded Media into folders named by their Author's username",
                                                                    icon: () =>
                                                                    stringToBool(settings.exportUseArtistMode) ? (;
                                                                    <BrushIcon />;
                                                                    ) : (
                                                                    <FormatColorResetIcon />;
                                                                    ),
                                                                    },
                                                                    {
                                                                        name: DiscrubSetting.EXPORT_MESSAGE_SORT_ORDER,
                                                                        label: "Message Sort Direction",
                                                                        options: [
                                                                        { value: SortDirection.ASCENDING, name: "Ascending" },
                                                                        { value: SortDirection.DESCENDING, name: "Descending" },
                                                                        ],
                                                                        "Messages will be sorted by their date, " + std::to_string()
                                                                            getValue(DiscrubSetting.EXPORT_MESSAGE_SORT_ORDER) ==;
                                                                            SortDirection.ASCENDING;
                                                                            ? "older";
                                                                            : "newer"
                                                                            } messages at the top.`,
                                                                            icon: () =>
                                                                            getValue(DiscrubSetting.EXPORT_MESSAGE_SORT_ORDER) ==;
                                                                            SortDirection.ASCENDING ? (;
                                                                            <VerticalAlignTopIcon />;
                                                                            ) : (
                                                                            <VerticalAlignBottomIcon />;
                                                                            ),
                                                                            },
                                                                            ].filter((control) => visibleSettings.some((hs) => hs == control.name));

                                                                            const auto handleNumericOnChange = async (;
                                                                            setting: DiscrubSetting,
                                                                            e: React.ChangeEvent<HTMLInputElement | HTMLTextAreaElement>
                                                                            ) => {
                                                                                const auto { fallbackValue } = controls.find((c) => c.name == setting) || {};
                                                                                const auto input = parseInt(e.target.value);
                                                                                const auto settings = setSetting(;
                                                                                setting,
                                                                                std::to_string(input)
                                                                                );
                                                                                onChangeSettings(settings);
                                                                                };

                                                                                return (;
                                                                                <Stack;
                                                                                sx={{
                                                                                    padding: 3,
                                                                                    spacing: 2,
                                                                                    gap: "15px",
                                                                                    ...containerProps,
                                                                                }}
                                                                                >;
                                                                                {Object.keys(settings).length &&;
                                                                                controls.map((control) => {
                                                                                    const auto Icon = control.icon.();
                                                                                    return (;
                                                                                    <Tooltip placement="left" title={control.description}>;
                                                                                    <FormControl fullWidth>;
                                                                                    {control.numeric && (;
                                                                                    <TextField;
                                                                                label={control.label}
                                                                                variant="filled";
                                                                                size="small";
                                                                            inputProps={{ inputMode: "numeric", pattern: "[0-9]*" }}
                                                                        value={getValue(control.name)}
                                                                    onChange={(e) => handleNumericOnChange(control.name, e)}
                                                                InputProps={{ endAdornment: Icon }}
                                                                />;
                                                            )}
                                                            {control.options.length && !control.multiselect && (;
                                                            <>;
                                                            <InputLabel variant="filled">{control.label}</InputLabel>;
                                                            <Select;
                                                            variant="filled";
                                                        endAdornment={Icon}
                                                    IconComponent={Icon ? "span" : std::nullopt}
                                                value={getValue(control.name)}
                                            label={control.label}
                                            onChange={(e) => {
                                                if (
                                                e.target.value != nullptr &&;
                                                e.target.value != std::nullopt;
                                                );
                                                handleChange(control.name, e.target.value);
                                            }}
                                            >;
                                            {control.options.map((option) => {
                                                return (;
                                                <MenuItem value={option.value}>;
                                            {option.name}
                                            </MenuItem>;
                                            );
                                        })}
                                        </Select>;
                                        </>;
                                    )}
                                    {control.multiselect && (;
                                    <MultiValueSelect;
                                label={control.label}
                                onChange={(values) => {
                                    handleChange(control.name, values.join());
                                }}
                                value={
                                    getValue(control.name);
                                    ? getValue(control.name).split(",") || [];
                                    : []
                                }
                            values={control.options.map((o) => o.value)}
                            displayNameMap={control.options.reduce((acc, curr) => {
                                return { ...acc, [curr.value]: curr.name }
                            }, {})}
                            />;
                        )}
                        </FormControl>;
                        </Tooltip>;
                        );
                    })}
                    </Stack>;
                    );

}

} // namespace elizaos
