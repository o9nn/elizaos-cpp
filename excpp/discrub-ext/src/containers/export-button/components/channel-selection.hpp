#include "...classes/channel.hpp"
#include "...common-components/tooltip/tooltip.hpp"
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

using ChannelSelectionProps = std::vector<{
  selectedExportChannels: Snowflake>;
  channels: Channel[];
  handleChannelSelect: (id: Snowflake) => void;
  onSelectAll: (channels: Channel[]) => void;
};

const ChannelSelection = ({
  selectedExportChannels,
  channels,
  handleChannelSelect,
  onSelectAll,
}: ChannelSelectionProps) => {
  const [filterValue, setFilterValue] = useState<string>("");

  const filteredChannels = filterValue
    ? channels.filter((c) => c.name && c.name.includes(filterValue))
    : channels;

  return (
    <Stack
      sx={{ width: "100%" }}
      direction="column"
      spacing={3}
      justifyContent="space-between"
    >
      <Box sx={{ maxHeight: "250px", overflow: "auto" }}>
        <List disablePadding dense>
          {filteredChannels.map((channel) => (
            <ListItem key={channel.id} value={channel.id} dense>
              <ListItemButton
                role={undefined}
                onClick={() => handleChannelSelect(channel.id)}
                dense
              >
                <ListItemIcon>
                  <Checkbox
                    size="small"
                    edge="start"
                    checked={selectedExportChannels.some(
                      (cId) => cId === channel.id
                    )}
                    tabIndex={-1}
                    disableRipple
                  />
                </ListItemIcon>
                <ListItemText primary={channel.name} />
              </ListItemButton>
            </ListItem>
          ))}
        </List>
      </Box>

      <Stack direction="row" spacing={3}>
        <Tooltip
          arrow
          title={selectedExportChannels.length ? "Deselect All" : "Select All"}
        >
          <IconButton
            onClick={() => onSelectAll(filteredChannels)}
            color={selectedExportChannels.length ? "secondary" : "primary"}
          >
            {selectedExportChannels.length ? (
              <DeselectIcon />
            ) : (
              <SelectAllIcon />
            )}
          </IconButton>
        </Tooltip>
        <TextField
          variant="filled"
          fullWidth
          size="small"
          label="Filter Channels"
          value={filterValue}
          onChange={(e) => setFilterValue(e.target.value)}
          InputProps={{
            endAdornment: (
              <IconButton
                onClick={(e) => {
                  e.preventDefault();
                  setFilterValue("");
                }}
              >
                <ClearIcon />
              </IconButton>
            ),
          }}
        />
      </Stack>
    </Stack>
  );
};

default ChannelSelection;

} // namespace elizaos
