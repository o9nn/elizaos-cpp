import * as Headless from "@headlessui/react";
import clsx from "clsx";
import type React from "react";

const sizes = {
  xs: "sm:max-w-xs",
  sm: "sm:max-w-sm",
  md: "sm:max-w-md",
  lg: "sm:max-w-lg",
  xl: "sm:max-w-xl",
  "2xl": "sm:max-w-2xl",
  "3xl": "sm:max-w-3xl",
  "4xl": "sm:max-w-4xl",
  "5xl": "sm:max-w-5xl",
};

export function Dialog({
  size = "lg",
  className,
  children,
  variant = "modal",
  ...props
}: {
  size?: keyof typeof sizes;
  className?: string;
  children: React.ReactNode;
  variant?: "modal" | "slideout";
} & Omit<Headless.DialogProps, "as" | "className">) {
  return (
    <Headless.Dialog {...props}>
      {variant === "modal" && (
        <Headless.DialogBackdrop
          transition
          className="fixed inset-0 z-40 flex w-screen justify-center overflow-y-auto bg-zinc-950/25 px-2 py-2 transition duration-100 focus:outline-0 data-[closed]:opacity-0 data-[enter]:ease-out data-[leave]:ease-in sm:px-6 sm:py-8 lg:px-8 lg:py-16 dark:bg-zinc-950/50"
        />
      )}

      {variant === "modal" ? (
        <div className="fixed inset-0 z-50 isolate w-screen overflow-y-auto pt-2 pb-2 sm:pt-0 sm:pb-0">
          <div className="flex min-h-full items-center justify-center p-2 sm:p-4">
            <Headless.DialogPanel
              transition
              className={clsx(
                className,
                sizes[size],
                "w-full max-h-[calc(100dvh-16px)] sm:max-h-[calc(100dvh-32px)] overflow-y-auto",
              )}
            >
              {children}
            </Headless.DialogPanel>
          </div>
        </div>
      ) : (
        <Headless.DialogPanel
          transition
          className={clsx(
            className,
            "fixed inset-y-0 right-0 z-50 w-full overflow-y-auto bg-white dark:bg-surface transition duration-300 ease-in-out data-[closed]:translate-x-full",
            "sm:max-w-sm",
          )}
        >
          {children}
        </Headless.DialogPanel>
      )}
    </Headless.Dialog>
  );
}
