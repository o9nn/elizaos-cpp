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

'use client';

;
;
;
;
;


        indexName="projects"
      >
        <SearchBox
          placeholder="Search projects..."
          className="w-full px-4 py-2 rounded-lg border focus:ring-2 focus:ring-blue-500"
          onFocus={() => setIsOpen(true)}
        />

        <AnimatePresence>
          {isOpen && (
            <motion.div
              initial={{ opacity: 0, y: 10 }}
              animate={{ opacity: 1, y: 0 }}
              exit={{ opacity: 0, y: 10 }}
              className="absolute top-full left-0 right-0 mt-2 bg-white dark:bg-gray-800 rounded-lg shadow-xl z-50 max-h-96 overflow-y-auto"
            >
              <Hits
                hitComponent={({ hit }) => (
                  <div
                    className="p-4 hover:bg-gray-100 dark:hover:bg-gray-700 cursor-pointer"
                    onClick={() => {
                      router.push(`/projects/${hit.objectID}`);
                      setIsOpen(false);
                    }}
                  >
                    <div className="font-semibold">{hit.name}</div>
                    <div className="text-sm text-gray-500">{hit.description}</div>
                    <div className="flex gap-2 mt-2">
                      {hit.tags.map((tag: string) => (
                        <span
                          key={tag}
                          className="px-2 py-1 text-xs bg-blue-100 text-blue-800 rounded-full"
                        >
                          {tag}
                        </span>
                      ))}
                    </div>
                  </div>
                )}
              />
            </motion.div>
          )}
        </AnimatePresence>
      </InstantSearch>

      {isOpen && (
        <div
          className="fixed inset-0 z-40"
          onClick={() => setIsOpen(false)}
        />
      )}
    </div>
  );
} 
} // namespace elizaos
