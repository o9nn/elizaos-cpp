#include "loader.hpp"
#include "ui/badge.hpp"
#include "ui/table.hpp"
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

default /sentiment`, {
        method: 'POST',
      });
      const data = await response.json();
      return data;
    },
    refetchInterval: 5_000,
  });

  if (query?.isPending) {
    return <Loader />;
  }

  return (
    <Table>
      <TableHeader>
        <TableRow>
          <TableHead className="w-[200px]">Timestamp</TableHead>
          <TableHead className="w-[400px]">Summary</TableHead>
          <TableHead>Tokens</TableHead>
        </TableRow>
      </TableHeader>
      <TableBody>
        {query?.data?.map((item) => (
          <TableRow key={item._id}>
            <TableCell className="font-medium text-muted-foreground">
              {moment(item.timeslot).format('LLL')}
            </TableCell>
            <TableCell>{item.text}</TableCell>
            <TableCell>
              <div className="flex flex-col gap-1 w-full">
                {item.occuringTokens.map((token) => (
                  <div className="flex items-center gap-2 w-full" key={item.token}>
                    <div className="w-24">{token.token}</div>
                    <div className="w-16">
                      <Badge variant={token.sentiment >= 0 ? 'success' : 'destructive'}>
                        {token.sentiment}
                      </Badge>
                    </div>
                    <div>{token.reason}</div>
                  </div>
                ))}
              </div>
            </TableCell>
          </TableRow>
        ))}
      </TableBody>
    </Table>
  );
}

} // namespace elizaos
