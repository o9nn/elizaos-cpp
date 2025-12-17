#include "ProjectNotification.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void ProjectNotification(ProjectNotificationProps { project }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <Html>;
    <Head />;
    <Body style={main}>;
    <Container style={container}>;
    <Heading style={h1}>New Project on ELIZA'S LIST</Heading>;
    <Text style={text}>;
    A new project has been added to ELIZA'S LIST:
    </Text>;
    <Heading style={h2}>{project.name}</Heading>;
    <Text style={text}>{project.description}</Text>;
    <Text style={text}>;
    By: {project.author}
    </Text>;
    <Link href={project.url} style={button}>;
    View Project;
    </Link>;
    </Container>;
    </Body>;
    </Html>;
    );

}

} // namespace elizaos
