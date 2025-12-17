#include "PartnersComponent.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void PartnerCard() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    "/partners/" + std::to_string(partner.slug) + "/";
    <div className={styles.partnerImageContainer}>;
    <img src={partner.preview} alt={partner.title} className={styles.partnerImage} />;
    </div>;
    <div className={styles.partnerContent}>;
    <h3 className={styles.partnerTitle}>{partner.title}</h3>;
    <p className={styles.partnerDescription}>{partner.description}</p>;
    </div>;
    </Link>;
    );

}

JSX::Element PartnersComponent() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <div className={styles.partnersContainer}>;
    <div className={styles.partnersGrid}>;
    {sortedPartners.map((partner) => (;
    <PartnerCard key={partner.title} partner={partner} />;
    ))}
    </div>;
    </div>;
    );

}

} // namespace elizaos
