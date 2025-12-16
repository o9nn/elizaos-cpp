#include "loader.hpp"
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

default /tweets`, {
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
          <TableHead>Timestamp</TableHead>
          <TableHead>ID</TableHead>
          <TableHead>Username</TableHead>
          <TableHead className="w-[750px]">Tweet</TableHead>
          <TableHead className="text-center">Likes</TableHead>
          <TableHead className="text-center">Retweets</TableHead>
        </TableRow>
      </TableHeader>
      <TableBody>
        {query?.data?.map((item) => (
          <TableRow key={`${item._id}_${item.likes}`}>
            <TableCell>{moment(item.timestamp).format('LLL')}</TableCell>
            <TableCell>
              <div className="flex items-center gap-2">
                <div>{item.id}</div>
                <a
                  href={`https://x.com/${item.username}/status/${item.id}`}
                  target="_blank"
                  rel="noreferrer"
                >
                  <ExternalLink className="size-4 text-muted-foreground" />
                </a>
              </div>
            </TableCell>
            <TableCell>{item.username}</TableCell>
            <TableCell>
              <div className="line-clamp-1">{item.text}</div>
            </TableCell>
            <TableCell className="text-center">{item.likes}</TableCell>
            <TableCell className="text-center">{item.retweets}</TableCell>
          </TableRow>
        ))}
      </TableBody>
    </Table>
  );
}

} // namespace elizaos
