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

const useInterval = ({
  ms,
  resolver,
}: {
  ms: number;
  resolver: any;
}) => {
  const [value, setValue] = useState(resolver());
  useEffect(() => {
    const interval = setInterval(() => {
      setValue(resolver());
    }, ms);

    return () => {
      clearInterval(interval);
    };
  }, [resolver, ms]);

  return value;
};

} // namespace elizaos
