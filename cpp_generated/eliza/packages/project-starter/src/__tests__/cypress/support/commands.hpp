#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PROJECT-STARTER_SRC___TESTS___CYPRESS_SUPPORT_COMMANDS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PROJECT-STARTER_SRC___TESTS___CYPRESS_SUPPORT_COMMANDS_H
#include "core.h"

namespace global {
    class Window;
}
namespace global {
    namespace Cypress {
        class Chainable;
    }
}

namespace global {
    class Window : public object, public std::enable_shared_from_this<Window> {
    public:
        using std::enable_shared_from_this<Window>::shared_from_this;
        object ELIZA_CONFIG;
    };

}
namespace global {
    namespace Cypress {
        class Chainable : public object, public std::enable_shared_from_this<Chainable> {
        public:
            using std::enable_shared_from_this<Chainable>::shared_from_this;
            virtual std::shared_ptr<Chainable<std::shared_ptr<JQuery<std::shared_ptr<HTMLElement>>>>> shouldBeDarkMode() = 0;
            virtual std::shared_ptr<Chainable<std::shared_ptr<Window>>> setElizaConfig(object config) = 0;
        };

    }
}
namespace global {
}
namespace global {
    namespace Cypress {
    }
}
#endif
