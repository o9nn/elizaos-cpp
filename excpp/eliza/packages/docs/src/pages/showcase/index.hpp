#include "_components/ShowcaseCards.hpp"
#include "_components/ShowcaseFilters.hpp"
#include "_components/ShowcaseLayout.hpp"
#include "_components/ShowcaseLayout/styles.module.css.hpp"
#include "_components/ShowcaseSearchBar.hpp"
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

import React, { useState, useMemo } from 'react';
;
;
;
;
;
;
;
;

const TITLE = 'elizaOS Packages';
const DESCRIPTION = 'Discover the awesome plugins in the eliza ecosystem.';
const GITHUB_LINK = 'https://github.com/elizaos-plugins/registry';

</h1>
      <p>{DESCRIPTION}</p>
      <div className={styles.submitButton}>
        <Link className="button button--primary" to={GITHUB_LINK}>
          Submit your plugin
        </Link>
      </div>
    </section>
  );
}


    }

    // Tags filter
    if (selectedTags.length === 0) {
      return true;
    }

    if (operator === 'AND') {
      return selectedTags.every((tag) => user.tags.includes(tag));
    }
    return selectedTags.some((tag) => user.tags.includes(tag));
  });
}

default ;

  const toggleOperator = () => {
    setOperator((op) => (op === 'OR' ? 'AND' : 'OR'));
  };

  const filteredUsers = useMemo(() => {
    return filterUsers(sortedUsers, searchValue, selectedTags, operator);
  }, [searchValue, selectedTags, operator]);

  return (
    <Layout title={TITLE} description={DESCRIPTION}>
      <main className="margin-vert--lg">
        <ShowcaseHeader />

        <div className="container">
          <ShowcaseLayout selectedTags={selectedTags} toggleTag={toggleTag}>
            <div className={styles.filtersContainer}>
              <div className={styles.filtersRow}>
                <ShowcaseFilters
                  selectedTags={selectedTags}
                  toggleTag={toggleTag}
                  operator={operator}
                  toggleOperator={toggleOperator}
                />
                <ShowcaseSearchBar onChange={setSearchValue} value={searchValue} />
              </div>
            </div>

            <ShowcaseCards users={filteredUsers} />
          </ShowcaseLayout>
        </div>
      </main>
    </Layout>
  );
}

} // namespace elizaos
