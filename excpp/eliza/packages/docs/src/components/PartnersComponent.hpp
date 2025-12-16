#include ".data/partners.hpp"
#include "PartnersComponent/styles.module.css.hpp"
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
import type { JSX } from 'react';
;
;
;
;

) {
  return (
    <Link to={`/partners/${partner.slug}/`} className={clsx('card', styles.partnerCard)}>
      <div className={styles.partnerImageContainer}>
        <img src={partner.preview} alt={partner.title} className={styles.partnerImage} />
      </div>
      <div className={styles.partnerContent}>
        <h3 className={styles.partnerTitle}>{partner.title}</h3>
        <p className={styles.partnerDescription}>{partner.description}</p>
      </div>
    </Link>
  );
}

default >
      <div className={styles.partnersGrid}>
        {sortedPartners.map((partner) => (
          <PartnerCard key={partner.title} partner={partner} />
        ))}
      </div>
    </div>
  );
}

} // namespace elizaos
