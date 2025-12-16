#include "...enum/discrub-setting.hpp"
#include "...features/app/app-types.hpp"
#include "..discrub-dialog/components/config.hpp"
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

using DefaultContentProps = {
  isExporting: boolean;
  messageCount: number;
  isDm: boolean;
  settings: AppSettings;
  onChangeSettings: (settings: AppSettings) => void;
};

const DefaultContent = ({
  isExporting,
  messageCount,
  settings,
  onChangeSettings,
  isDm,
}: DefaultContentProps) => {
  let visibleSettings = [
    DiscrubSetting.EXPORT_ARTIST_MODE,
    DiscrubSetting.EXPORT_DOWNLOAD_MEDIA,
    DiscrubSetting.EXPORT_PREVIEW_MEDIA,
    DiscrubSetting.EXPORT_SEPARATE_THREAD_AND_FORUM_POSTS,
    DiscrubSetting.EXPORT_MESSAGES_PER_PAGE,
    DiscrubSetting.EXPORT_IMAGE_RES_MODE,
  ];
  if (isDm) {
    visibleSettings = visibleSettings.filter(
      (s) => s !== DiscrubSetting.EXPORT_SEPARATE_THREAD_AND_FORUM_POSTS
    );
  }

  return (
    <DialogContent>
      {!isExporting && (
        <>
          <DialogContentText>
            <Typography variant="body2">
              <strong>{messageCount}</strong> messages are available to </Typography>
          </DialogContentText>
          <Stack mt={1} mb={1} sx={{ maxHeight: "325px", overflow: "auto" }}>
            <Config
              onChangeSettings={onChangeSettings}
              visibleSettings={visibleSettings}
              settings={settings}
              containerProps={{ width: "auto" }}
            />
          </Stack>
        </>
      )}
      {isExporting && <Progress />}
    </DialogContent>
  );
};

default DefaultContent;

} // namespace elizaos
