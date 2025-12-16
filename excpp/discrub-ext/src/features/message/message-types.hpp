#include "..classes/channel.hpp"
#include "..classes/message.hpp"
#include "..enum/filter-name.hpp"
#include "..enum/filter-type.hpp"
#include "..enum/has-type.hpp"
#include "..enum/sort-direction.hpp"
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

using MessageState = std::vector<{
  messages: Message>; // Message objects
  selectedMessages: Snowflake[]; // Array of id
  filteredMessages: Message[]; // Message objects
  filters: Filter[]; // Array of object filters
  isLoading: boolean | Maybe;
  order: SortDirection;
  orderBy: keyof Message;
  searchBeforeDate: Date | Maybe;
  searchAfterDate: Date | Maybe;
  searchMessageContent: string | Maybe;
  selectedHasTypes: HasType[];
};

using Filter = std::variant<, {
      filterName?: undefined>;
      filterValue: Snowflake | Maybe;
      filterType: FilterType.THREAD;
    }
  | {
      filterValue: string | string[] | Maybe;
      filterType: FilterType.TEXT;
      filterName:
        | FilterName.ATTACHMENT_NAME
        | FilterName.CONTENT
        | keyof Message;
    }
  | {
      filterValue: Date | Maybe;
      filterType: FilterType.DATE;
      filterName: FilterName.END_TIME | FilterName.START_TIME;
    }
  | {
      filterValue: boolean;
      filterType: FilterType.TOGGLE;
      filterName: FilterName.INVERSE;
    }
  | {
      filterName: FilterName.MESSAGE_TYPE;
      filterValue: string[];
      filterType: FilterType.ARRAY;
    };

using DeleteConfiguration = {
  attachments: boolean;
  messages: boolean;
};

using MessageData = std::vector<{
  threads: Channel>;
  messages: Message[];
};

using SearchMessageProps = std::variant<{
  preFilterUserId: string, Maybe>;
  searchAfterDate: Date | Maybe;
  searchBeforeDate: Date | Maybe;
  searchMessageContent: string | Maybe;
  selectedHasTypes: string[];
};

using MessageSearchOptions = std::variant<{
  preFilterUserId: string, Maybe>;
  excludeReactions: boolean | Maybe;
};

} // namespace elizaos
