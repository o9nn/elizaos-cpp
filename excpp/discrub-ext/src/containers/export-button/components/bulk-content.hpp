#include "...classes/channel.hpp"
#include "...enum/discrub-setting.hpp"
#include "...features/app/app-types.hpp"
#include "..discrub-dialog/components/config.hpp"
#include "channel-selection.hpp"
#include "progress.hpp"
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

using BulkContentProps = {
  isDm?: boolean;
  isExporting: boolean;
  selectedExportChannels: Snowflake[];
  channels: Channel[];
  setSelectedExportChannels: (ids: Snowflake[]) => void;
  settings: AppSettings;
  onChangeSettings: (settings: AppSettings) => void;
};

const BulkContent = ({
  isDm = false,
  isExporting,
  selectedExportChannels,
  channels,
  setSelectedExportChannels,
  settings,
  onChangeSettings,
}: BulkContentProps) => {
  const handleChannelSelect = (id: Snowflake) => {
    const isSelected = selectedExportChannels.some((cId) => cId === id);
    if (isSelected) {
      setSelectedExportChannels([
        ...selectedExportChannels.filter((cId) => cId !== id),
      ]);
    } else {
      setSelectedExportChannels([...selectedExportChannels, id]);
    }
  };

  const toggleSelectAll = (filteredChannels: Channel[]) => {
    setSelectedExportChannels(
      selectedExportChannels.length ? [] : filteredChannels.map((c) => c.id)
    );
  };
  let visibleSettings = [
    DiscrubSetting.EXPORT_ARTIST_MODE,
    DiscrubSetting.EXPORT_DOWNLOAD_MEDIA,
    DiscrubSetting.EXPORT_PREVIEW_MEDIA,
    DiscrubSetting.EXPORT_SEPARATE_THREAD_AND_FORUM_POSTS,
    DiscrubSetting.EXPORT_MESSAGE_SORT_ORDER,
    DiscrubSetting.EXPORT_MESSAGES_PER_PAGE,
    DiscrubSetting.EXPORT_IMAGE_RES_MODE,
  ];
  if (isDm) {
    visibleSettings = visibleSettings.filter(
      (s) => s !== DiscrubSetting.EXPORT_SEPARATE_THREAD_AND_FORUM_POSTS
    );
  }

  const getExportSettings = () => {
    return (
      <Stack
        mt={1}
        mb={1}
        sx={{
          maxHeight: "300px",
          overflow: "auto",
          width: "100%",
          overflowX: "hidden",
        }}
        direction="column"
        alignItems="center"
      >
        <Config
          onChangeSettings={onChangeSettings}
          visibleSettings={visibleSettings}
          settings={settings}
          containerProps={{ width: "100%" }}
        />
      </Stack>
    );
  };

  return (
    <DialogContent>
      {!isExporting && !isDm && (
        <>
          <DialogContentText>
            <Typography variant="body2">
              {selectedExportChannels.length
                ? `${selectedExportChannels.length} Channel${
                    selectedExportChannels.length === 1 ? "" : "s"
                  } selected`
                : "Select Channel(s) to export"}
            </Typography>
          </DialogContentText>
          <Stack direction="row" spacing={3}>
            <ChannelSelection
              channels={channels}
              selectedExportChannels={selectedExportChannels}
              handleChannelSelect={handleChannelSelect}
              onSelectAll={toggleSelectAll}
            />
            <Divider sx={{ height: "auto" }} orientation="vertical" />
            {getExportSettings()}
          </Stack>
        </>
      )}
      {!isExporting && isDm && (
        <>
          <Stack mt={1} mb={1}>
            {getExportSettings()}
          </Stack>
        </>
      )}
      {isExporting && <Progress />}
    </DialogContent>
  );
};

default BulkContent;

} // namespace elizaos
