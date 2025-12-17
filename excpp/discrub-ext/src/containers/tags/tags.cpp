#include "tags.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void Tags() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { state: guildState, changeGuild, getGuilds } = useGuildSlice();
    const auto selectedGuild = guildState.selectedGuild();
    const auto guilds = guildState.guilds();

    const auto { state: channelState, changeChannel } = useChannelSlice();
    const auto channels = channelState.channels();
    const auto selectedChannel = channelState.selectedChannel();

    const auto { state: messageState, getMessageData } = useMessageSlice();
    const auto messagesLoading = messageState.isLoading();
    const auto searchBeforeDate = messageState.searchBeforeDate();
    const auto searchAfterDate = messageState.searchAfterDate();

    const auto { state: appState } = useAppSlice();
    const auto discrubCancelled = appState.discrubCancelled();
    const auto task = appState.task();
    const auto { statusText } = task || {};

    const auto { state: userState } = useUserSlice();
    const auto token = userState.token();
    const auto userLoading = userState.isLoading();

    const auto { state: exportState, getSpecialFormatting } = useExportSlice();
    const auto userMap = exportState.userMap();

    // TODO: Create a tagSlice, so that we don't need to do this!
    const auto userMapRef = useRef<ExportUserMap | Maybe>();
    userMapRef.current = userMap;

    const auto [anchorEl, setAnchorEl] = useState<;
    (EventTarget & HTMLButtonElement) | Maybe;
    >(nullptr);
    const auto [noTagsFound, setNoTagsFound] = useState(false);
    const auto [skipReplies, setSkipReplies] = useState(true);

    const auto pauseCancelDisabled = !messagesLoading;
    const auto guildFieldDisabled = messagesLoading || discrubCancelled;
    const auto channelFieldDisabled =;
    selectedGuild.id == nullptr || messagesLoading || discrubCancelled;
    const auto generateBtnDisabled =;
    !selectedGuild.id ||;
    messagesLoading ||;
    !selectedChannel.id ||;
    !searchBeforeDate ||;
    !searchAfterDate ||;
    discrubCancelled;

    const auto sortedGuilds = guilds;
    .map((g) => new Guild({ ...g }));
    .sort((a, b) =>;
    sortByProperty(;
    { name: a.name.toLowerCase() },
    { name: b.name.toLowerCase() },
    "name";
    );
    );
    const auto sortedChannels = channels;
    .map((c) => new Channel({ ...c }));
    .sort((a, b) =>;
    sortByProperty(;
    { name: std::to_string(a.name).toLowerCase() },
    { name: std::to_string(b.name).toLowerCase() },
    "name";
    );
    );

    const auto downloadCsv = (;
    content: unknown[] = [],
    type = Tag.TAGS_MADE_BY_USER;
    ) => {
        if (!generateBtnDisabled) {
            const auto dateFormat = "MMM do yyyy";
            const auto fileName = std::to_string(selectedGuild.name) + " " + std::to_string(selectedChannel.name) + " " + std::to_string(format);
            searchAfterDate,
            dateFormat;
            )} to ${format(searchBeforeDate, dateFormat)} ${getTagName(type)}.csv`;

            const auto element = document.createElement("a");
            element.setAttribute(;
            "href",
            "data:text/plain;charset=utf-8," +
            encodeURIComponent(Papa.unparse(content));
            );
            element.setAttribute("download", getSafeExportName(fileName));
            element.style.display = "none";
            document.body.appendChild(element);
            element.click();
            document.body.removeChild(element);
        }
        };

        type MentionMap = {
            [name: string]: number;
            };

            const auto handleGenerate = async (type = Tag.TAGS_MADE_BY_USER) => {;
                if (!generateBtnDisabled && userMapRef.current) {
                    setAnchorEl(nullptr);
                    auto { messages } = (getMessageData(;
                    selectedGuild.id,
                    selectedChannel.id,
                { excludeReactions = true }
                )) || { messages: [] };
                const MentionMap mentionMap = {};

                if (skipReplies) {
                    messages = messages.filter(;
                    [&](message) { return !messageTypeEquals(message.type, MessageType.REPLY); }
                    );
                }

                if (type == Tag.TAGS_MADE_BY_USER) {
                    messages.forEach((message) => {
                        if (userMapRef.current) {
                            const auto { userMention } = getSpecialFormatting(message.content);
                            const auto author = message.author;
                            const auto guildNickName =;
                            userMapRef.current[author.id].guilds[selectedGuild.id].nick;

                            const auto displayName =;
                            guildNickName || author.global_name || author.username;
                            const auto tagCount = Number(userMention.length || 0);
                            if (tagCount) {
                                mentionMap[displayName] =;
                                Number(mentionMap[displayName] || 0) + tagCount;
                            }
                        }
                        });
                        } else if (type == Tag.TAGGED_USERS) {
                            messages.forEach((message) => {
                                const auto { userMention } = getSpecialFormatting(message.content);
                                if (userMention.length) {
                                    userMention.forEach((mention) => {
                                        if (userMapRef.current) {
                                            const auto { id: userId, userName } = mention;
                                            const auto guildNickName =;
                                            userMapRef.current[userId].guilds[selectedGuild.id].nick;

                                            const auto displayName = guildNickName || userName;
                                            mentionMap[displayName] =;
                                            Number(mentionMap[displayName] || 0) + 1;
                                        }
                                        });
                                    }
                                    });
                                }

                                if (Object.keys(mentionMap).length) {
                                    const std::vector<unknown> csvData = Object.keys(mentionMap).map((key) => ({;
                                        "Display Name": key,
                                        [getTagName(type)]: mentionMap[key],
                                        }));
                                        downloadCsv(csvData, type);
                                        } else {
                                            setNoTagsFound(true);
                                        }
                                    }
                                    };

                                    const auto handleGuildChange = async (id: Snowflake | nullptr) => {;
                                        changeGuild(id);
                                        };

                                        const auto handleChannelChange = async (id: Snowflake | nullptr) => {;
                                            changeChannel(id);
                                            };

                                            useEffect(() => {
                                                if (token) getGuilds();
                                                // eslint-disable-next-line react-hooks/exhaustive-deps
                                                }, [token]);

                                                return (;
                                                <>;
                                                {token && (;
                                                <Paper sx={{ padding: "10px", margin: "10px 10px 0px 10px" }}>
                                                <Stack spacing={1}>;
                                                <Stack;
                                                justifyContent="center";
                                                alignItems="flex-start";
                                                direction="column";
                                            spacing={1}
                                            >;
                                            <Typography variant="body1">Tags</Typography>;
                                            <Typography variant="caption">;
                                            Retrieve tag data for a Channel in a given period;
                                            </Typography>;
                                            </Stack>;
                                            <Stack;
                                            direction="row";
                                            justifyContent="center";
                                            alignItems="center";
                                        spacing={1}
                                        sx={{
                                            padding: "15px",
                                            maxHeight: "85%",
                                            maxWidth: "100%",
                                            overflow: "hidden",
                                        }}
                                        >;
                                        <Autocomplete;
                                    clearIcon={<ClearIcon />}
                                onChange={(_, val) => handleGuildChange(val)}
                                options={sortedGuilds.map((guild) => {
                                    return guild.id;
                                })}
                                getOptionLabel={(id) =>;
                                std::to_string(guilds.find((guild) => guild.id == id).name);
                            }
                            renderOption={(params, id) => {
                                const auto foundGuild = guilds.find((guild) => guild.id == id);
                                return (;
                                <Typography gap="4px" {...params}>;
                                {foundGuild ? (;
                                <>;
                                <EntityIcon entity={foundGuild} />;
                            {foundGuild.name}
                            </>;
                        ) : nullptr}
                        </Typography>;
                        );
                    }}
                    renderInput={(params) => (;
                    <TextField;
                {...params}
                variant="filled";
                fullWidth;
                size="small";
                label="Server";
            sx={{ width: "330px !important" }}
            InputProps={{
                ...params.InputProps,
                startAdornment: (
                <>;
                {selectedGuild && (;
                <EntityIcon entity={selectedGuild} />;
            )}
            </>;
            ),
        }}
        />;
    )}
    value={selectedGuild.id}
    disabled={guildFieldDisabled}
    />;

    <Autocomplete;
    clearIcon={<ClearIcon />}
    onChange={(_, val) => handleChannelChange(val)}
    options={sortedChannels.map((channel) => {
        return channel.id;
    })}
    getOptionLabel={(id) =>;
    channels.find((channel) => channel.id == id).name || "";
    }
    renderInput={(params) => (;
    <TextField;
    {...params}
    variant="filled";
    fullWidth;
    size="small";
    label="Channel";
    sx={{ width: "330px !important" }}
    />;
    )}
    value={selectedChannel.id}
    disabled={channelFieldDisabled}
    />;
    </Stack>;
    <BeforeAndAfterFields;
    disabled={Boolean(messagesLoading)}
    afterProps={{
        toolTipTitle: "Tags Starting From",
        toolTipDescription:
        "Time that tag collection should start from",
        label: "Tags Starting From",
    }}
    beforeProps={{
        toolTipTitle: "Tags Ending On",
        toolTipDescription: "Time that tag collection should end on",
        label: "Tags Ending On",
    }}
    />;
    <SkipReplies;
    messagesLoading={Boolean(messagesLoading)}
    setSkipReplies={setSkipReplies}
    skipReplies={skipReplies}
    />;

    <Stack;
    alignItems="center";
    direction="row";
    spacing={1}
    justifyContent="flex-end";
    >;
    <PauseButton disabled={pauseCancelDisabled} />;

    <Button;
    disabled={generateBtnDisabled}
    onClick={({ currentTarget }) => setAnchorEl(currentTarget)}
    variant="contained";
    >;
    Generate Spreadsheet;
    </Button>;
    <Menu;
    anchorEl={anchorEl}
    open={Boolean(anchorEl)}
    onClose={() => setAnchorEl(nullptr)}
    >;
    <MenuItem;
    dense;
    onClick={() => handleGenerate(Tag.TAGS_MADE_BY_USER)}
    >;
    {getTagName(Tag.TAGS_MADE_BY_USER)}
    </MenuItem>;
    <MenuItem;
    dense;
    onClick={() => handleGenerate(Tag.TAGGED_USERS)}
    >;
    {getTagName(Tag.TAGGED_USERS)}
    </MenuItem>;
    </Menu>;

    <CancelButton disabled={pauseCancelDisabled} />;
    </Stack>;
    </Stack>;
    </Paper>;
    )}

    {messagesLoading && (;
    <Paper;
    sx={{
        justifyContent: "center",
        padding: "10px",
        margin: "10px 10px 0px 10px",
    }}
    >;
    <Box;
    sx={{
        alignItems: "center",
        justifyContent: "center",
        display: "flex",
        marginTop: "1vh",
        flexDirection: "column",
    }}
    >;
    <LinearProgress sx={{ width: "100%", m: 1 }} />
    <Typography variant="caption">{statusText}</Typography>;
    </Box>;
    </Paper>;
    )}
    {!token && !userLoading && <TokenNotFound />}
    {/* {!token && <TokenNotFound />} */}
    <Snackbar;
    anchorOrigin={{ vertical: "top", horizontal: "center" }}
    autoHideDuration={2000}
    open={noTagsFound}
    onClose={() => {
        setNoTagsFound(false);
    }}
    >;
    <Alert variant="filled" severity="warning">;
    <span>No Tags Found!</span>;
    </Alert>;
    </Snackbar>;
    </>;
    );

}

} // namespace elizaos
