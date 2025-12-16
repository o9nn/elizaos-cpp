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



const useGetUsers = ({ cursor = 1, limit = 100 }) => {
  const [isLoading, setIsLoading] = useState(true)
  const [users, setUsers] = useState([])

  const getUsers = useCallback(async () => {
    const response = await fetch(
      `/api/user/getUsers?cursor=${cursor}&limit=${limit}`
    )
    setIsLoading(false)
    const data = await response.json()
    setUsers(data.users)
  }, [cursor, limit])

  useEffect(() => {
    getUsers()
  }, [getUsers])

  return { users, isLoading }
}

} // namespace elizaos
