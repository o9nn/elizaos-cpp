"use client";

import { AppProgressBar } from "next-nprogress-bar";

// Brand primary color for progress bar
const BRAND_PRIMARY = "#F75B1E";

export function ProgressBar() {
  return (
    <AppProgressBar
      height="1px"
      color={BRAND_PRIMARY}
      options={{ showSpinner: false }}
      shallowRouting
    />
  );
}
