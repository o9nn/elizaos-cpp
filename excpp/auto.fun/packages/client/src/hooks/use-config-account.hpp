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
// The query key should be consistent
const configAccountQueryKey = ["configAccount"];

const getConfigAccount = async (program: Program<Autofun>) => {
  return queryClient.ensureQueryData({
    queryKey: configAccountQueryKey,
    queryFn: async () => {
      if (!program) {
        throw new Error("missing program");
      }

      const [configPda, _] = PublicKey.findProgramAddressSync(
        [Buffer.from(SEED_CONFIG)],
        program.programId,
      );
      const configAccount = await program.account.config.fetch(configPda);

      return configAccount;
    },
    staleTime: 1000 * 60 * 10, // 10 minutes
  });
};

} // namespace elizaos
