#include "...classes/channel.hpp"
#include "...classes/guild.hpp"
#include "...classes/message.hpp"
#include "...enum/message-type.hpp"
#include "...utils.hpp"
#include "..message-mock/message-mock.hpp"
#include "message-title-mock.hpp"
#include "page-footer-controls.hpp"
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

using ExportMessagesProps = std::variant<{
  componentRef: React.MutableRefObject<HTMLDivElement, null>>;
  isExporting: boolean;
  messages: Message[];
  entity: Channel | Guild | Maybe;
  currentPage: number;
  totalPages: number;
  safeEntityName: string;
};

const ExportMessages = ({
  componentRef,
  isExporting,
  messages,
  entity,
  currentPage,
  totalPages,
  safeEntityName,
}: ExportMessagesProps) => {
  const getExportPageTitle = (): string => {
    return `Page ${currentPage} of ${totalPages}`;
  };

  return (
    <Box sx={{ display: "none", margin: 0 }}>
      <Stack
        direction="column"
        alignItems="center"
        justifyContent="center"
        ref={componentRef}
      >
        <MessageTitleMock
          entity={entity}
          getExportPageTitle={getExportPageTitle}
        />
        <Stack
          direction="column"
          justifyContent="flex-start"
          alignItems="flex-start"
          spacing={1}
          sx={{ marginTop: "55px", width: "100%", marginBottom: "45px" }}
        >
          {isExporting &&
            messages.map((message, index) => {
              const previousMessage = messages[index - 1];

              let isChained = false;
              if (
                previousMessage &&
                !isNonStandardMessage(previousMessage) &&
                !isNonStandardMessage(message)
              ) {
                const elapsedSeconds = differenceInSeconds(
                  parseISO(message.timestamp),
                  parseISO(previousMessage.timestamp)
                );

                isChained =
                  Math.abs(elapsedSeconds) <= 330 &&
                  message.author.id === previousMessage.author.id &&
                  message.author.username === previousMessage.author.username &&
                  !messageTypeEquals(message.type, MessageType.REPLY) &&
                  previousMessage.channel_id === message.channel_id;
              }

              return (
                <MessageMock
                  message={message}
                  index={index}
                  isChained={isChained}
                />
              );
            })}
        </Stack>
        <PageFooterControls
          safeEntityName={safeEntityName}
          currentPage={currentPage}
          totalPages={totalPages}
        />
      </Stack>
    </Box>
  );
};
default ExportMessages;

} // namespace elizaos
