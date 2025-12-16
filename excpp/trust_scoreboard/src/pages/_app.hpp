#include ".components/ContentContainer.hpp"
#include ".components/Notification.hpp"
#include ".components/nav-element/NavBar.hpp"
#include ".contexts/ContextProvider.hpp"
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










import "../styles/profile.css" // Adjust the path based on your project structure.
require("@solana/wallet-adapter-react-ui/styles.css")
require("../styles/globals.css")





const wallets = [new PhantomWalletAdapter(), new SolflareWalletAdapter()]

const App: FC<AppProps> = ({ Component, pageProps }) => {
  return (
    <SessionProvider session={pageProps.session}>
      <Head>
        <title>ai16z - Marc&apos;s Trust Leaderboard</title>
      </Head>

      <Script
        src="https://telegram.org/js/telegram-widget.js?22"
        strategy="afterInteractive"
        onLoad={() => {
          console.log("Telegram widget script loaded")
        }}
      />

      <ContextProvider>
        <div className="flex min-h-screen flex-col">
          <Notifications />
          <Component {...pageProps} />
        </div>
      </ContextProvider>
    </SessionProvider>
  )
}

default App

} // namespace elizaos
