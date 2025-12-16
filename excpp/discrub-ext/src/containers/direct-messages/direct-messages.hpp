#include "..classes/channel.hpp"
#include "..classes/message.hpp"
#include "..common-components/tooltip/tooltip.hpp"
#include "..components/attachment-modal.hpp"
#include "..components/cancel-button.hpp"
#include "..components/copy-adornment.hpp"
#include "..components/embed-modal.hpp"
#include "..components/entity-icon.hpp"
#include "..components/pause-button.hpp"
#include "..components/reaction-modal.hpp"
#include "..components/table-message.hpp"
#include "..components/token-not-found.hpp"
#include "..enum/sort-direction.hpp"
#include "..features/app/use-app-slice.hpp"
#include "..features/dm/use-dm-slice.hpp"
#include "..features/message/use-message-slice.hpp"
#include "..features/user/use-user-slice.hpp"
#include "..utils.hpp"
#include ".advanced-filtering/advanced-filtering.hpp"
#include ".export-button/export-button.hpp"
#include ".message-table-toolbar/message-table-toolbar.hpp"
#include ".purge-button/purge-button.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;
;
import Table, {
  OrderProps,
  TableColumn,
  TableRow,
} from "../../common-components/table/table";
;
;
;
;
;
;
;
;
;
;
;
;
;
;
;
;
;
;
;
;
;
;
;
;
;
;
;

 = useUserSlice();
  const token = userState.token();
  const userLoading = userState.isLoading();

  const { state: dmState, getDms, setSelectedDms } = useDmSlice();
  const selectedDms = dmState.selectedDms();
  const dms = dmState.dms();
  const preFilterUserId = dmState.preFilterUserId();

  const {
    state: messageState,
    getMessageData,
    setOrder,
    deleteAttachment,
    setSelected,
    deleteReaction,
    resetAdvancedFilters,
  } = useMessageSlice();
  const messagesLoading = messageState.isLoading();
  const messages = messageState.messages();
  const searchBeforeDate = messageState.searchBeforeDate();
  const searchAfterDate = messageState.searchAfterDate();
  const searchMessageContent = messageState.searchMessageContent();
  const selectedHasTypes = messageState.selectedHasTypes();
  const selectedMessages = messageState.selectedMessages();
  const filters = messageState.filters();
  const filteredMessages = messageState.filteredMessages();

  const { state: appState, setModifyEntity } = useAppSlice();
  const discrubCancelled = appState.discrubCancelled();
  const task = appState.task();
  const settings = appState.settings();

  const [searchTouched, setSearchTouched] = useState(false);
  const [expanded, setExpanded] = useState(true);
  const [attachmentModalOpen, setAttachmentModalOpen] = useState(false);
  const [embedModalOpen, setEmbedModalOpen] = useState(false);
  const [reactionModalOpen, setReactionModalOpen] = useState(false);

  const { statusText } = task || {};

  const columns: TableColumn<Message>[] = [
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

  const rows: TableRow<Message>[] = (
    filters.length ? filteredMessages : messages
  ).map((m) => ({
    data: m,
    selectable: isRemovableMessage(m),
    renderRow: (row) => (
      <TableMessage
        settings={settings}
        row={row}
        setModifyEntity={setModifyEntity}
        openAttachmentModal={() => setAttachmentModalOpen(true)}
        openEmbedModal={() => setEmbedModalOpen(true)}
        openReactionModal={() => setReactionModalOpen(true)}
      />
    ),
  }));

  const orderProps: OrderProps<Message> = {
    order: SortDirection.DESCENDING,
    orderBy: "timestamp",
    onRequestSort: (order, orderBy) => {
      setOrder({ order, orderBy });
    },
  };

  const fetchDmData = async () => {
    getMessageData(null, selectedDms[0].id, { preFilterUserId });
    setSearchTouched(true);
    setExpanded(false);
  };

  const handleChangeDm = async (ids: Snowflake[]) => {
    setSelectedDms(ids);
    setSearchTouched(false);
    if (ids.length > 1) {
      resetAdvancedFilters();
    }
  };

  const sortedDms = dms
    .map((d) => new Channel({ ...d }))
    .sort((a, b) =>
      sortByProperty(
        { name: String(a.name).toLowerCase() },
        { name: String(b.name).toLowerCase() },
        "name"
      )
    );

  const advancedFilterActive = [
    preFilterUserId,
    searchBeforeDate,
    searchAfterDate,
    searchMessageContent,
    selectedHasTypes.length,
  ].some((c) => c);

  const dmFieldDisabled = messagesLoading || discrubCancelled;
  const searchDisabled =
    selectedDms.length !== 1 || messagesLoading || discrubCancelled;
  const pauseCancelDisabled = !messagesLoading;
  const exportAndPurgeDisabled =
    selectedDms.length === 0 ||
    messagesLoading ||
    messages.length > 0 ||
    advancedFilterActive ||
    discrubCancelled;

  useEffect(() => {
    if (token) getDms();
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [token]);

  return (
    <Stack
      spacing={2}
      sx={{
        padding: "15px",
        maxHeight: "85%",
        maxWidth: "100%",
        overflow: "hidden",
      }}
    >
      <ReactionModal
        task={task}
        handleClose={() => setReactionModalOpen(false)}
        open={reactionModalOpen}
        handleReactionDelete={deleteReaction}
      />
      <AttachmentModal
        task={task}
        onDeleteAttachment={deleteAttachment}
        handleClose={() => setAttachmentModalOpen(false)}
        open={attachmentModalOpen}
      />
      <EmbedModal
        task={task}
        handleClose={() => setEmbedModalOpen(false)}
        open={embedModalOpen}
      />
      {token && dms && (
        <Stack spacing={2}>
          <Paper sx={{ padding: "10px" }}>
            <Stack spacing={2}>
              <Stack
                direction="row"
                justifyContent="space-between"
                alignItems="center"
              >
                <Typography variant="body1">Direct Messages</Typography>
                <Tooltip title={expanded ? "Collapse" : "Expand"}>
                  <IconButton
                    onClick={() => {
                      setExpanded(!expanded);
                    }}
                    color="secondary"
                  >
                    {expanded ? <RemoveIcon /> : <AddIcon />}
                  </IconButton>
                </Tooltip>
              </Stack>

              <Collapse orientation="vertical" in={expanded}>
                <Stack direction="column" gap="5px" spacing={1}>
                  <Stack
                    direction="row"
                    justifyContent="center"
                    alignItems="center"
                    spacing={1}
                  >
                    <Autocomplete
                      multiple
                      clearIcon={<ClearIcon />}
                      onChange={(_, val) => handleChangeDm(val)}
                      options={sortedDms.map((directMessage) => {
                        return directMessage.id;
                      })}
                      getOptionLabel={(id) =>
                        String(dms.find((dm) => dm.id === id)?.name)
                      }
                      renderOption={(params, id) => {
                        const foundDm = dms.find((dm) => dm.id === id);
                        return (
                          <Typography gap="4px" {...params}>
                            {foundDm && <EntityIcon entity={foundDm} />}
                            {foundDm?.name}
                          </Typography>
                        );
                      }}
                      renderTags={(value: readonly string[], getTagProps) =>
                        value.map((option: string, index: number) => {
                          const { key, ...tagProps } = getTagProps({ index });
                          const foundDm = dms.find((dm) => dm.id === option);
                          return (
                            <Chip
                              variant="outlined"
                              label={String(
                                dms.find((dm) => dm.id === option)?.name
                              )}
                              key={key}
                              {...tagProps}
                              avatar={
                                foundDm ? (
                                  <EntityIcon entity={foundDm} />
                                ) : undefined
                              }
                            />
                          );
                        })
                      }
                      renderInput={(params) => (
                        <TextField
                          {...params}
                          variant="filled"
                          fullWidth
                          size="small"
                          label="DM"
                          sx={{ width: "670px !important" }}
                          InputProps={{
                            ...params.InputProps,
                            startAdornment: (
                              <>
                                <CopyAdornment
                                  copyValue={sortedDms
                                    .map((dm) => dm.name)
                                    .join("\r\n")}
                                  copyName="DM List"
                                  disabled={dmFieldDisabled}
                                />
                                {params.InputProps.startAdornment}
                              </>
                            ),
                          }}
                        />
                      )}
                      value={selectedDms.map((dm) => dm.id)}
                      disabled={dmFieldDisabled}
                    />
                  </Stack>

                  <AdvancedFiltering isDm />
                </Stack>
              </Collapse>
              <Stack
                alignItems="center"
                direction="row"
                spacing={1}
                justifyContent="flex-end"
              >
                <ExportButton bulk disabled={exportAndPurgeDisabled} isDm />
                <PurgeButton disabled={exportAndPurgeDisabled} isDm />
                <PauseButton disabled={pauseCancelDisabled} />
                <Button
                  disabled={searchDisabled}
                  onClick={fetchDmData}
                  variant="contained"
                >
                  Search
                </Button>

                <CancelButton disabled={pauseCancelDisabled} />
              </Stack>
            </Stack>
          </Paper>
        </Stack>
      )}

      <>
        {!messagesLoading && (
          <>
            {messages.length > 0 && (
              <Box sx={{ maxHeight: "430px", overflow: "auto" }}>
                <Table
                  columns={columns}
                  rows={rows}
                  orderProps={orderProps}
                  selectedRows={selectedMessages}
                  renderToolbarComponent={(selectedRows) => (
                    <MessageTableToolbar selectedRows={selectedRows} />
                  )}
                  setSelectedRows={setSelected}
                />
              </Box>
            )}
            {messages.length === 0 && !!selectedDms.length && searchTouched && (
              <Paper sx={{ padding: "10px" }}>
                <Box
                  sx={{
                    alignItems: "center",
                    justifyContent: "center",
                    display: "flex",
                    marginTop: "1vh",
                    flexDirection: "column",
                  }}
                >
                  <SentimentDissatisfiedIcon />
                  <Typography>No Messages to Display</Typography>
                </Box>
              </Paper>
            )}
          </>
        )}

        {(userLoading || messagesLoading) && (
          <Paper sx={{ justifyContent: "center", padding: "10px" }}>
            <Box
              sx={{
                alignItems: "center",
                justifyContent: "center",
                display: "flex",
                marginTop: "1vh",
                flexDirection: "column",
              }}
            >
              <LinearProgress sx={{ width: "100%", m: 1 }} />
              <Typography variant="caption">{statusText}</Typography>
            </Box>
          </Paper>
        )}
        {!token && !userLoading && <TokenNotFound />}
      </>
    </Stack>
  );
}

default DirectMessages;

} // namespace elizaos
