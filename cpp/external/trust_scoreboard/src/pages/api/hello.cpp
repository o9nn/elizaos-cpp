#include "hello.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void handler(NextApiRequest req, NextApiResponse<Data> res) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    res.status(200).json({ name: "John Doe" })

}

} // namespace elizaos
