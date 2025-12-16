#include "layout/root.hpp"
#include "pages.hpp"
#include "pages/admin.hpp"
#include "pages/callback.hpp"
#include "pages/chat.hpp"
#include "pages/create.hpp"
#include "pages/fees.hpp"
#include "pages/not-found.hpp"
#include "pages/privacy-policy.hpp"
#include "pages/profile.hpp"
#include "pages/support.hpp"
#include "pages/terms-of-service.hpp"
#include "pages/testing.hpp"
#include "pages/token.hpp"
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
import "./index.css";
;
;
;
;
;
;
;
;
;
;
;
;
;
;

createRoot(document.getElementById("root")!).render(
  <HelmetProvider>
    <BrowserRouter>
      <Routes>
        <Route element={<Layout />}>
          <Route index element={<Homepage />} />
          <Route path="/support" element={<Support />} />
          <Route path="/terms-of-service" element={<TermsOfService />} />
          <Route path="/profile" element={<Profile />} />
          <Route path="/profiles/:address" element={<Profile />} />
          <Route path="/privacy-policy" element={<PrivacyPolicy />} />
          <Route path="/fees" element={<Fees />} />
          <Route path="/token/:address" element={<Token />} />
          <Route path="/create" element={<Create />} />
          <Route path="/testing" element={<Testing />} />
          <Route path="/callback" element={<CallbackPage />} />
          <Route path="/chat" element={<ChatPage />} />
          <Route path="/chat/:mint" element={<ChatPage />} />
          <Route path="/admin/*" element={<Admin />} />
          <Route path="*" element={<PageNotFound />} />
        </Route>
      </Routes>
    </BrowserRouter>
  </HelmetProvider>,
);

} // namespace elizaos
