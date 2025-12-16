#include "....data/users.hpp"
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
;
;
;

: { label: string; color: string }) {
  return (
    <span
      style={{
        backgroundColor: color,
        width: 7,
        height: 7,
        borderRadius: '50%',
        marginLeft: 2,
        marginRight: 6,
      }}
    />
  );
}


}

: { tags: TagType[] }) {
  return (
    <>
      {tags.map((tag) => {
        const { label, color } = Tags[tag];
        const displayLabel = getShortLabel(label);

        return (
          <li key={tag} className={styles.tag} title={label}>
            <TagIcon label={label} color={color} />
            <span className={styles.textLabel}>{displayLabel.toLowerCase()}</span>
          </li>
        );
      })}
    </>
  );
}

default : { user: User }) {
  // Function to convert title to documentation link
  const getDocLink = (user: User) => {
    const baseUrl = '/packages/'; // Make sure to include your baseUrl prefix
    let category = '';

    if (user.tags.includes('adapter')) {
      category = 'adapters/';
    } else if (user.tags.includes('client')) {
      category = 'clients/';
    } else if (user.tags.includes('plugin')) {
      category = 'plugins/';
    }

    // Extract package name from title
    // Convert to lowercase and remove spaces/special chars
    const slug = user.title.toLowerCase().replace(/[^a-z0-9]+/g, '-');

    return `${baseUrl}${category}${slug}`;
  };

  return (
    <li key={user.title} className="card shadow--md">
      <div className={clsx('card__image', styles.showcaseCardImage)}>
        {user.preview && (
          <img src={user.preview} alt={user.title} loading="lazy" className={styles.cardImage} />
        )}
      </div>
      <div className="card__body">
        <div className={styles.showcaseCardHeader}>
          <h4 className={styles.showcaseCardTitle}>
            <Link href={user.website} className={styles.showcaseCardLink}>
              {user.title}
            </Link>
          </h4>
          {user.source && (
            <Link
              href={user.source}
              className={clsx('button button--secondary button--sm', styles.showcaseCardSrcBtn)}
            >
              source
            </Link>
          )}
        </div>
        <p className={styles.showcaseCardBody}>{user.description}</p>
      </div>
      <div className={clsx('card__footer', styles.cardFooter)}>
        <div className={styles.tagContainer}>
          <ShowcaseCardTag tags={user.tags.filter((tag) => tag)} />
        </div>
        <Link
          to={getDocLink(user)}
          className={clsx('button button--primary button--sm', styles.readmeBtn)}
        >
          Readme
        </Link>
      </div>
    </li>
  );
}

} // namespace elizaos
