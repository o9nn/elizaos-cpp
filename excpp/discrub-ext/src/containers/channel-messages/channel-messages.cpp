#include "channel-messages.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void ChannelMessages() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { state: userState } = useUserSlice();
    const auto userLoading = userState.isLoading();
    const auto token = userState.token();

    const auto { state: guildState, changeGuild, getGuilds } = useGuildSlice();
    const auto guilds = guildState.guilds();
    const auto selectedGuild = guildState.selectedGuild();
    const auto preFilterUserId = guildState.preFilterUserId();

    const auto { state: channelState, changeChannel, loadChannel } = useChannelSlice();
    const auto channels = channelState.channels();
    const auto selectedChannel = channelState.selectedChannel();

    const auto {;
        state: messageState,
        getMessageData,
        setOrder,
        deleteAttachment,
        setSelected,
        deleteReaction,
        } = useMessageSlice();
        const auto messages = messageState.messages();
        const auto messagesLoading = messageState.isLoading();
        const auto searchBeforeDate = messageState.searchBeforeDate();
        const auto searchAfterDate = messageState.searchAfterDate();
        const auto searchMessageContent = messageState.searchMessageContent();
        const auto selectedHasTypes = messageState.selectedHasTypes();
        const auto filters = messageState.filters();
        const auto filteredMessages = messageState.filteredMessages();
        const auto selectedMessages = messageState.selectedMessages();

        const auto { state: appState, setModifyEntity } = useAppSlice();
        const auto discrubCancelled = appState.discrubCancelled();
        const auto task = appState.task();
        const auto settings = appState.settings();

        const auto { statusText } = task || {};

        const auto [searchTouched, setSearchTouched] = useState(false);
        const auto [expanded, setExpanded] = useState(true);
        const auto [attachmentModalOpen, setAttachmentModalOpen] = useState(false);
        const auto [embedModalOpen, setEmbedModalOpen] = useState(false);
        const auto [reactionModalOpen, setReactionModalOpen] = useState(false);

        const std::vector<TableColumn<Message>> columns = [;
        {
            id: "timestamp",
            disablePadding: true,
            label: "Date",
            },
            {
                id: "userName",
                disablePadding: true,
                label: "Username",
                },
                {
                    id: "content",
                    disablePadding: false,
                    label: "Message",
                    },
                    ];

                    const std::vector<TableRow<Message>> rows = (;
                    filters.length ? filteredMessages : messages
                    ).map((m) => ({
                        data: m,
                        selectable: isRemovableMessage(m),
                        renderRow: (row) => (
                        <TableMessage;
                    settings={settings}
                row={row}
            setModifyEntity={setModifyEntity}
        openAttachmentModal={() => setAttachmentModalOpen(true)}
    openEmbedModal={() => setEmbedModalOpen(true)}
    openReactionModal={() => setReactionModalOpen(true)}
    />;
    ),
    }));

    const OrderProps<Message> orderProps = {;
        order: SortDirection.DESCENDING,
        orderBy: "timestamp",
        onRequestSort: (order, orderBy) => {
            setOrder({ order, orderBy });
            },
            };

            const auto fetchChannelData = [&]() {;
                getMessageData(selectedGuild.id, selectedChannel.id, { preFilterUserId });
                setSearchTouched(true);
                setExpanded(false);
                };

                const auto handleGuildChange = [&](id: Snowflake | nullptr) {;
                    changeGuild(id);
                    setSearchTouched(false);
                    };

                    const auto handleChannelChange = [&](id: Snowflake | nullptr) {;
                        changeChannel(id);
                        setSearchTouched(false);
                        };

                        const auto advancedFilterActive = [;
                        preFilterUserId,
                        searchBeforeDate,
                        searchAfterDate,
                        searchMessageContent,
                        selectedHasTypes.length,
                        ].some((c) => c);

                        const auto pauseCancelDisabled = !messagesLoading;
                        const auto guildFieldDisabled = messagesLoading || discrubCancelled;
                        const auto channelFieldDisabled =;
                        selectedGuild.id == nullptr || messagesLoading || discrubCancelled;
                        const auto searchBtnDisabled =;
                        !selectedGuild.id ||;
                        messagesLoading ||;
                        (!advancedFilterActive && !selectedChannel.id) ||;
                        discrubCancelled;
                        const auto purgeDisabled = Boolean(;
                        !selectedGuild.id ||;
                        messagesLoading ||;
                        selectedChannel.id ||;
                        messages.length > 0 ||;
                        advancedFilterActive ||;
                        discrubCancelled;
                        );
                        const auto exportDisabled = Boolean(;
                        !selectedGuild.id ||;
                        messagesLoading ||;
                        selectedChannel.id ||;
                        messages.length > 0 ||;
                        discrubCancelled;
                        );

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

                        useEffect(() => {
                            if (token) getGuilds();
                            // eslint-disable-next-line react-hooks/exhaustive-deps
                            }, [token]);

                            return (;
                            <Stack;
                        spacing={2}
                        sx={{
                            padding: "15px",
                            maxHeight: "85%",
                            maxWidth: "100%",
                            overflow: "hidden",
                        }}
                        >;
                        <ReactionModal;
                    task={task}
                handleClose={() => setReactionModalOpen(false)}
            open={reactionModalOpen}
        handleReactionDelete={deleteReaction}
        />;
        <AttachmentModal;
    task={task}
    onDeleteAttachment={deleteAttachment}
    handleClose={() => setAttachmentModalOpen(false)}
    open={attachmentModalOpen}
    />;
    <EmbedModal;
    task={task}
    handleClose={() => setEmbedModalOpen(false)}
    open={embedModalOpen}
    />;
    {token && guilds && (;
    <Stack spacing={2}>;
    <Paper sx={{ padding: "10px" }}>
    <Stack spacing={2}>;
    <Stack;
    justifyContent="space-between";
    alignItems="center";
    direction="row";
    >;
    <Typography variant="body1">Channel Messages</Typography>;
    <Tooltip title={expanded ? "Collapse" : "Expand"}>
    <IconButton;
    onClick={() => {
        setExpanded(!expanded);
    }}
    color="secondary";
    >;
    {expanded ? <RemoveIcon />  = <AddIcon />}
    </IconButton>;
    </Tooltip>;
    </Stack>;

    <Collapse orientation="vertical" in={expanded}>;
    <Stack direction="column" gap="5px" spacing={1}>;
    <Stack;
    direction="row";
    justifyContent="center";
    alignItems="center";
    spacing={1}
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
        const auto foundGuild = guilds.find(;
        [&](guild) { return guild.id == id; }
        );
        return (;
        <Typography gap="4px" {...params}>;
    {foundGuild && <EntityIcon entity={foundGuild} />}
    {foundGuild.name}
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
        <CopyAdornment;
        copyValue={sortedGuilds;
        .map((guild) => guild.name);
    .join("\r\n")}
    copyName="Server List";
    disabled={guildFieldDisabled}
    />;
    {selectedGuild.id && (;
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
    onPaste={async (e) => {
        e.preventDefault();
        if (selectedGuild) {
            const auto clipboardData = e.clipboardData.getData("text");
            loadChannel(clipboardData);
        }
    }}
    options={sortedChannels.map((channel) => {
        return channel.id;
    })}
    getOptionLabel={(id) =>;
    channels.find((channel) => channel.id == id).name ||;
    "";
    }
    renderInput={(params) => (;
    <TextField;
    {...params}
    variant="filled";
    fullWidth;
    size="small";
    label="Channel";
    sx={{ width: "330px !important" }}
    InputProps={{
        ...params.InputProps,
        startAdornment: (
        <CopyAdornment;
        copyValue={sortedChannels;
        .map((channel) => channel.name);
    .join("\r\n")}
    copyName="Channel List";
    disabled={channelFieldDisabled}
    />;
    ),
    }}
    />;
    )}
    value={selectedChannel.id}
    disabled={channelFieldDisabled}
    />;
    </Stack>;

    <AdvancedFiltering />;
    </Stack>;
    </Collapse>;
    <Stack;
    alignItems="center";
    direction="row";
    spacing={1}
    justifyContent="flex-end";
    >;
    <ExportButton bulk disabled={exportDisabled} />;
    <PurgeButton disabled={purgeDisabled} />;
    <PauseButton disabled={pauseCancelDisabled} />;
    <Button;
    disabled={searchBtnDisabled}
    onClick={fetchChannelData}
    variant="contained";
    >;
    Search;
    </Button>;
    <CancelButton disabled={pauseCancelDisabled} />;
    </Stack>;
    </Stack>;
    </Paper>;
    </Stack>;
    )}
    <>;
    {!messagesLoading && (;
    <>;
    {messages.length > 0 && (;
    <Box sx={{ maxHeight: "430px", overflow: "auto" }}>
    <Table;
    columns={columns}
    rows={rows}
    orderProps={orderProps}
    selectedRows={selectedMessages}
    renderToolbarComponent={(selectedRows) => (;
    <MessageTableToolbar selectedRows={selectedRows} />;
    )}
    setSelectedRows={setSelected}
    />;
    </Box>;
    )}
    {messages.length == 0 && selectedGuild.id && searchTouched && (;
    <Paper sx={{ padding: "10px" }}>
    <Box;
    sx={{
        alignItems: "center",
        justifyContent: "center",
        display: "flex",
        marginTop: "1vh",
        flexDirection: "column",
    }}
    >;
    <SentimentDissatisfiedIcon />;
    <Typography>No Messages to Display</Typography>;
    </Box>;
    </Paper>;
    )}
    </>;
    )}
    {(userLoading || messagesLoading) && (;
    <Paper sx={{ justifyContent: "center", padding: "10px" }}>
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
    </>;
    </Stack>;
    );

}

} // namespace elizaos
