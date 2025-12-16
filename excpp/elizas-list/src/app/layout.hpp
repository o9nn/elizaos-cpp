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
import type { Metadata } from 'next';

const geistSans = GeistSans;
const geistMono = GeistMono;

const metadata: Metadata = {
  title: 'Eliza\'s List',
  description: 'A modern project discovery platform'
};

default : {
  children: React.ReactNode;
}) {
  return (
    <html lang="en" suppressHydrationWarning>
      <body 
        className={`${geistSans.variable} ${geistMono.variable} antialiased`}
      >
        <ThemeProvider 
          attribute="class" 
          defaultTheme="system"
          enableSystem
        >
          {children}
        </ThemeProvider>
      </body>
    </html>
  );
}

} // namespace elizaos
