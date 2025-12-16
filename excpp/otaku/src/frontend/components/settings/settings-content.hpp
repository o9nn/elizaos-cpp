#include "notification-settings.hpp"
#include "preferences-settings.hpp"
#include "profile-settings.hpp"
#include "security-settings.hpp"
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

"use client"









type SettingsSection = "profile" | "preferences" | "notifications" | "security"

const sections = [
  { id: "profile" as const, label: "Profile" },
  { id: "preferences" as const, label: "Preferences" },
  { id: "notifications" as const, label: "Notifications" },
  { id: "security" as const, label: "Security" },
]


      <nav className="lg:w-64 shrink-0">
        <div className="sticky top-32 space-y-1 bg-card ring-2 ring-border rounded-lg p-4">
          <div className="flex items-center gap-2 mb-4 text-xs uppercase text-muted-foreground">
            <Bullet />
            <span>Settings</span>
          </div>
          {sections.map((section) => (
            <button
              key={section.id}
              onClick={() => setActiveSection(section.id)}
              className={cn(
                "w-full text-left px-4 py-2.5 rounded-md text-sm transition-colors",
                activeSection === section.id
                  ? "bg-primary text-primary-foreground font-medium"
                  : "hover:bg-accent text-foreground",
              )}
            >
              {section.label}
            </button>
          ))}
        </div>
      </nav>

      {/* Main Content */}
      <div className="flex-1 min-w-0">
        {activeSection === "profile" && <ProfileSettings />}
        {activeSection === "preferences" && <PreferencesSettings />}
        {activeSection === "notifications" && <NotificationSettings />}
        {activeSection === "security" && <SecuritySettings />}
      </div>
    </div>
  )
}

} // namespace elizaos
