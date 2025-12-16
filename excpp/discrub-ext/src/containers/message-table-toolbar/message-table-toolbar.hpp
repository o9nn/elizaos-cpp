#include "..common-components/tooltip/tooltip.hpp"
#include "..features/app/use-app-slice.hpp"
#include "..features/dm/use-dm-slice.hpp"
#include "..features/message/message-types.hpp"
#include "..features/message/use-message-slice.hpp"
#include "..features/thread/use-thread-slice.hpp"
#include ".export-button/export-button.hpp"
#include "components/delete-modal.hpp"
#include "components/edit-modal.hpp"
#include "components/filter-modal.hpp"
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

using MessageTableToolbarProps = std::vector<{
  selectedRows: string>;
};

const MessageTableToolbar = ({ selectedRows }: MessageTableToolbarProps) => {
  const { state: dmState } = useDmSlice();
  const selectedDms = dmState.selectedDms();
  const isDm = !!selectedDms.length;

  const {
    state: appState,
    setDiscrubCancelled,
    setDiscrubPaused,
  } = useAppSlice();
  const discrubCancelled = appState.discrubCancelled();
  const task = appState.task();

  const {
    state: messageState,
    resetFilters,
    deleteMessages,
    editMessages,
    updateFilters,
    filterMessages,
  } = useMessageSlice();
  const messages = messageState.messages();
  const filters = messageState.filters();

  const { state: threadState } = useThreadSlice();
  const threads = threadState.threads();

  const [filterOpen, setFilterOpen] = useState(false);
  const [deleteModalOpen, setDeleteModalOpen] = useState(false);
  const [editModalOpen, setEditModalOpen] = useState(false);

  const handleFilterToggle = () => {
    setFilterOpen(!filterOpen);
  };

  const handleDeleteModalClose = () => {
    if (task.active) {
      // We are actively deleting, we need to send a cancel request
      setDiscrubCancelled(true);
    }

    setDiscrubPaused(false);
    setDeleteModalOpen(false);
  };

  const handleDeleteMessage = (deleteConfig: DeleteConfiguration) => {
    const selections = messages.filter((x) => selectedRows.includes(x.id));
    deleteMessages(selections, deleteConfig);
  };

  const handleEditMessage = (updateText: string) => {
    const toEdit = messages.filter((m) =>
      selectedRows.some((smId) => smId === m.id)
    );
    editMessages(toEdit, updateText);
  };

  const handleEditModalClose = () => {
    if (task.active) {
      // We are actively editing, we need to send a cancel request
      setDiscrubCancelled(true);
    }
    setDiscrubPaused(false);
    setEditModalOpen(false);
  };

  const handleFilterMessages = debounce(() => {
    filterMessages();
  }, 600);

  const handleFilterUpdate = (filter: Filter) => {
    updateFilters(filter);
    handleFilterMessages();
  };

  return (
    <Toolbar
      sx={{
        pl: { sm: 2 },
        pr: { xs: 1, sm: 1 },
        ...(selectedRows.length > 0 && {
          bgcolor: (theme) =>
            alpha(
              theme.palette.primary.main,
              theme.palette.action.activatedOpacity
            ),
        }),
      }}
    >
      <DeleteModal
        selectedRows={selectedRows}
        open={deleteModalOpen}
        handleClose={handleDeleteModalClose}
        task={task}
        handleDeleteMessage={handleDeleteMessage}
      />
      <EditModal
        task={task}
        handleClose={handleEditModalClose}
        handleEditMessage={handleEditMessage}
        open={editModalOpen}
      />
      <Stack sx={{ width: "100%" }} direction="column">
        <Stack
          sx={{ width: "100%" }}
          alignItems="baseline"
          direction="column"
          spacing={2}
          mb="10px"
        >
          <Stack
            sx={{ width: "100%" }}
            mt="20px !important"
            direction="row"
            justifyContent="space-between"
            zIndex={2} // This ensures that the Export options show over FilterComponent
          >
            <Button
              variant="contained"
              color={filters.length ? "primary" : "secondary"}
              startIcon={
                filters.length ? <FilterListIcon /> : <FilterListOffIcon />
              }
              onClick={handleFilterToggle}
            >
              {`Quick Filtering${filters.length ? " (Active)" : ""}`}
            </Button>
            <ExportButton
              disabled={discrubCancelled}
              bulk={false}
              isDm={isDm}
            />
          </Stack>
          <FilterModal
            open={filterOpen}
            handleModalToggle={handleFilterToggle}
            isDm={isDm}
            handleFilterUpdate={handleFilterUpdate}
            threads={threads}
            handleResetFilters={resetFilters}
            filters={filters}
          />
        </Stack>

        <Stack
          direction="row"
          justifyContent="space-between"
          alignItems="center"
          sx={{ opacity: selectedRows.length > 0 ? 1 : 0, minHeight: "40px" }}
        >
          <Typography variant="subtitle1" component="div">
            {selectedRows.length} selected
          </Typography>
          {selectedRows.length > 0 && (
            <Stack justifyContent="flex-end" direction="row">
              <Tooltip arrow title="Delete">
                <IconButton
                  disabled={discrubCancelled}
                  onClick={() => setDeleteModalOpen(true)}
                >
                  <DeleteIcon />
                </IconButton>
              </Tooltip>
              <Tooltip arrow title="Edit">
                <IconButton
                  disabled={discrubCancelled}
                  onClick={() => setEditModalOpen(true)}
                >
                  <EditIcon />
                </IconButton>
              </Tooltip>
            </Stack>
          )}
        </Stack>
      </Stack>
    </Toolbar>
  );
};

default MessageTableToolbar;

} // namespace elizaos
