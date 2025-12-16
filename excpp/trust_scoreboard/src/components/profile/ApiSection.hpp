#include "ApiSection.module.css.hpp"
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

const ApiSection: FC = () => {
  const router = useRouter();

  const handleNavigateToSaas = () => {
    router.push('/saas');
  };
  
  const handleNavigateToEliza = () => {
    router.push('/eliza');
  };

  const handleNavigateToCheckout = () => {
    router.push('/eliza');  // Points to eliza/index
  };

  return (
    <div>
      <h2 className={styles.title}></h2>
      <div className={styles.buttonParent}>
        <div 
          className={styles.button}
          onClick={handleNavigateToSaas}
        >
          <div className={styles.text}>
            Get API Access
          </div>
        </div>
        

        <div 
          className={`${styles.button} ${styles.checkoutButton}`}
          onClick={handleNavigateToCheckout}
        >
          <div className={styles.text}>
            Get an Eliza
          </div>
        </div>
      </div>
    </div>
  );
};

default ApiSection;

} // namespace elizaos
