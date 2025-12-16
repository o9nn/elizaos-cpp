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

using DateTimePickerProps = std::variant<TextFieldProps & {
  onDateChange: (val: Date, Maybe) => void>;
  value: Date | Maybe;
  label?: string;
};

const DateTimePicker = forwardRef<HTMLInputElement, DateTimePickerProps>(
  (props, ref) => {
    const { label, onDateChange, disabled, value = null, ...rest } = props;

    const [selectedDate, setSelectedDate] = useState<Date | Maybe>(value);

    useEffect(() => {
      setSelectedDate(value);
    }, [value]);

    return (
      <LocalizationProvider dateAdapter={AdapterDateFns}>
        <MobileDateTimePicker
          ref={ref}
          disabled={disabled}
          value={selectedDate}
          onChange={(e) => {
            setSelectedDate(e);
            onDateChange(e);
          }}
          slotProps={{
            dialog: {
              hideBackdrop: true,
            },
            textField: {
              size: "small",
              variant: "filled",
              fullWidth: true,
              label: label,
              ...rest,
              InputProps: {
                endAdornment: (
                  <IconButton
                    onClick={(e) => {
                      e.preventDefault();
                      setSelectedDate(null);
                      onDateChange(null);
                    }}
                  >
                    <ClearIcon />
                  </IconButton>
                ),
              },
            },
          }}
        />
      </LocalizationProvider>
    );
  }
);

default DateTimePicker;

} // namespace elizaos
