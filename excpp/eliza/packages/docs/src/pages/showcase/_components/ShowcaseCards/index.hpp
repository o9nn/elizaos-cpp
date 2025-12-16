#include ".ShowcaseCard.hpp"
#include "styles.module.css.hpp"
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
import type { User } from '@site/src/data/users';
;
;

default : { users: User[] }): JSX.Element {
  // Keep only unique entries by title
  const uniqueUsers = users.filter(
    (user, index, self) => index === self.findIndex((u) => u.title === user.title)
  );

  if (uniqueUsers.length === 0) {
    return (
      <section className="container margin-top--lg margin-bottom--xl">
        <h2>No results found</h2>
        <p>Try adjusting your search or filter criteria.</p>
      </section>
    );
  }

  return (
    <section className="container margin-top--lg margin-bottom--xl">
      <div className={clsx('margin-bottom--md')}>
        <h2>
          {uniqueUsers.length} {uniqueUsers.length === 1 ? 'Site' : 'Packages'}
        </h2>
      </div>
      <ul className={clsx('clean-list', styles.showcaseList)}>
        {uniqueUsers.map((user) => (
          <ShowcaseCard key={user.title + user.source} user={user} />
        ))}
      </ul>
    </section>
  );
}

} // namespace elizaos
