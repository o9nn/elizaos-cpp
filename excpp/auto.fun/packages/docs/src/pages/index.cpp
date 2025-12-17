#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void HomepageHeader() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto {siteConfig} = useDocusaurusContext();
    return (;
    <header className={clsx("hero hero--primary", styles.heroBanner)}>;
    <div className="container">;
    <Heading as="h1" className="hero__title">;
    {siteConfig.title}
    </Heading>;
    <p className="hero__subtitle">{siteConfig.tagline}</p>;
    <div className={styles.buttons}>;
    <Link;
    className="button button--secondary button--lg";
    to="/docs/intro">;
    Docusaurus Tutorial - 5min ⏱️;
    </Link>;
    </div>;
    </div>;
    </header>;
    );

}

ReactNode Home() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto {siteConfig} = useDocusaurusContext();
    return (;
    <Layout;
    "title={" + "Hello from " + siteConfig.title;
    description="Description will go into a meta tag in <head />">;
    <HomepageHeader />;
    <main>;
    <HomepageFeatures />;
    </main>;
    </Layout>;
    );

}

} // namespace elizaos
