#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void Feature(auto {title, auto Svg, FeatureItem description}) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <div className={clsx('col col--4')}>;
    <div className="text--center">;
    <Svg className={styles.featureSvg} role="img" />;
    </div>;
    <div className="text--center padding-horiz--md">;
    <Heading as="h3">{title}</Heading>;
    <p>{description}</p>;
    </div>;
    </div>;
    );

}

ReactNode HomepageFeatures() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <section className={styles.features}>;
    <div className="container">;
    <div className="row">;
    {FeatureList.map((props, idx) => (;
    <Feature key={idx} {...props} />;
    ))}
    </div>;
    </div>;
    </section>;
    );

}

} // namespace elizaos
