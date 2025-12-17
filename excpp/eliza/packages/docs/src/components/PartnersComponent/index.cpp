#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

JSX::Element PartnersComponent() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Memoize the sorted partners to prevent recalculation on every render
    const auto sortedPartners = useMemo(() => {;
        return [...partners].sort((a, b) => a.title.localeCompare(b.title));
        }, []);

        return (;
        <div className={styles.partnersContainer}>;
        <div className={styles.partnersHeader}>;
        <h1>Our Partners</h1>;
        <p>;
        Discover the innovative projects and organizations we collaborate with to advance Web3;
        technology.;
        </p>;
        </div>;
        <div className={styles.partnersGrid}>;
        {sortedPartners.map((partner) => (;
        <PartnerCard key={partner.title} partner={partner} />;
    ))}
    </div>;
    </div>;
    );

}

} // namespace elizaos
