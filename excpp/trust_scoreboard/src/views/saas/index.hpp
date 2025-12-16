#include "..components/saas/Pricing.hpp"
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

const SaasView: FC = () => {
  return (
    <div className={styles.container}>
      <div className={styles.hero}>
        <h1 className={styles.heroTitle}>Trust Score API</h1>
        <p className={styles.heroSubtitle}>
          Integrate trust scores directly into your application
        </p>
      </div>
      <Pricing />
      <div className={styles.features}>
        <h2 className={styles.featuresTitle}>Why Choose Our API?</h2>
        <div className={styles.featureGrid}>
          <div className={styles.feature}>
            <h3>Real-time Updates</h3>
            <p>Get the latest trust scores as they change</p>
          </div>
          <div className={styles.feature}>
            <h3>Easy Integration</h3>
            <p>Simple REST API with comprehensive documentation</p>
          </div>
          <div className={styles.feature}>
            <h3>Reliable Service</h3>
            <p>99.9% uptime guarantee with enterprise SLA</p>
          </div>
          <div className={styles.feature}>
            <h3>Secure Access</h3>
            <p>Enterprise-grade security and encryption</p>
          </div>
        </div>
      </div>
    </div>
  );
};

default SaasView;
} // namespace elizaos
