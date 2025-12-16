#include ".utils/axios.hpp"
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

const fetchHighestRankedUsers = async () => {
  console.log("sdasdsdadasd=?????????");
  
  try {
    const response = await get('/user/highestRankedUsers');
    return response.data;
  } catch (error) {
    console.error('Error fetching highest ranked users:', error);
    throw error;
  }
};



struct Partner {
    double trustScore;
    std::optional<std::string> avatarUrl;
    std::optional<double> rank;
    std::optional<std::string> id;
    std::optional<std::string> name;
};


struct DashboardData {
    std::vector<Partner> partners;
};


const useDashboard = () => {
  const [data, setData] = useState<DashboardData | null>(null);
  const [isLoading, setIsLoading] = useState(true);
  const [error, setError] = useState<Error | null>(null);

  useEffect(() => {
    const fetchDashboard = async () => {
      try {
        const partners = await fetchHighestRankedUsers();
  
        
        setData({partners});
      } catch (err) {
        setError(err instanceof Error ? err : new Error('Failed to fetch dashboard data'));
      } finally {
        setIsLoading(false);
      }
    };

    fetchDashboard();
  }, []);

  

  return { data, isLoading, error };
};
} // namespace elizaos
