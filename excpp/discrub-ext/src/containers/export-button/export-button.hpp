#include "..classes/channel.hpp"
#include "..enum/export-type.hpp"
#include "..enum/media-type.hpp"
#include "..features/app/use-app-slice.hpp"
#include "..features/channel/use-channel-slice.hpp"
#include "..features/dm/use-dm-slice.hpp"
#include "..features/export/export-utils.hpp"
#include "..features/export/use-export-slice.hpp"
#include "..features/guild/use-guild-slice.hpp"
#include "..features/message/use-message-slice.hpp"
#include "..utils.hpp"
#include "components/bulk-content.hpp"
#include "components/default-content.hpp"
#include "components/export-messages.hpp"
#include "components/export-modal.hpp"
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

using ExportButtonProps = {
  bulk?: boolean;
  isDm?: boolean;
  disabled?: boolean;
};

const ExportButton = ({
  isDm = false,
  bulk = false,
  disabled = false,
}: ExportButtonProps) => {
  const {
    state: exportState,
    setIsGenerating,
    exportMessages,
    exportChannels,
  } = useExportSlice();
  const isGenerating = exportState.isGenerating();
  const isExporting = exportState.isExporting();
  const currentPage = exportState.currentPage();
  const activeExportMessages = exportState.exportMessages();
  const totalPages = exportState.totalPages();
  const entity = exportState.currentExportEntity();
  const name = exportState.name();

  const {
    setDiscrubCancelled,
    setDiscrubPaused,
    setSettings,
    state: appState,
  } = useAppSlice();
  const settings = appState.settings();
  const folderingThreads = stringToBool(
    settings.exportSeparateThreadAndForumPosts
  );
  const artistMode = stringToBool(settings.exportUseArtistMode);
  const previewMedia = stringToTypedArray<MediaType>(
    settings.exportPreviewMedia_2
  );
  const isPreviewingImages = previewMedia.some((mt) => mt === MediaType.IMAGES);
  const isPreviewingVideos = previewMedia.some((mt) => mt === MediaType.VIDEOS);
  const isPreviewingAudio = previewMedia.some((mt) => mt === MediaType.AUDIO);

  const downloadMedia = stringToTypedArray<MediaType>(
    settings.exportDownloadMedia_2
  );
  const isDownloadingImages = downloadMedia.some(
    (mt) => mt === MediaType.IMAGES
  );
  const isDownloadingVideos = downloadMedia.some(
    (mt) => mt === MediaType.VIDEOS
  );
  const isDownloadingAudio = downloadMedia.some((mt) => mt === MediaType.AUDIO);

  const { state: channelState, setSelectedExportChannels } = useChannelSlice();
  const channels = channelState.channels();
  const selectedExportChannels = channelState.selectedExportChannels();
  const selectedChannel = channelState.selectedChannel();

  const { state: guildState } = useGuildSlice();
  const selectedGuild = guildState.selectedGuild();
  const userId = guildState.preFilterUserId();

  const { state: dmState } = useDmSlice();
  const selectedDms = dmState.selectedDms();

  const { state: messageState } = useMessageSlice();
  const messages = messageState.messages();
  const filteredMessages = messageState.filteredMessages();
  const filters = messageState.filters();

  const [dialogOpen, setDialogOpen] = useState(false);

  const exportType = isDm
    ? `DM${selectedDms.length > 1 ? "'s" : ""}`
    : "Server";

  const contentRef = useRef<HTMLDivElement | null>(null);

  const getZipName = () => {
    if (isDm) {
      return bulk ? "Direct Messages" : String(selectedDms[0]?.name);
    } else {
      return String(selectedGuild?.name);
    }
  };

  const exportUtils = new ExportUtils(
    contentRef,
    (e: boolean) => setIsGenerating(e),
    getZipName()
  );

  const handleDialogClose = () => {
    if (isGenerating || isExporting) {
      // We are actively exporting, we need to send a cancel request
      setDiscrubCancelled(true);
    }
    if (bulk) {
      setSelectedExportChannels([]);
    }
    setDiscrubPaused(false);
    setDialogOpen(false);
  };

  const handleExportSelected = async (format: ExportType = ExportType.JSON) => {
    if (bulk) {
      let channelsToExport: Channel[] = [];
      if (isDm && selectedDms.length) {
        channelsToExport = [...selectedDms];
      } else if (selectedGuild) {
        channelsToExport = channels.filter((c) =>
          selectedExportChannels.some((id) => id === c.id)
        );
      }
      if (channelsToExport.length) {
        exportChannels(
          channelsToExport,
          exportUtils,
          format,
          userId || undefined
        );
      }
    } else {
      const entity = isDm ? selectedDms[0] : selectedChannel || selectedGuild;
      const messagesToExport = filters.length ? filteredMessages : messages;
      if (entity && messagesToExport.length) {
        exportMessages(
          messagesToExport,
          entity.name || entity.id,
          exportUtils,
          format
        );
      }
    }
  };

  const exportDisabled =
    isExporting || (bulk && !isDm && selectedExportChannels.length === 0);
  const pauseDisabled = !isExporting;

  const getContentComponent = (): React.ReactNode => {
    if (bulk) {
      return (
        <BulkContent
          isDm={isDm}
          isExporting={isExporting}
          selectedExportChannels={selectedExportChannels}
          channels={channels}
          setSelectedExportChannels={setSelectedExportChannels}
          settings={settings}
          onChangeSettings={setSettings}
        />
      );
    } else
      return (
        <DefaultContent
          isDm={isDm}
          isExporting={isExporting}
          messageCount={
            filters.length ? filteredMessages?.length : messages.length
          }
          settings={settings}
          onChangeSettings={setSettings}
        />
      );
  };

  const exportTitle = `Export ${bulk ? exportType : "Messages"}`;

  const getTooltipDescription = (exportType: ExportType): string => {
    const descriptionArr: string[] = [];

    if (exportType !== ExportType.MEDIA) {
      const exportAccessories: string[] = [
        `${exportType.toUpperCase()} Format`,
      ];

      if (exportType === ExportType.HTML) {
        const previewArr = [];
        if (isPreviewingImages) {
          previewArr.push("Images");
        }
        if (isPreviewingVideos) {
          previewArr.push("Videos");
        }
        if (isPreviewingAudio) {
          previewArr.push("Audio");
        }
        if (previewArr.length) {
          exportAccessories.push(`${punctuateStringArr(previewArr)} Previewed`);
        }
      }

      if (folderingThreads) {
        exportAccessories.push("Threads & Forum Posts Foldered");
      }

      const accessory = punctuateStringArr(exportAccessories);
      descriptionArr.push(
        `Messages ${accessory.length ? `(${accessory})` : ""}`
      );
    }

    if (downloadMedia.length) {
      const downloadArr = [];
      if (isDownloadingImages) {
        downloadArr.push("Images");
      }
      if (isDownloadingVideos) {
        downloadArr.push("Videos");
      }
      if (isDownloadingAudio) {
        downloadArr.push("Audio");
      }
      descriptionArr.push(
        `Attached & Embedded ${punctuateStringArr(downloadArr)}${
          artistMode ? " (Artist Mode)" : ""
        }`
      );
    }
    if (!isDm) {
      descriptionArr.push("Roles");
    }
    return `${descriptionArr.join(", ")}${
      descriptionArr.length ? ", " : ""
    }Emojis, and Avatars`;
  };

  return (
    <>
      <Button
        disabled={disabled}
        onClick={async () => {
          setDialogOpen(true);
        }}
        variant="contained"
      >
        {exportTitle}
      </Button>
      {isGenerating && (
        <ExportMessages
          messages={activeExportMessages}
          componentRef={contentRef}
          isExporting={isExporting}
          entity={entity}
          currentPage={currentPage}
          totalPages={totalPages}
          safeEntityName={name}
        />
      )}
      <ExportModal
        onCancel={handleDialogClose}
        handleExportSelected={handleExportSelected}
        dialogOpen={dialogOpen}
        exportDisabled={exportDisabled}
        pauseDisabled={pauseDisabled}
        ContentComponent={getContentComponent()}
        dialogTitle={exportTitle}
        getTooltipDescription={getTooltipDescription}
      />
    </>
  );
};

default ExportButton;

} // namespace elizaos
