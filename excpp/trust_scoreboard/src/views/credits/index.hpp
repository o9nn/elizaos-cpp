#include "..components/saas/BuyCredits.hpp"
#include "index.module.css.hpp"
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

const CreditsView: FC = () => {
  return (
    <div className={styles.container}>
      <div className={styles.hero}>
        <h1 className={styles.heroTitle}>Request Credits</h1>
        <p className={styles.heroSubtitle}>
          Purchase credits to increase your request limit
        </p>
      </div>
      <BuyCredits />
      <div className={styles.features}>
        <h2 className={styles.featuresTitle}>Why Buy Credits?</h2>
        <div className={styles.featureGrid}>
          <div className={styles.feature}>
            <h3>Flexible Usage</h3>
            <p>Pay only for what you need</p>
          </div>
          <div className={styles.feature}>
            <h3>Extended Validity</h3>
            <p>Credits valid for up to 90 days</p>
          </div>
          <div className={styles.feature}>
            <h3>Volume Savings</h3>
            <p>Better rates for larger packages</p>
          </div>
          <div className={styles.feature}>
            <h3>Instant Access</h3>
            <p>Start using credits immediately</p>
          </div>
        </div>
      </div>
    </div>
  );
};

default CreditsView;
} // namespace elizaos
