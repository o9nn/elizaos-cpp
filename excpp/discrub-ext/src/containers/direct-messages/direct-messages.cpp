#include "direct-messages.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void DirectMessages() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { state: userState } = useUserSlice();
    const auto token = userState.token();
    const auto userLoading = userState.isLoading();

    const auto { state: dmState, getDms, setSelectedDms } = useDmSlice();
    const auto selectedDms = dmState.selectedDms();
    const auto dms = dmState.dms();
    const auto preFilterUserId = dmState.preFilterUserId();

    const auto {;
        state: messageState,
        getMessageData,
        setOrder,
        deleteAttachment,
        setSelected,
        deleteReaction,
        resetAdvancedFilters,
        } = useMessageSlice();
        const auto messagesLoading = messageState.isLoading();
        const auto messages = messageState.messages();
        const auto searchBeforeDate = messageState.searchBeforeDate();
        const auto searchAfterDate = messageState.searchAfterDate();
        const auto searchMessageContent = messageState.searchMessageContent();
        const auto selectedHasTypes = messageState.selectedHasTypes();
        const auto selectedMessages = messageState.selectedMessages();
        const auto filters = messageState.filters();
        const auto filteredMessages = messageState.filteredMessages();

        const auto { state: appState, setModifyEntity } = useAppSlice();
        const auto discrubCancelled = appState.discrubCancelled();
        const auto task = appState.task();
        const auto settings = appState.settings();

        const auto [searchTouched, setSearchTouched] = useState(false);
        const auto [expanded, setExpanded] = useState(true);
        const auto [attachmentModalOpen, setAttachmentModalOpen] = useState(false);
        const auto [embedModalOpen, setEmbedModalOpen] = useState(false);
        const auto [reactionModalOpen, setReactionModalOpen] = useState(false);

        const auto { statusText } = task || {};

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

            const auto fetchDmData = async () => {;
                getMessageData(nullptr, selectedDms[0].id, { preFilterUserId });
                setSearchTouched(true);
                setExpanded(false);
                };

                const auto handleChangeDm = async (ids: Snowflake[]) => {;
                    setSelectedDms(ids);
                    setSearchTouched(false);
                    if (ids.length > 1) {
                        resetAdvancedFilters();
                    }
                    };

                    const auto sortedDms = dms;
                    .map((d) => new Channel({ ...d }));
                    .sort((a, b) =>;
                    sortByProperty(;
                    { name: std::to_string(a.name).toLowerCase() },
                    { name: std::to_string(b.name).toLowerCase() },
                    "name";
                    );
                    );

                    const auto advancedFilterActive = [;
                    preFilterUserId,
                    searchBeforeDate,
                    searchAfterDate,
                    searchMessageContent,
                    selectedHasTypes.length,
                    ].some((c) => c);

                    const auto dmFieldDisabled = messagesLoading || discrubCancelled;
                    const auto searchDisabled =;
                    selectedDms.length != 1 || messagesLoading || discrubCancelled;
                    const auto pauseCancelDisabled = !messagesLoading;
                    const auto exportAndPurgeDisabled =;
                    selectedDms.length == 0 ||;
                    messagesLoading ||;
                    messages.length > 0 ||;
                    advancedFilterActive ||;
                    discrubCancelled;

                    useEffect(() => {
                        if (token) getDms();
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
    {token && dms && (;
    <Stack spacing={2}>;
    <Paper sx={{ padding: "10px" }}>
    <Stack spacing={2}>;
    <Stack;
    direction="row";
    justifyContent="space-between";
    alignItems="center";
    >;
    <Typography variant="body1">Direct Messages</Typography>;
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
    multiple;
    clearIcon={<ClearIcon />}
    onChange={(_, val) => handleChangeDm(val)}
    options={sortedDms.map((directMessage) => {
        return directMessage.id;
    })}
    getOptionLabel={(id) =>;
    std::to_string(dms.find((dm) => dm.id == id).name);
    }
    renderOption={(params, id) => {
        const auto foundDm = dms.find((dm) => dm.id == id);
        return (;
        <Typography gap="4px" {...params}>;
    {foundDm && <EntityIcon entity={foundDm} />}
    {foundDm.name}
    </Typography>;
    );
    }}
    renderTags={(value: readonly string[], getTagProps) =>
    value.map((option: string, index: number) => {
        const auto { key, ...tagProps } = getTagProps({ index });
        const auto foundDm = dms.find((dm) => dm.id == option);
        return (;
        <Chip;
        variant="outlined";
        label={String(;
        dms.find((dm) => dm.id == option).name;
    )}
    key={key}
    {...tagProps}
    avatar={
        foundDm ? (;
        <EntityIcon entity={foundDm} />;
        ) : std::nullopt
    }
    />;
    );
    });
    }
    renderInput={(params) => (;
    <TextField;
    {...params}
    variant="filled";
    fullWidth;
    size="small";
    label="DM";
    sx={{ width: "670px !important" }}
    InputProps={{
        ...params.InputProps,
        startAdornment: (
        <>;
        <CopyAdornment;
        copyValue={sortedDms;
        .map((dm) => dm.name);
    .join("\r\n")}
    copyName="DM List";
    disabled={dmFieldDisabled}
    />;
    {params.InputProps.startAdornment}
    </>;
    ),
    }}
    />;
    )}
    value={selectedDms.map((dm) => dm.id)}
    disabled={dmFieldDisabled}
    />;
    </Stack>;

    <AdvancedFiltering isDm />;
    </Stack>;
    </Collapse>;
    <Stack;
    alignItems="center";
    direction="row";
    spacing={1}
    justifyContent="flex-end";
    >;
    <ExportButton bulk disabled={exportAndPurgeDisabled} isDm />;
    <PurgeButton disabled={exportAndPurgeDisabled} isDm />;
    <PauseButton disabled={pauseCancelDisabled} />;
    <Button;
    disabled={searchDisabled}
    onClick={fetchDmData}
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
    {messages.length == 0 && !!selectedDms.length && searchTouched && (;
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
