#include "loader.hpp"
#include "ui/button.hpp"
#include "ui/input.hpp"
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

default /image-prompt`, {
        method: 'POST',
        headers: {
          Accept: '*/*',
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({ prompt }),
      });
      const buffer = await response.arrayBuffer();
      const blob = new Blob([buffer]);
      const url = URL.createObjectURL(blob);
      return url;
    },
    mutationKey: ['image_prompt'],
  });

  return (
    <div className="flex flex-col gap-4">
      <div className="flex items-center gap-2">
        <Input
          placeholder="Enter image prompt"
          value={prompt}
          onChange={({ target }) => setPrompt(target.value)}
        />
        <Button disabled={mutation?.isPending} onClick={() => mutation.mutate()}>
          Send
        </Button>
      </div>
      {mutation?.isPending ? <Loader /> : null}
      {mutation?.data ? (
        <img
          src={mutation?.data}
          width={750}
          height={750}
          className="mx-auto max-w-full"
          alt="degen"
        />
      ) : null}
    </div>
  );
}

} // namespace elizaos
